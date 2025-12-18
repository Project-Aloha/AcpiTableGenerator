#!/usr/bin/env python3
"""
Device Tree parser - generate PPTT headers

Parse CPU topology and cache information from a DTB and generate platform headers.
Uses pyfdt for device tree parsing.
"""

import sys
import argparse
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from dataclasses import dataclass, field

try:
    from pyfdt.pyfdt import FdtBlobParse, FdtNode, FdtProperty
except ImportError:
    print("Error: pyfdt library not installed")
    print("Please run: pip install -r requirements.txt")
    sys.exit(1)


@dataclass
class CacheInfo:
    """Cache information"""
    level: int
    size: Optional[int] = None
    line_size: Optional[int] = None
    sets: Optional[int] = None
    ways: Optional[int] = None
    cache_type: Optional[str] = None  # "data", "instruction", "unified"
    next_level: Optional[str] = None
    phandle: Optional[int] = None
    owners: List[str] = field(default_factory=list)
    scope: Optional[str] = None  # 'private', 'cluster', 'system' etc.


@dataclass
class CPUInfo:
    """CPU information"""
    node_name: str
    reg: int
    capacity: Optional[int] = None
    l1d: Optional[CacheInfo] = None
    l1i: Optional[CacheInfo] = None
    l2: Optional[CacheInfo] = None
    cluster: Optional[str] = None
    core_type: Optional[str] = None  # P / E / M or None, inferred from DTB capacities if available


@dataclass
class ClusterInfo:
    """Cluster information"""
    name: str
    cpus: List[CPUInfo] = field(default_factory=list)


class DTBParser:
    """DTB parser (using pyfdt)"""
    
    def __init__(self, dtb_path: Path):
        self.dtb_path = dtb_path
        with open(dtb_path, 'rb') as f:
            self.fdt = FdtBlobParse(f).to_fdt()
        self.root = self.fdt.get_rootnode()
        self.cpus: List[CPUInfo] = []
        self.clusters: List[ClusterInfo] = []
        self.l3_cache: Optional[CacheInfo] = None
        self.phandle_map: Dict[int, CPUInfo] = {}
        self.cache_map: Dict[int, CacheInfo] = {}  # phandle -> CacheInfo
    
    def parse(self):
        """Parse the DTB file."""
        # Collect global cache nodes first (e.g., standalone shared cache nodes)
        self._collect_global_cache_nodes()
        self._parse_cpus()
        self._parse_clusters()
        # Analyze cache owners and sharing scope
        self._analyze_cache_sharing()
        self._infer_cache_info()

    def _collect_global_cache_nodes(self):
        """Walk all nodes, find cache nodes and record them in cache_map by phandle."""
        def walk(node):
            for sub in self._get_subnodes(node):
                # Heuristic: treat nodes with 'cache-level' or compatible including 'arm,arch-cache' as cache nodes
                comp = self._get_prop_value(sub, 'compatible')
                has_cache_level = self._get_prop_value(sub, 'cache-level') is not None
                is_cache_compat = False
                if comp:
                    if isinstance(comp, list):
                        is_cache_compat = any('arm,arch-cache' in str(s) for s in comp)
                    else:
                        is_cache_compat = 'arm,arch-cache' in str(comp)

                if has_cache_level or is_cache_compat:
                    # Parse as cache node (ignore if no phandle)
                    ci = None
                    try:
                        ci = self._parse_cache_node(sub, int(self._parse_u32(self._get_prop_value(sub, 'cache-level') or 0)))
                    except Exception:
                        ci = self._parse_cache_node(sub, 0)
                    if ci.phandle:
                        self.cache_map.setdefault(ci.phandle, ci)
                walk(sub)
        walk(self.root)

    def _analyze_cache_sharing(self):
        """Determine cache sharing scope (private/cluster/system) from owners and cluster distribution."""
        # build a cpu node_name -> cluster mapping
        cpu_to_cluster = {c.node_name: (c.cluster if c.cluster else None) for c in self.cpus}

        for ph, ci in list(self.cache_map.items()):
            owners = ci.owners
            # Deduplicate
            owners = list(dict.fromkeys(owners))
            ci.owners = owners
            if not owners:
                ci.scope = 'unknown'
                continue
            # If owners include "system" or cross-cluster cpus, treat as system-shared
            clusters = set()
            cpu_owners = []
            for o in owners:
                if o == 'system':
                    clusters.add('system')
                elif o in cpu_to_cluster and cpu_to_cluster[o]:
                    clusters.add(cpu_to_cluster[o])
                    cpu_owners.append(o)
                else:
                    # May be cluster name or unknown string
                    if o.startswith('cluster'):
                        clusters.add(o)
                    else:
                        # If cpu node name without cluster info, treat as independent owner
                        cpu_owners.append(o)
            if 'system' in clusters or len(clusters) > 1:
                ci.scope = 'system'
            elif len(owners) == 1 and owners[0].startswith('cpu@'):
                ci.scope = 'private'
            else:
                # Multiple cpus in same cluster or cluster-name owners
                ci.scope = 'cluster'
    
    def _get_prop_value(self, node, prop_name: str):
        """Get a property value (return words list or raw data)."""
        for item in node:
            if not isinstance(item, FdtNode) and item.get_name() == prop_name:
                # FdtPropertyWords has 'words'; FdtPropertyStrings uses 'strings'
                if hasattr(item, 'words'):
                    return item.words
                elif hasattr(item, 'strings'):
                    return item.strings
                return None
        return None
    
    def _get_subnodes(self, node):
        """Return child nodes (FdtNode instances)."""
        return [item for item in node if isinstance(item, FdtNode)]
    
    def _parse_u32(self, data) -> int:
        """Parse a u32 value from words or bytes."""
        if data is None:
            return 0
        # pyfdt returns words as integer lists
        if isinstance(data, list):
            return data[0] if data else 0
        # handle bytes
        if isinstance(data, bytes):
            if len(data) < 4:
                return 0
            return int.from_bytes(data[:4], byteorder='big')
        return int(data)
    
    def _parse_u32_array(self, data) -> List[int]:
        """Parse an array of u32 values."""
        if data is None:
            return []
        # pyfdt returns words as integer lists
        if isinstance(data, list):
            return data
        # handle bytes
        if isinstance(data, bytes):
            values = []
            for i in range(0, len(data), 4):
                if i + 4 <= len(data):
                    values.append(int.from_bytes(data[i:i+4], byteorder='big'))
            return values
        return [int(data)]
    
    def _find_node(self, node, name: str):
        """Find a child node by name (supports name and name@addr)."""
        for subnode in self._get_subnodes(node):
            node_name = subnode.get_name()
            if node_name == name or node_name.startswith(name + '@'):
                return subnode
        return None

    def _get_any_prop(self, node, names):
        """Return the first existing property value among candidate names."""
        for n in names:
            val = self._get_prop_value(node, n)
            if val is not None:
                return val
        return None

    def _parse_cache_node(self, node, level: int, default_type: Optional[str] = None) -> CacheInfo:
        """Parse CacheInfo from a cache subnode (use DT fields when present).

        Also read phandle and initialize owners list for sharing analysis.
        """
        ci = CacheInfo(level=level)

        size = self._get_any_prop(node, ['cache-size', 'size', 'cache_size', 'cache-size-bytes'])
        if size is not None:
            ci.size = self._parse_u32(size)

        line = self._get_any_prop(node, ['line-size', 'cache-line-size', 'line_size', 'cache_line_size'])
        if line is not None:
            ci.line_size = self._parse_u32(line)

        sets = self._get_any_prop(node, ['cache-sets', 'num-sets', 'sets'])
        if sets is not None:
            ci.sets = self._parse_u32(sets)

        ways = self._get_any_prop(node, ['ways', 'associativity', 'cache-associativity'])
        if ways is not None:
            ci.ways = self._parse_u32(ways)

        # Record type if known
        if default_type:
            ci.cache_type = default_type

        # Parse phandle if present
        ph = self._get_prop_value(node, 'phandle')
        if ph is not None:
            ci.phandle = self._parse_u32(ph)

        return ci
    
    def _parse_cpus(self):
        """Parse CPU nodes and collect cache subnode references (L2/L3). L1 is assumed to be per-core and not required."""
        cpus_node = self._find_node(self.root, 'cpus')
        if not cpus_node:
            return

        for cpu_node in self._get_subnodes(cpus_node):
            node_name = cpu_node.get_name()
            if not node_name.startswith('cpu@'):
                continue

            # Parse reg property
            reg_data = self._get_prop_value(cpu_node, 'reg')
            if not reg_data:
                continue

            reg_values = self._parse_u32_array(reg_data)
            reg = reg_values[1] if len(reg_values) > 1 else reg_values[0] if reg_values else 0

            cpu = CPUInfo(
                node_name=node_name,
                reg=reg
            )

            # Parse capacity-dmips-mhz
            cap_data = self._get_prop_value(cpu_node, 'capacity-dmips-mhz')
            if cap_data:
                cpu.capacity = self._parse_u32(cap_data)

            # Parse phandle
            phandle_data = self._get_prop_value(cpu_node, 'phandle')
            if phandle_data:
                phandle = self._parse_u32(phandle_data)
                self.phandle_map[phandle] = cpu

            # Check for next-level-cache property (reference to L2/L3)
            next_cache_ref = self._get_prop_value(cpu_node, 'next-level-cache')
            if next_cache_ref:
                next_cache_phandle = self._parse_u32(next_cache_ref)
                if next_cache_phandle in self.cache_map:
                    # Reference existing cache
                    ci = self.cache_map[next_cache_phandle]
                    if ci.level == 2:
                        cpu.l2 = ci
                    elif ci.level == 3:
                        cpu.l3 = ci
                        self.l3_cache = ci
                    # Add CPU as owner
                    if cpu.node_name not in ci.owners:
                        ci.owners.append(cpu.node_name)

            # Try parsing cache subnodes (L2/L3) and record cache_map
            for subnode in self._get_subnodes(cpu_node):
                subname = subnode.get_name().lower()
                # Skip explicit L1 parsing; L1 is assumed to be present per-core
                if 'l2' in subname:
                    ci = self._parse_cache_node(subnode, 2, 'unified')
                    cpu.l2 = ci
                    if ci.phandle:
                        existing = self.cache_map.setdefault(ci.phandle, ci)
                        if cpu.node_name not in existing.owners:
                            existing.owners.append(cpu.node_name)
                    # check l3 under l2
                    l3_node = self._find_node(subnode, 'l3-cache')
                    if l3_node:
                        l3ci = self._parse_cache_node(l3_node, 3, 'unified')
                        self.l3_cache = l3ci
                        if l3ci.phandle:
                            existing = self.cache_map.setdefault(l3ci.phandle, l3ci)
                            if 'system' not in existing.owners:
                                existing.owners.append('system')
                # L3 present directly
                elif 'l3' in subname:
                    l3ci = self._parse_cache_node(subnode, 3, 'unified')
                    self.l3_cache = l3ci
                    if l3ci.phandle:
                        existing = self.cache_map.setdefault(l3ci.phandle, l3ci)
                        if 'system' not in existing.owners:
                            existing.owners.append('system')

            self.cpus.append(cpu)

        # Sort by reg
        self.cpus.sort(key=lambda x: x.reg)
    
    def _parse_clusters(self):
        """Parse cpu-map / cluster information."""
        cpus_node = self._find_node(self.root, 'cpus')
        if not cpus_node:
            return
        
        # Find cpu-map node
        cpu_map = self._find_node(cpus_node, 'cpu-map')
        if not cpu_map:
            return
        
        # Iterate clusters
        for cluster_node in self._get_subnodes(cpu_map):
            cluster_name = cluster_node.get_name()
            if not cluster_name.startswith('cluster'):
                continue
            
            cluster = ClusterInfo(name=cluster_name)
            
            # Iterate cores
            for core_node in self._get_subnodes(cluster_node):
                if not core_node.get_name().startswith('core'):
                    continue
                
                # Get cpu reference (phandle)
                cpu_ref_data = self._get_prop_value(core_node, 'cpu')
                if not cpu_ref_data:
                    continue
                
                phandle = self._parse_u32(cpu_ref_data)
                
                # Find CPU by phandle
                if phandle in self.phandle_map:
                    cpu = self.phandle_map[phandle]
                    cpu.cluster = cluster_name
                    cluster.cpus.append(cpu)
            
            if cluster.cpus:
                self.clusters.append(cluster)

        # Infer core type (P/E/M) from capacity
        for cluster in self.clusters:
            caps = [c.capacity for c in cluster.cpus if c.capacity is not None]
            if not caps:
                continue
            maxc = max(caps)
            minc = min(caps)
            for cpu in cluster.cpus:
                if cpu.capacity is None:
                    continue
                if maxc != minc and cpu.capacity == maxc:
                    cpu.core_type = 'P'
                elif maxc != minc and cpu.capacity == minc:
                    cpu.core_type = 'E'
                else:
                    cpu.core_type = 'M'  # medium/mixed type
    
    def _infer_cache_info(self):
        """Do not infer cache parameters heuristically.

        Size/NumOfSets/Associativity/LineSize/Attributes must be used only if explicitly provided in the DT.
        The kernel reads these fields from registers, so avoid hardcoding defaults here.
        """
        return
    
    def get_summary(self) -> Dict:
        """Return a summary of topology."""
        return {
            'num_cpus': len(self.cpus),
            'num_clusters': len(self.clusters),
            'has_l3': self.l3_cache is not None,
            'clusters': [
                {
                    'name': cluster.name,
                    'num_cores': len(cluster.cpus),
                    'capacities': [cpu.capacity for cpu in cluster.cpus if cpu.capacity]
                }
                for cluster in self.clusters
            ]
        }

    def print_info(self):
        """Print topology summary and relationship table (used by --info)."""
        print(f"\n=== Topology Info: CPUs={len(self.cpus)} Clusters={len(self.clusters)} L3={'Yes' if self.l3_cache else 'No'} ===")

    def print_relationship_table(self):
        """Print vertical relationship table with merged cells for repeated values."""
        if not self.cpus:
            print("No CPU info to display relationship table")
            return

        # Column widths
        cpu_col_w = 10
        l2_col_w = 12
        cluster_col_w = 15
        l3_col_w = 12
        
        # Helper to center cell content
        def center_cell(content, width):
            content = str(content)
            if len(content) >= width:
                return content[:width]
            pad = width - len(content)
            left = pad // 2
            right = pad - left
            return ' ' * left + content + ' ' * right
        
        # Helper to left-align cell content
        def pad_cell(content, width):
            content = str(content)
            if len(content) >= width:
                return content[:width]
            return content + ' ' * (width - len(content))
        
        has_l3 = self.l3_cache is not None
        
        # Analyze L2 cache groupings for merged cells (by phandle)
        l2_phandles = [(cpu.l2.phandle if cpu.l2 and cpu.l2.phandle else None) for cpu in self.cpus]
        l2_groups = []  # [(start_idx, end_idx, phandle)]
        i = 0
        while i < len(self.cpus):
            phandle = l2_phandles[i]
            j = i + 1
            while j < len(self.cpus) and l2_phandles[j] == phandle:
                j += 1
            l2_groups.append((i, j - 1, phandle))
            i = j
        
        # Analyze cluster groupings for merged cells
        cpu_clusters = [(cpu.cluster.capitalize() if cpu.cluster else '-') for cpu in self.cpus]
        cluster_groups = []  # [(start_idx, end_idx, cluster_name)]
        i = 0
        while i < len(self.cpus):
            cluster = cpu_clusters[i]
            j = i + 1
            while j < len(self.cpus) and cpu_clusters[j] == cluster:
                j += 1
            cluster_groups.append((i, j - 1, cluster))
            i = j
        
        # Header
        print('\nRelationship Table:')
        header_sep = '+' + '-' * cpu_col_w + '+' + '-' * l2_col_w + '+' + '-' * cluster_col_w
        if has_l3:
            header_sep += '+' + '-' * l3_col_w + '+'
        else:
            header_sep += '+'
        
        print(header_sep)
        header_line = '| ' + pad_cell('CPU', cpu_col_w - 2) + ' | ' + pad_cell('L2', l2_col_w - 2) + ' | ' + pad_cell('Cluster', cluster_col_w - 2)
        if has_l3:
            header_line += ' | ' + pad_cell('L3', l3_col_w - 2) + ' |'
        else:
            header_line += ' |'
        print(header_line)
        print(header_sep)
        
        # Data rows with merged cells
        for idx, cpu in enumerate(self.cpus):
            cpu_name = f"CPU{idx}"
            
            # Determine L2 display (show in middle row of merged cell)
            l2_display = ''
            current_l2_group = None
            for start, end, phandle in l2_groups:
                if start <= idx <= end:
                    current_l2_group = (start, end, phandle)
                    # Calculate middle row
                    middle_idx = (start + end) // 2
                    if idx == middle_idx:
                        l2_label = 'L2' if phandle else '-'
                        l2_display = center_cell(l2_label, l2_col_w - 2)
                    else:
                        l2_display = ' ' * (l2_col_w - 2)
                    break
            
            # Determine cluster display (show in middle row of merged cell)
            cluster_display = ''
            current_cluster_group = None
            for start, end, cluster_name in cluster_groups:
                if start <= idx <= end:
                    current_cluster_group = (start, end, cluster_name)
                    # Calculate middle row
                    middle_idx = (start + end) // 2
                    if idx == middle_idx:
                        cluster_display = center_cell(cluster_name, cluster_col_w - 2)
                    else:
                        cluster_display = ' ' * (cluster_col_w - 2)
                    break
            
            # L3 display: show in middle row of all CPUs
            l3_display = ''
            if has_l3:
                middle_idx = len(self.cpus) // 2
                if idx == middle_idx:
                    l3_display = center_cell('L3', l3_col_w - 2)
                else:
                    l3_display = ' ' * (l3_col_w - 2)
            
            # Build row
            row = '| ' + pad_cell(cpu_name, cpu_col_w - 2) + ' | '
            row += l2_display + ' | '
            row += cluster_display
            
            if has_l3:
                row += ' | ' + l3_display + ' |'
            else:
                row += ' |'
            
            print(row)
            
            # Print separator after each CPU row (except the last one)
            if idx < len(self.cpus) - 1:
                # Determine if next row is in a different L2 group
                next_l2_changes = False
                if current_l2_group:
                    _, end, _ = current_l2_group
                    if idx == end:
                        next_l2_changes = True
                
                # Determine if next row is in a different cluster
                next_cluster_changes = False
                if current_cluster_group:
                    _, end, _ = current_cluster_group
                    if idx == end:
                        next_cluster_changes = True
                
                # Build separator line
                sep_line = '+' + '-' * cpu_col_w + '+'
                
                # L2 column: add separator only if L2 group changes
                if next_l2_changes:
                    sep_line += '-' * l2_col_w + '+'
                else:
                    sep_line += ' ' * l2_col_w + '+'
                
                # Cluster column: add separator only if cluster changes
                if next_cluster_changes:
                    sep_line += '-' * cluster_col_w + '+'
                else:
                    sep_line += ' ' * cluster_col_w + '+'
                
                # L3 column: no separator (merged for all rows)
                if has_l3:
                    sep_line += ' ' * l3_col_w + '+'
                
                print(sep_line)
        
        print(header_sep)
        print()

class HeaderGenerator:
    """Header generator"""
    
    def __init__(self, parser: DTBParser, platform: str):
        self.parser = parser
        self.platform = platform
    
    def generate(self) -> str:
        """Generate header file content"""
        lines = []
        
        # header comment
        lines.append(f"#ifndef PPTT_{self.platform.upper()}_H")
        lines.append(f"#define PPTT_{self.platform.upper()}_H")
        lines.append("")
        lines.append("#include <common.h>")
        lines.append("")
        lines.append("// ============================================")
        lines.append(f"// Platform configuration - {self.platform.upper()}")
        lines.append(f"// Auto-generated from device tree")
        lines.append("// ============================================")
        lines.append("")
        
        # OEM info
        lines.append("// OEM information")
        lines.append('#define PPTT_OEM_ID                     "QCOM  "')
        lines.append('#define PPTT_OEM_TABLE_ID               "QCOMEDK2"')
        
        # OEM Revision: extract XXXX from platform name or DTB filename (SMXXXX)
        import re
        sm_match = re.search(r'sm(\d+)', self.platform.lower())
        if not sm_match:
            # Try extracting from DTB path
            sm_match = re.search(r'sm(\d+)', str(self.parser.dtb_path).lower())
        
        if sm_match:
            oem_rev = f"0x{sm_match.group(1)}"
        else:
            # default value
            oem_rev = "0x0001"
        lines.append(f'#define PPTT_OEM_REVISION               {oem_rev}')
        lines.append("")
        
        # CPU topology
        lines.append("// CPU topology configuration")
        lines.append(f"#define NUM_CORES                       {len(self.parser.cpus)}")
        
        if self.parser.clusters:
            lines.append(f"#define NUM_CLUSTERS                    {len(self.parser.clusters)}")
            for cluster in self.parser.clusters:
                cluster_idx = int(cluster.name.replace('cluster', ''))
                lines.append(f"#define {cluster.name.upper()}_CORES            {len(cluster.cpus)}")
        
        lines.append("")
        
        lines.append("// L1 Data Cache configuration")
        lines.append("// Note: The kernel reads L1/L2 parameters from registers. Values below are used only if explicitly provided in the DT; otherwise 0.")
        if self.parser.cpus and self.parser.cpus[0].l1d and self.parser.cpus[0].l1d.size:
            l1d = self.parser.cpus[0].l1d
            lines.append(f"#define L1D_SIZE                        {l1d.size}")
            lines.append(f"#define L1D_NUM_SETS                    {l1d.sets or 0}")
            lines.append(f"#define L1D_ASSOCIATIVITY               {l1d.ways or 0}")
            lines.append(f"#define L1D_LINE_SIZE                   {l1d.line_size or 0}")
            lines.append(f"#define L1D_ATTRIBUTES                  0")
        else:
            lines.append("#define L1D_SIZE                        0")
            lines.append("#define L1D_NUM_SETS                    0")
            lines.append("#define L1D_ASSOCIATIVITY               0")
            lines.append("#define L1D_LINE_SIZE                   0")
            lines.append("#define L1D_ATTRIBUTES                  0")
        
        # Generate per-cluster L1D macros (pointing to global L1D values)
        if self.parser.clusters:
            for cluster in self.parser.clusters:
                cluster_idx = int(cluster.name.replace('cluster', ''))
                lines.append(f"#define CLUSTER{cluster_idx}_L1D_SIZE              L1D_SIZE")
                lines.append(f"#define CLUSTER{cluster_idx}_L1D_NUM_SETS          L1D_NUM_SETS")
                lines.append(f"#define CLUSTER{cluster_idx}_L1D_ASSOCIATIVITY     L1D_ASSOCIATIVITY")
                lines.append(f"#define CLUSTER{cluster_idx}_L1D_LINE_SIZE         L1D_LINE_SIZE")
                lines.append(f"#define CLUSTER{cluster_idx}_L1D_ATTRIBUTES        L1D_ATTRIBUTES")
        lines.append("")

        # L1 Instruction Cache configuration
        lines.append("// L1 Instruction Cache configuration")
        if self.parser.cpus and self.parser.cpus[0].l1i and self.parser.cpus[0].l1i.size:
            l1i = self.parser.cpus[0].l1i
            lines.append(f"#define L1I_SIZE                        {l1i.size}")
            lines.append(f"#define L1I_NUM_SETS                    {l1i.sets or 0}")
            lines.append(f"#define L1I_ASSOCIATIVITY               {l1i.ways or 0}")
            lines.append(f"#define L1I_LINE_SIZE                   {l1i.line_size or 0}")
            lines.append(f"#define L1I_ATTRIBUTES                  0")
        else:
            lines.append("#define L1I_SIZE                        0")
            lines.append("#define L1I_NUM_SETS                    0")
            lines.append("#define L1I_ASSOCIATIVITY               0")
            lines.append("#define L1I_LINE_SIZE                   0")
            lines.append("#define L1I_ATTRIBUTES                  0")
        
        # Generate per-cluster L1I macros (pointing to global L1I values)
        if self.parser.clusters:
            for cluster in self.parser.clusters:
                cluster_idx = int(cluster.name.replace('cluster', ''))
                lines.append(f"#define CLUSTER{cluster_idx}_L1I_SIZE              L1I_SIZE")
                lines.append(f"#define CLUSTER{cluster_idx}_L1I_NUM_SETS          L1I_NUM_SETS")
                lines.append(f"#define CLUSTER{cluster_idx}_L1I_ASSOCIATIVITY     L1I_ASSOCIATIVITY")
                lines.append(f"#define CLUSTER{cluster_idx}_L1I_LINE_SIZE         L1I_LINE_SIZE")
                lines.append(f"#define CLUSTER{cluster_idx}_L1I_ATTRIBUTES        L1I_ATTRIBUTES")
        lines.append("")

        # L2 Cache configuration
        lines.append("// L2 Cache configuration")
        
        # First, check if ALL CPUs have unique L2 (per-core L2)
        all_l2_phandles = set()
        cpu_count_with_l2 = 0
        for cpu in self.parser.cpus:
            if cpu.l2 and cpu.l2.phandle:
                all_l2_phandles.add(cpu.l2.phandle)
                cpu_count_with_l2 += 1
        
        is_per_core_l2 = (cpu_count_with_l2 > 1 and len(all_l2_phandles) == cpu_count_with_l2)
        
        if is_per_core_l2:
            # Per-core L2: each CPU has its own L2
            lines.append("// Per-core L2 detected - each CPU has private L2")
            for idx, cpu in enumerate(self.parser.cpus):
                if cpu.l2:
                    lines.append(f"#define CORE{idx}_L2_SIZE              {cpu.l2.size or 0}")
                    lines.append(f"#define CORE{idx}_L2_NUM_SETS          {cpu.l2.sets or 0}")
                    lines.append(f"#define CORE{idx}_L2_ASSOCIATIVITY     {cpu.l2.ways or 0}")
                    lines.append(f"#define CORE{idx}_L2_LINE_SIZE         {cpu.l2.line_size or 0}")
                    lines.append(f"#define CORE{idx}_L2_ATTRIBUTES        0")
                    lines.append("")
            # Define default L2 macros as 0
            lines.append("// Default L2 macros (unused when per-core macros present)")
            lines.append("#define L2_SIZE                         0")
            lines.append("#define L2_NUM_SETS                     0")
            lines.append("#define L2_ASSOCIATIVITY                0")
            lines.append("#define L2_LINE_SIZE                    0")
            lines.append("#define L2_ATTRIBUTES                   0")
        else:
            # Check for per-cluster shared L2 (multiple CPUs in cluster share same L2 phandle)
            cluster_l2_map = {}
            for cluster in self.parser.clusters:
                phs = set()
                cpu_count = 0
                for cpu in cluster.cpus:
                    cpu_count += 1
                    if cpu.l2 and cpu.l2.phandle:
                        phs.add(cpu.l2.phandle)
                # Only treat as per-cluster L2 if multiple CPUs share the same L2
                if len(phs) == 1 and cpu_count > 1:
                    ph = next(iter(phs))
                    ci = self.parser.cache_map.get(ph)
                    if ci:
                        cluster_idx = int(cluster.name.replace('cluster', ''))
                        cluster_l2_map[cluster_idx] = ci

            if cluster_l2_map:
                lines.append("// Per-cluster L2 detected - emitting CLUSTERn_L2_* macros")
                for idx in sorted(cluster_l2_map.keys()):
                    ci = cluster_l2_map[idx]
                    lines.append(f"#define CLUSTER{idx}_L2_SIZE              {ci.size or 0}")
                    lines.append(f"#define CLUSTER{idx}_L2_NUM_SETS          {ci.sets or 0}")
                    lines.append(f"#define CLUSTER{idx}_L2_ASSOCIATIVITY     {ci.ways or 0}")
                    lines.append(f"#define CLUSTER{idx}_L2_LINE_SIZE         {ci.line_size or 0}")
                    lines.append(f"#define CLUSTER{idx}_L2_ATTRIBUTES        0")
                    lines.append("")
                # Define default L2 macros as 0 to avoid upstream misuse
                lines.append("// Default L2 macros (unused when per-cluster macros present)")
                lines.append("#define L2_SIZE                         0")
                lines.append("#define L2_NUM_SETS                     0")
                lines.append("#define L2_ASSOCIATIVITY                0")
                lines.append("#define L2_LINE_SIZE                    0")
                lines.append("#define L2_ATTRIBUTES                   0")
            else:
                # No per-cluster shared L2; try finding unified L2
                l2cpu = next((c for c in self.parser.cpus if c.l2 and c.l2.size), None)
                if l2cpu and l2cpu.l2 and l2cpu.l2.size:
                    l2 = l2cpu.l2
                    if len(self.parser.clusters) > 1:
                        lines.append("// Note: each cluster may have independent L2 cache")
                    lines.append(f"#define L2_SIZE                         {l2.size}")
                    lines.append(f"#define L2_NUM_SETS                     {l2.sets or 0}")
                    lines.append(f"#define L2_ASSOCIATIVITY                {l2.ways or 0}")
                    lines.append(f"#define L2_LINE_SIZE                    {l2.line_size or 0}")
                    lines.append("#define L2_ATTRIBUTES                   0")
                else:
                    lines.append("#define L2_SIZE                         0")
                    lines.append("#define L2_NUM_SETS                     0")
                    lines.append("#define L2_ASSOCIATIVITY                0")
                    lines.append("#define L2_LINE_SIZE                    0")
                    lines.append("#define L2_ATTRIBUTES                   0")
        lines.append("")
        
        # L3 Cache configuration (if present)
        if self.parser.l3_cache:
            lines.append("// L3 Cache configuration (shared)")
            lines.append("// Note: L3 cache is shared across all clusters")
            lines.append("#define HAS_L3_CACHE                    1")
            lines.append("#define L3_SIZE                         0")
            lines.append("#define L3_NUM_SETS                     0")
            lines.append("#define L3_ASSOCIATIVITY                0")
            lines.append("#define L3_LINE_SIZE                    0")
            lines.append("#define L3_ATTRIBUTES                   0")
            lines.append("")
        
        # Cache line size
        lines.append("// General cache configuration")
        lines.append("#define CACHE_LINE_SIZE                 64")
        lines.append("")
        
        lines.append(f"#endif // PPTT_{self.platform.upper()}_H")
        lines.append("")
        
        return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description='Generate PPTT header file from DTB file',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Generate header file from DTB file
  %(prog)s sm8550.dtb -o include/sm8550/pptt.h
  
  # Auto-generate header file to include/<platform-name>/pptt.h
  %(prog)s sm8550.dtb
  
  # Display device tree topology information
  %(prog)s sm8550.dtb --info
        """
    )
    
    parser.add_argument('input', type=Path, help='DTB file path')
    parser.add_argument('-o', '--output', type=Path, help='Output header file path')
    parser.add_argument('-p', '--platform', help='Platform name (default: extracted from filename)')
    parser.add_argument('--info', action='store_true', help='Only display topology information and print CPU/Cluster/Cache relationship table (without file generation)')
    
    args = parser.parse_args()
    
    # Check input file
    if not args.input.exists():
        print(f"Error: File not found: {args.input}")
        return 1
    
    if args.input.suffix != '.dtb':
        print(f"Error: Input file must be a .dtb file")
        return 1
    
    # Extract platform name
    if args.platform:
        platform = args.platform
    else:
        platform = args.input.stem
    
    # Parse DTB
    print(f"Parsing DTB: {args.input}")
    try:
        dtb_parser = DTBParser(args.input)
        dtb_parser.parse()
    except Exception as e:
        print(f"Error: Failed to parse DTB: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    # Print topology summary
    summary = dtb_parser.get_summary()
    print(f"\nDTB Topology ({platform}):")
    print(f"  Total CPUs: {summary['num_cpus']}")
    print(f"  Clusters: {summary['num_clusters']}")
    print(f"  L3 Cache: {'Yes' if summary['has_l3'] else 'No'}")
    
    if summary['clusters']:
        print(f"\n  Cluster details:")
        for cluster in summary['clusters']:
            print(f"    {cluster['name']}: {cluster['num_cores']} cores")
            if cluster['capacities']:
                caps = cluster['capacities']
                if all(c == caps[0] for c in caps):
                    print(f"      Capacity: {caps[0]}")
                else:
                    print(f"      Capacity: {min(caps)} ~ {max(caps)}")
    
    # Info-only mode
    if args.info:
        dtb_parser.print_info()
        dtb_parser.print_relationship_table()
        return 0
    
    # Generate header file
    generator = HeaderGenerator(dtb_parser, platform)
    header_content = generator.generate()
    
    # Determine output path
    if args.output:
        output_file = args.output
    else:
        output_file = Path(f"include/{platform}/pptt.h")
    
    # 创建目录
    output_file.parent.mkdir(parents=True, exist_ok=True)
    
    # Write file
    output_file.write_text(header_content)
    print(f"\n✅ Generated header file: {output_file}")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
