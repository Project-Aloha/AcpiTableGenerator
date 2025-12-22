#!/usr/bin/env python3
"""
Generate a MADT-style C header from a device tree blob (DTB).

Usage:
  python tools/dtb_to_madt.py sm7325.dtb -o include/vendor/qcom/sm7325/madt.h --vendor qcom

This script uses pyfdt (pip install pyfdt). It will:
 - parse the DTB and count CPU cores under /cpus
 - find the main GIC interrupt-controller node and extract reg/redistributor stride
 - attempt to locate ITS nodes (if any) and set NUM_ITS and ITS base
 - extract interrupt triplets from the GIC node and convert them to GSI values
 - produce a header with macros (all macros except per-core MPIDR values)

Notes:
 - If pyfdt is not installed, the script will print a helpful error message.
 - The script tries to be conservative: if a value cannot be determined it will emit 0
   or a short comment in the generated header so it's obvious what's missing.
"""

from __future__ import annotations
import argparse
import sys
from pathlib import Path
from typing import List, Tuple, Optional

# Import pyfdt parser
try:
    from pyfdt.pyfdt import FdtBlobParse
except Exception as e:
    sys.exit("Missing dependency: pyfdt (pip install pyfdt). Error: %s" % e)


def to_u64(high: int, low: int) -> int:
    return (high << 32) | (low & 0xFFFFFFFF)


def pair_words(words: List[int]) -> List[Tuple[int, int]]:
    pairs = []
    for i in range(0, len(words), 2):
        hi = words[i]
        lo = words[i + 1] if i + 1 < len(words) else 0
        pairs.append((hi, lo))
    return pairs


def words_to_int(w: List[int]) -> int:
    # If length==1 treat as 32-bit value, if 2 treat as high/low
    if not w:
        return 0
    if len(w) == 1:
        return w[0]
    if len(w) >= 2:
        return to_u64(w[0], w[1])
    return 0


def find_gic_node(rootnode) -> Optional[object]:
    # Traverse nodes to find one with 'interrupt-controller' prop and 'gic' in compatible
    stack = [rootnode]
    while stack:
        n = stack.pop()
        for sd in n.subdata:
            if not hasattr(sd, 'subdata'):
                continue
            stack.append(sd)
            # Collect property names/strings
            names = [p.name for p in sd.subdata if hasattr(p, 'name')]
            strings = []
            for p in sd.subdata:
                if hasattr(p, 'strings') and p.strings:
                    strings += p.strings
            if 'interrupt-controller' in names and any('gic' in s for s in strings if s):
                return sd
    return None


def find_its_nodes(rootnode) -> List[object]:
    hits = []
    stack = [rootnode]
    while stack:
        n = stack.pop()
        for sd in n.subdata:
            if not hasattr(sd, 'subdata'):
                continue
            stack.append(sd)
            for p in sd.subdata:
                if hasattr(p, 'strings') and p.strings:
                    for s in p.strings:
                        if s and ('gic-its' in s or 'gic,its' in s or 'arm,gic-its' in s):
                            hits.append(sd)
    return hits


def find_cpus(rootnode) -> List[object]:
    # find /cpus node and return cpu@ subnodes
    stack = [rootnode]
    cpus_node = None
    while stack and cpus_node is None:
        n = stack.pop()
        for sd in n.subdata:
            if not hasattr(sd, 'subdata'):
                continue
            if sd.name == 'cpus':
                cpus_node = sd
                break
            stack.append(sd)
    if not cpus_node:
        return []
    cpus = [sd for sd in cpus_node.subdata if hasattr(sd, 'name') and sd.name.startswith('cpu@')]
    return cpus


def read_prop_words(node, propname) -> Optional[List[int]]:
    for p in node.subdata:
        if hasattr(p, 'name') and p.name == propname and hasattr(p, 'words'):
            return p.words
    return None


def read_prop_strings(node, propname) -> Optional[List[str]]:
    for p in node.subdata:
        if hasattr(p, 'name') and p.name == propname and hasattr(p, 'strings'):
            return p.strings
    return None


def parse_interrupt_triplets(words: List[int]) -> List[Tuple[int, int, int]]:
    triplets = []
    for i in range(0, len(words), 3):
        a = words[i] if i < len(words) else 0
        b = words[i + 1] if i + 1 < len(words) else 0
        c = words[i + 2] if i + 2 < len(words) else 0
        triplets.append((a, b, c))
    return triplets


def triplet_to_gsi(triplet: Tuple[int, int, int]) -> int:
    typ, num, flags = triplet
    # For GIC interrupt specifier: typ 0=SPI, 1=PPI -- map PPI -> 16 + number
    if typ == 1:
        return 16 + num
    else:
        return num


HEADER_TEMPLATE = """
#pragma once
#include "table_header.h"
#include <common/madt.h>

#define GICD_BASE_ADDRESS {gicd:#010x}ULL
{gic_its_macro}#define GICR_BASE_ADDRESS {gicr:#010x}ULL
#define GICR_STRIDE {gicr_stride:#010x}ULL
#define GIC_VERSION {gic_version_macro} {gic_version_comment}
#define GICC_PERFORMANCE_INTERRUPT_GSI {perf_gsi:#04x}
#define GICC_VGIC_MAINTENANCE_INTERRUPT {vgic_gsi:#04x}

{mpidr_macros}#define NUM_ITS {num_its}

MADT_DEFINE_TABLE(NUM_CORES, NUM_ITS, ACPI_MADT_TABLE_STRUCTURE_NAME);
MADT_DEFINE_WITH_MAGIC;

MADT_START{{
    /* Table Header */
    MADT_DECLARE_HEADER,
    MADT_DECLARE_HEADER_EXTRA_DATA(0, 0),
    /* GICD Structure */
    MADT_DECLARE_GICD_STRUCTURE(GICD_BASE_ADDRESS, 0),

{gic_its_decl}
    /* GICC Structure */
{gicc_decls}
}} MADT_END;
"""


def generate_header(out_path: Path, dtb_path: Path, platform_name: Optional[str] = None):
    with dtb_path.open('rb') as f:
        fdt = FdtBlobParse(f).to_fdt()

    root = fdt.rootnode

    cpus = find_cpus(root)
    num_cores = len(cpus)

    gic = find_gic_node(root)

    # defaults
    gicd = 0
    gicr = 0
    gicr_stride = 0
    num_its = 0
    gicits = 0
    vgic_gsi = 0
    perf_gsi = 0

    if gic is not None:
        reg = read_prop_words(gic, 'reg') or []
        # reg often contains pairs of base/size. take first pair as GICD base, second pair as GICR base
        pairs = pair_words(reg)
        if pairs:
            gicd = words_to_int([pairs[0][0]])
        if len(pairs) > 1:
            gicr = words_to_int([pairs[1][0]])

        rstride = read_prop_words(gic, 'redistributor-stride')
        if rstride:
            # support 1 or 2 words
            gicr_stride = words_to_int(rstride)

        # interrupts property (triplets)
        ints = read_prop_words(gic, 'interrupts')
        if ints:
            triplets = parse_interrupt_triplets(ints)
            # Mapping heuristic:
            # - If there are two or more triplets: [0] -> VGIC maintenance, [1] -> performance
            # - If there is only one triplet: use it for both performance and VGIC maintenance
            if len(triplets) >= 2:
                vgic_gsi = triplet_to_gsi(triplets[0])
                perf_gsi = triplet_to_gsi(triplets[1])
            elif len(triplets) == 1:
                vgic_gsi = triplet_to_gsi(triplets[0])
                perf_gsi = triplet_to_gsi(triplets[0])


    its_nodes = find_its_nodes(root)
    if its_nodes:
        num_its = len(its_nodes)
        # take first its reg if present
        r = read_prop_words(its_nodes[0], 'reg')
        if r:
            pairs = pair_words(r)
            if pairs:
                gicits = words_to_int([pairs[0][0]])

    # Fallbacks if none found
    if vgic_gsi == 0:
        # try to look for a property named 'vgic-maintenance-interrupt' somewhere
        stack = [root]
        while stack and vgic_gsi == 0:
            n = stack.pop()
            for sd in n.subdata:
                if not hasattr(sd, 'subdata'):
                    continue
                stack.append(sd)
                for p in sd.subdata:
                    if hasattr(p, 'name') and 'vgic' in p.name and hasattr(p, 'words'):
                        w = p.words
                        trip = (w[0] if len(w) > 0 else 0, w[1] if len(w) > 1 else 0, w[2] if len(w) > 2 else 0)
                        vgic_gsi = triplet_to_gsi(trip)
    if perf_gsi == 0:
        stack = [root]
        while stack and perf_gsi == 0:
            n = stack.pop()
            for sd in n.subdata:
                if not hasattr(sd, 'subdata'):
                    continue
                stack.append(sd)
                for p in sd.subdata:
                    if hasattr(p, 'name') and 'performance' in p.name and hasattr(p, 'words'):
                        w = p.words
                        trip = (w[0] if len(w) > 0 else 0, w[1] if len(w) > 1 else 0, w[2] if len(w) > 2 else 0)
                        perf_gsi = triplet_to_gsi(trip)

    # generate GICC declarations and MPIDR macros
    gicc_lines = []
    mpidr_lines = []
    for i in range(num_cores):
        # Default MPIDR value: 0x000000X00 where X is cpu id (placed in the 2nd least-significant byte)
        mpidr = 0x00000000 | (i << 8)
        mpidr_lines.append(f"#define GICC_MPIDR_CORE{i} 0x{mpidr:08x}ULL")
        gicc_lines.append(f"    MADT_DECLARE_GICC_STRUCTURE({i}, {i}, GICC_MPIDR_CORE{i}), // Core {i}")

    if mpidr_lines:
        mpidr_macros = '/* Fix Me: MPIDR values are placeholders, please verify per-platform */\n' + '\n'.join(mpidr_lines) + '\n'
    else:
        mpidr_macros = ''

    gic_its_decl = ''
    gic_its_macro = ''
    if num_its > 0:
        gic_its_decl = f"    MADT_DECLARE_GIC_ITS_STRUCTURE(0, GIC_ITS_BASE_ADDRESS),"
        gic_its_macro = f"#define GIC_ITS_BASE_ADDRESS {gicits:#010x}ULL\n"

    # Detect GIC version from compatible strings (only support v3 and v4 intentionally)
    gic_version_macro = 'GIC_INVALID'
    if gic is not None:
        comp = read_prop_strings(gic, 'compatible') or []
        # Normalize and search for 'gic-v4' or 'gic-v3'
        comp_strs = [s.lower() for s in comp if s]
        if any('gic-v4' in s for s in comp_strs):
            gic_version_macro = 'GIC_V4'
        elif any('gic-v3' in s for s in comp_strs):
            gic_version_macro = 'GIC_V3'
        else:
            # fallback: look for 'gic' and leave as invalid to force user check
            if any('gic' in s for s in comp_strs):
                gic_version_macro = 'GIC_INVALID'

    # warn user if unsupported
    if gic_version_macro == 'GIC_INVALID':
        print('Warning: GIC version not recognized as v3 or v4; GIC_VERSION will be set to GIC_INVALID')

    # Provide a hint comment when GIC is detected as v3 (some DTB use same compatible for v3/v4)
    if gic_version_macro == 'GIC_V3':
        gic_version_comment = '/* Fix GIC version if using GICv4 */'
    else:
        gic_version_comment = ''

    header = HEADER_TEMPLATE.format(
        gicd=gicd,
        gicr=gicr,
        gicr_stride=gicr_stride,
        gic_version_macro=gic_version_macro,
        gic_version_comment=gic_version_comment,
        gicits=gicits,
        perf_gsi=perf_gsi,
        vgic_gsi=vgic_gsi,
        num_its=num_its,
        gic_its_decl=gic_its_decl,
        gic_its_macro=gic_its_macro,
        gicc_decls='\n'.join(gicc_lines),
        mpidr_macros=mpidr_macros
    )

    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(header)
    print(f"Wrote MADT header to: {out_path}")


def main():
    parser = argparse.ArgumentParser(description='Generate MADT header from DTB')
    parser.add_argument('dtb', type=Path, help='DTB file to parse')
    parser.add_argument('--vendor', required=True, help='Vendor directory under include (e.g. qcom, mtk)')
    parser.add_argument('-o', '--output', type=Path, default=None, help='Output header path (default: include/<vendor>/<dtb_stem>/madt.h)')
    args = parser.parse_args()

    # Compute default output if not provided
    if args.output is None:
        dtb_stem = args.dtb.name.split('.')[0]
        args.output = Path('include') / 'vendor' / args.vendor / dtb_stem / 'madt.h'

    # If user passed a directory, append filename
    if args.output.is_dir():
        args.output = args.output / 'madt.h'

    if not args.dtb.exists():
        print(f"DTB not found: {args.dtb}")
        return 2

    generate_header(args.output, args.dtb, platform_name=args.dtb.name.split('.')[0])
    return 0


if __name__ == '__main__':
    sys.exit(main())
