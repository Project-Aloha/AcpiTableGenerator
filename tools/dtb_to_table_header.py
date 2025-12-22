#!/usr/bin/env python3
"""
Generate a platform-specific `table_header.h` from a device tree blob (DTB).

Heuristics implemented:
 - NUM_CORES: number of cpu@ nodes under /cpus
 - NUM_CLUSTERS: detected from /cpus/cpu-map (preferred) or from "qcom,gic-class*" lists under gic-interrupt-router
 - NUM_CLUSTER_<i>_CORES: counts per cluster (if >2 clusters we'll still emit macros for each)
 - ACPI_OEM_REVISION: derived from DTB filename if it contains 'sm<digits>' (e.g., sm8850 -> 0x8850) or via --oem-rev
 - L2_CACHES_COUNT: number of distinct next-level-cache phandles referenced by CPUs
 - L1_CACHES_COUNT: guessed 2 by default (can be overridden)
 - L3_CACHES_COUNT: detected by searching for nodes named *l3* or cache nodes with 'level' == 3

Output path default: include/<dtb_stem>/table_header.h

Usage:
  python tools/dtb_to_table_header.py sm7325.dtb
  python tools/dtb_to_table_header.py sm7325.dtb -o include/sm7325/table_header.h --oem-rev 0x7325

Requires: pyfdt (pip install pyfdt)
"""

from __future__ import annotations
import argparse
import re
import sys
from pathlib import Path
from typing import List, Dict

try:
    from pyfdt.pyfdt import FdtBlobParse
except Exception as e:
    sys.exit("Missing dependency: pyfdt (pip install pyfdt). Error: %s" % e)


def find_cpus(rootnode):
    # return list of cpu nodes (node objects)
    def walk(node):
        for sd in node.subdata:
            if hasattr(sd, 'subdata'):
                if sd.name == 'cpus':
                    return [n for n in sd.subdata if hasattr(n, 'name') and n.name.startswith('cpu@')]
                r = walk(sd)
                if r:
                    return r
        return []
    return walk(rootnode)


def find_cpu_map_clusters(rootnode):
    # Look for /cpus/cpu-map -> cluster@ entries
    def walk(node):
        for sd in node.subdata:
            if hasattr(sd, 'subdata') and sd.name == 'cpus':
                for child in sd.subdata:
                    if hasattr(child, 'subdata'):
                        # cpu-map is typically a child under /cpus or named 'cpu-map'
                        for cc in child.subdata:
                            if hasattr(cc, 'name') and cc.name == 'cpu-map':
                                # parse cluster entries
                                clusters = []
                                for cluster in cc.subdata:
                                    if hasattr(cluster, 'name') and cluster.name.startswith('cluster@'):
                                        core_count = sum(1 for c in cluster.subdata if hasattr(c, 'name') and c.name.startswith('cpu@'))
                                        clusters.append(core_count)
                                if clusters:
                                    return clusters
        return None
    return walk(rootnode)


def find_clusters_from_qcom_gic(rootnode):
    # Prefer explicit 'gic-interrupt-router' node; gather qcom,gic-class* lists
    stack = [rootnode]
    clusters = []
    while stack:
        n = stack.pop()
        for sd in n.subdata:
            if not hasattr(sd, 'subdata'):
                continue
            # check node name first for explicit router
            if sd.name and 'gic-interrupt-router' in sd.name:
                for p in sd.subdata:
                    if hasattr(p, 'name') and p.name.startswith('qcom,gic-class') and hasattr(p, 'words'):
                        clusters.append(len(p.words))
                if clusters:
                    return clusters
            stack.append(sd)
    # fallback: scan entire tree for qcom,gic-class* props
    stack = [rootnode]
    while stack:
        n = stack.pop()
        for sd in n.subdata:
            if not hasattr(sd, 'subdata'):
                continue
            for p in sd.subdata:
                if hasattr(p, 'name') and p.name.startswith('qcom,gic-class') and hasattr(p, 'words'):
                    clusters.append(len(p.words))
            stack.append(sd)
    return clusters if clusters else None


def count_l2_caches(cpus):
    """Return 2 if every CPU references the same next-level-cache phandle, otherwise return None to indicate manual inspection needed."""
    ph_list = []
    for cpu in cpus:
        val = None
        for p in cpu.subdata:
            if hasattr(p, 'name') and p.name == 'next-level-cache' and hasattr(p, 'words') and p.words:
                val = p.words[0]
                break
        ph_list.append(val)
    if not ph_list:
        return None
    first = ph_list[0]
    if first is None:
        return None
    if all(x == first for x in ph_list):
        # Per requirement: if equal across CPUs, set to 2
        return 2
    return None


def detect_l3_caches(rootnode):
    # look for nodes with 'l3' in name or properties stating level 3
    stack = [rootnode]
    for n in stack:
        for sd in n.subdata:
            if hasattr(sd, 'subdata'):
                stack.append(sd)
                if sd.name and 'l3' in sd.name.lower():
                    return 1
                # check properties for cache level
                for p in sd.subdata:
                    if hasattr(p, 'name') and p.name in ('cache-level', 'cache-size', 'cache-levels'):
                        # best-effort: treat as L3 present
                        return 1
    return 0


def infer_oem_rev(dtb_name: str) -> int:
    # if dtb name contains 'sm' followed by digits, use that
    m = re.search(r'sm([0-9a-fA-F]+)', dtb_name, re.IGNORECASE)
    if m:
        # interpret the digits as hex so 'sm8850' -> 0x8850
        val = int(m.group(1), 16)
        return val
    return 0


HEADER_TMPL = '''#pragma once
#include <acpi_vendor.h>

#define ACPI_OEM_REVISION 0x{oem_rev:04x}

/* Platform specific configuration */
#define NUM_CORES {num_cores}
#define NUM_CLUSTERS {num_clusters}
#define NUM_SYSTEM 1
{cluster_macros}

#define L1_CACHES_COUNT {l1}
#define L2_CACHES_COUNT {l2}
#define L3_CACHES_COUNT {l3}
'''


def generate_header(dtb_path: Path, out_path: Path, oem_rev: int = 0, default_l1: int = 2):
    with dtb_path.open('rb') as f:
        fdt = FdtBlobParse(f).to_fdt()

    root = fdt.rootnode
    cpus = find_cpus(root)
    num_cores = len(cpus)

    # clusters detection
    clusters = find_cpu_map_clusters(root)
    if not clusters:
        clusters = find_clusters_from_qcom_gic(root)
    if not clusters:
        # fallback: single cluster
        clusters = [num_cores] if num_cores > 0 else [0]

    num_clusters = len(clusters)

    # normalize cluster ordering: put largest cluster as cluster 0 for compatibility
    clusters_sorted = sorted(clusters, reverse=True)

    # build cluster macros
    cluster_macros_lines = []
    for i, ccount in enumerate(clusters_sorted):
        # only emit macros for first two clusters to match existing code style, but also emit extras if more clusters
        cluster_macros_lines.append(f"#define NUM_CLUSTER_{i}_CORES {ccount}")
    cluster_macros = '\n'.join(cluster_macros_lines)

    # caches
    l2_count = count_l2_caches(cpus)
    l3_count = detect_l3_caches(root)
    l1_count = default_l1

    # Format L2 macro: if l2_count is None -> prompt for manual fix
    if l2_count is None:
        l2_value = '/*Fix Me*/'
    else:
        l2_value = str(l2_count)

    # OEM revision
    if oem_rev == 0:
        oem_rev = infer_oem_rev(dtb_path.name)

    header = HEADER_TMPL.format(
        oem_rev=oem_rev,
        num_cores=num_cores,
        num_clusters=num_clusters,
        cluster_macros=cluster_macros,
        l1=l1_count,
        l2=l2_value,
        l3=l3_count,
    )

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(header)
    print(f"Wrote table header to: {out_path}")


def main():
    p = argparse.ArgumentParser(description='Generate table_header.h from DTB')
    p.add_argument('dtb', type=Path, help='DTB file')
    p.add_argument('--vendor', required=True, help='Vendor directory under include (e.g. qcom, mtk)')
    p.add_argument('-o', '--output', type=Path, default=None, help='Output path (default include/<vendor>/<dtb_stem>/table_header.h)')
    p.add_argument('--oem-rev', type=lambda x: int(x, 0), default=0, help='Override OEM revision (e.g. 0x8850)')
    p.add_argument('--l1', type=int, default=2, help='Default L1 caches count')
    args = p.parse_args()

    # Compute default output if not provided
    if args.output is None:
        dtb_stem = args.dtb.name.split('.')[0]
        args.output = Path('include') / 'vendor' / args.vendor / dtb_stem / 'table_header.h'

    if not args.dtb.exists():
        print(f"DTB not found: {args.dtb}")
        return 2

    generate_header(args.dtb, args.output, args.oem_rev, args.l1)
    return 0


if __name__ == '__main__':
    sys.exit(main())
