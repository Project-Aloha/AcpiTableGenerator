#!/usr/bin/env python3
"""
Generate MCFG table parameters from a device tree blob (DTB).

MCFG (Memory Mapped Configuration Table) 是 ACPI 表，用于配置 PCI Express
配置空间的内存映射地址。

Usage:
  python tools/dtb_to_mcfg.py sm8845.dtb -o include/vendor/qcom/sm8845/mcfg.h

This script parses the DTB and extracts:
  - Number of PCI domains (segments)
  - Base addresses for each PCI Express configuration space
  - Bus number ranges for each domain
  - Generate the corresponding C header file

Requires: pyfdt (pip install pyfdt)
"""

from __future__ import annotations
import argparse
import sys
import re
from pathlib import Path
from typing import List, Tuple, Optional, Dict

try:
    from pyfdt.pyfdt import FdtBlobParse
except Exception as e:
    sys.exit("Missing dependency: pyfdt (pip install pyfdt). Error: %s" % e)


def to_u64(high: int, low: int) -> int:
    """Convert 32-bit high and low to 64-bit value."""
    return (high << 32) | (low & 0xFFFFFFFF)


def parse_reg_property(node) -> List[Tuple[int, int]]:
    """
    Parse 'reg' property from a device tree node.
    Returns list of (address, size) tuples.
    """
    results = []
    for prop in node.subdata:
        if hasattr(prop, 'name') and prop.name == 'reg' and hasattr(prop, 'words'):
            words = prop.words
            # Process in pairs (address_high, address_low, size_high, size_low)
            # Assuming #address-cells=2 and #size-cells=2 (64-bit)
            for i in range(0, len(words), 4):
                if i + 3 < len(words):
                    addr = to_u64(words[i], words[i + 1])
                    size = to_u64(words[i + 2], words[i + 3])
                    results.append((addr, size))
    return results


def parse_ranges_property(node) -> List[Dict]:
    """
    Parse 'ranges' property to extract PCI address mappings.
    
    For PCI nodes, ranges property format (for #address-cells=3, #size-cells=2):
    child-unit-address parent-unit-address length
    
    Each entry:
    <phys_hi phys_mid phys_lo> <parent_addr_h parent_addr_l> <size_h size_l>
    
    phys_hi contains:
    - bits 31-30: address type (00=config, 01=IO, 10=memory, 11=reserved)
    - bits 29: prefetchable
    - bits 28: aliased
    - bits 27-25: reserved
    - bits 24: relocatable
    - bits 23-16: reserved
    - bits 15-0: bus/device/function
    """
    results = []
    for prop in node.subdata:
        if hasattr(prop, 'name') and prop.name == 'ranges' and hasattr(prop, 'words'):
            words = prop.words
            # Process in groups of 7 cells
            # child-address (3 cells), parent-address (2 cells), size (2 cells)
            i = 0
            while i + 6 < len(words):
                phys_hi = words[i]      # address type and flags
                phys_mid = words[i + 1] # bus number
                phys_lo = words[i + 2]  # device/function
                parent_addr_h = words[i + 3]
                parent_addr_l = words[i + 4]
                size_h = words[i + 5]
                size_l = words[i + 6]
                
                parent_addr = to_u64(parent_addr_h, parent_addr_l)
                size = to_u64(size_h, size_l)
                
                # Extract type from phys_hi (bits 31-30)
                addr_type = (phys_hi >> 30) & 0x03
                if addr_type == 0:
                    type_name = "CONFIG"
                elif addr_type == 1:
                    type_name = "IO"
                elif addr_type == 2:
                    type_name = "MEMORY"
                else:
                    type_name = "RESERVED"
                
                # Extract prefetchable flag (bit 29)
                prefetchable = (phys_hi >> 29) & 0x01
                
                results.append({
                    'type': type_name,
                    'parent_addr': parent_addr,
                    'parent_addr_h': parent_addr_h,
                    'parent_addr_l': parent_addr_l,
                    'size': size,
                    'phys_hi': phys_hi,
                    'phys_mid': phys_mid,
                    'phys_lo': phys_lo,
                    'prefetchable': prefetchable,
                })
                
                i += 7
    
    return results


def find_pcie_nodes(rootnode) -> List[Tuple[object, str, int]]:
    """Find all PCIe controller nodes and return (node, path, domain_number).

    The returned "path" is the full device-tree path (e.g., '/soc/qcom,pcie@1c00000')
    to avoid ambiguity between vendor-prefixed names.
    """
    results = []
    
    def node_full_path(node):
        parts = []
        p = node
        while p is not None and hasattr(p, 'name') and p.name:
            parts.append(p.name)
            p = getattr(p, 'parent', None)
        if parts:
            # filter out any empty parts and ensure single leading slash
            parts = [p for p in reversed(parts) if p]
            # Ensure single leading slash and no duplicate slashes
            path = '/' + '/'.join(parts)
            return re.sub('/+', '/', path)
        return node.name

    def walk(node, depth=0):
        if hasattr(node, 'name') and node.name and 'pcie' in node.name.lower():
            # Extract domain number from node name (e.g., pcie@1c00000 -> 0, pcie1@... -> 1)
            match = re.search(r'pcie(\d+)?', node.name)
            domain_num = int(match.group(1)) if match and match.group(1) else 0
            results.append((node, node_full_path(node), domain_num))
        
        if hasattr(node, 'subdata'):
            for child in node.subdata:
                if hasattr(child, 'subdata'):
                    walk(child, depth + 1)
    
    walk(rootnode)
    return results


def get_bus_range(node) -> Tuple[int, int]:
    """Extract bus range from 'bus-range' property."""
    default_start, default_end = 0, 0xFF
    
    if not hasattr(node, 'subdata'):
        return default_start, default_end
    
    for prop in node.subdata:
        if hasattr(prop, 'name') and prop.name == 'bus-range' and hasattr(prop, 'words'):
            words = prop.words
            if len(words) >= 2:
                return words[0], words[1]
    
    return default_start, default_end


def get_segment_number(node) -> int:
    """Extract PCI segment number from 'linux,pci-domain' property."""
    if not hasattr(node, 'subdata'):
        return 0
    
    for prop in node.subdata:
        if hasattr(prop, 'name') and prop.name == 'linux,pci-domain' and hasattr(prop, 'words'):
            if prop.words:
                return prop.words[0]
    
    return 0


def extract_mcfg_params(dtb_path: str) -> List[Dict]:
    """
    Extract MCFG parameters from DTB.
    
    Returns list of dicts with keys:
      - segment: PCI segment number
      - base_addr: Configuration space base address
      - bus_start: Starting bus number
      - bus_end: Ending bus number
      - domain_num: Domain number from device tree
    """
    with open(dtb_path, 'rb') as f:
        fdt = FdtBlobParse(f).to_fdt()
    
    root = fdt.get_rootnode()
    pcie_nodes = find_pcie_nodes(root)
    
    results = []
    
    for node, node_name, domain_num in pcie_nodes:
        # Get segment number
        segment = get_segment_number(node)
        
        # Get bus range
        bus_start, bus_end = get_bus_range(node)
        
        # Parse ranges property to find config space address
        ranges = parse_ranges_property(node)
        
        # Look for CONFIG type range (type 0)
        # According to PCI spec, CONFIG space is where Enhanced Configuration Space is located
        config_found = False
        for rng in ranges:
            if rng['type'] == 'CONFIG':
                # Parent address in DTB can be encoded oddly (sometimes placed in the
                # high cell with low cell zero). We normalize to a 32/64-bit base and
                # remove any bus-specific offset encoded in the child phys fields so
                # we get the base for bus 0.
                p = rng['parent_addr']

                # Prefer a 32-bit value placed in the high cell when low cell is zero
                if (p & 0xFFFFFFFF) == 0 and (p >> 32) != 0:
                    base_candidate = (p >> 32) & 0xFFFFFFFF
                else:
                    base_candidate = p

                # Child phys_lo can encode a bus byte in bits 23:16; remove that
                # offset to compute the base address for bus 0.
                phys_lo = rng.get('phys_lo', 0)
                bus_byte = (phys_lo >> 16) & 0xFF
                bus_offset = (bus_byte << 16)

                base_for_bus0 = base_candidate - bus_offset

                # If base_for_bus0 fits in 32-bits, keep it as 32-bit value; otherwise
                # leave as 64-bit integer.
                if base_for_bus0 <= 0xFFFFFFFF:
                    base_addr = int(base_for_bus0 & 0xFFFFFFFF)
                else:
                    base_addr = int(base_for_bus0)

                results.append({
                    'segment': segment,
                    'base_addr': base_addr,
                    'bus_start': bus_start,
                    'bus_end': bus_end,
                    'domain_num': domain_num,
                    'node_name': node_name,
                    'node_basename': getattr(node, 'name', node_name),
                })
                config_found = True
                break
        
        # If no CONFIG type found, try the first MEMORY range with large address
        # which is likely the Extended Configuration Space
        if not config_found:
            for rng in ranges:
                if rng['type'] == 'MEMORY' and rng['parent_addr'] > 0x100000000:
                    # apply same normalization logic as above
                    p = rng['parent_addr']
                    if (p & 0xFFFFFFFF) == 0 and (p >> 32) != 0:
                        base_candidate = (p >> 32) & 0xFFFFFFFF
                    else:
                        base_candidate = p

                    phys_lo = rng.get('phys_lo', 0)
                    bus_byte = (phys_lo >> 16) & 0xFF
                    bus_offset = (bus_byte << 16)

                    base_for_bus0 = base_candidate - bus_offset

                    if base_for_bus0 <= 0xFFFFFFFF:
                        base_addr = int(base_for_bus0 & 0xFFFFFFFF)
                    else:
                        base_addr = int(base_for_bus0)

                    results.append({
                        'segment': segment,
                        'base_addr': base_addr,
                        'bus_start': bus_start,
                        'bus_end': bus_end,
                        'domain_num': domain_num,
                        'node_name': node_name,
                    })
                    break
    
    # Sort by segment number
    results.sort(key=lambda x: x['segment'])
    
    return results


def generate_c_header(mcfg_params: List[Dict], vendor: str = "qcom") -> str:
    """Generate C header file content for MCFG table."""
    if not mcfg_params:
        return "// No MCFG parameters found in device tree\n"
    
    lines = [
        '#pragma once',
        '#include "table_header.h"',
        '#include <common/mcfg.h>',
        '',
    ]
    
    # Define PCI segment count and addresses
    lines.append(f"#define PCI_EC_SPACE_COUNT {len(mcfg_params)}")
    
    for i, param in enumerate(mcfg_params):
        node_comment = param.get('node_basename', param.get('node_name', ''))
        lines.append(f"#define PCI_EC_{i}_BASE_ADDRESS 0x{param['base_addr']:X}ULL  /* {node_comment} */")
    
    lines.extend(['', ''])
    lines.append(f"MCFG_DEFINE_TABLE(PCI_EC_SPACE_COUNT);")
    lines.append("MCFG_DEFINE_WITH_MAGIC;")
    lines.extend(['', ''])
    
    lines.append("MCFG_START {")
    lines.append("    MCFG_DECLARE_HEADER,")
    lines.append("    MCFG_DECLARE_HEADER_EXTRA_DATA,")
    
    for i, param in enumerate(mcfg_params):
        start_bus = f"0x{param['bus_start']:02X}"
        end_bus = f"0x{param['bus_end']:02X}"
        lines.append(
            f"    MCFG_DECLARE_EC_SPACE_STRUCTURE({i}, {param['segment']}, PCI_EC_{i}_BASE_ADDRESS, "
            f"{start_bus}, {end_bus}), // PCI Segment {param['segment']}"
        )
    
    lines.append("} MCFG_END")
    
    return '\n'.join(lines) + '\n'


def main():
    parser = argparse.ArgumentParser(
        description="Generate MCFG table parameters from DTB"
    )
    parser.add_argument('dtb_file', help='Device tree blob file (.dtb)')
    parser.add_argument('-o', '--output', help='Output header file path')
    parser.add_argument('--vendor', default='qcom', help='Vendor name (default: qcom)')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    
    args = parser.parse_args()
    
    dtb_path = Path(args.dtb_file)
    if not dtb_path.exists():
        sys.exit(f"Error: DTB file not found: {dtb_path}")
    
    # Extract parameters
    mcfg_params = extract_mcfg_params(str(dtb_path))
    
    if args.verbose or not args.output:
        print("MCFG Parameters extracted from DTB:")
        print("=" * 60)
        for param in mcfg_params:
            print(f"  Segment {param['segment']}:")
            print(f"    Base Address:  0x{param['base_addr']:016X}")
            print(f"    Bus Range:     {param['bus_start']:#04x} - {param['bus_end']:#04x}")
            print(f"    Node:          {param['node_name']}")
            print()
    
    # Generate C header
    header_content = generate_c_header(mcfg_params, args.vendor)
    
    if args.output:
        output_path = Path(args.output)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(header_content)
        print(f"Generated: {output_path}")
    else:
        print(header_content)


if __name__ == '__main__':
    main()
