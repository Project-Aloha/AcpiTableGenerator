#!/usr/bin/env python3
"""
验证 per-core L2 架构的 PPTT 表
"""

import re
import sys
from pathlib import Path


def parse_pptt_dsl(dsl_file):
    """解析 PPTT DSL 文件"""
    with open(dsl_file, 'r') as f:
        content = f.read()
    
    # 提取所有缓存大小
    cache_sizes = []
    for match in re.finditer(r'\[.*?\]\s+Size\s*:\s*([0-9A-F]+)', content):
        size_hex = match.group(1)
        size_bytes = int(size_hex, 16)
        size_kb = size_bytes // 1024
        cache_sizes.append(size_kb)
    
    # 提取缓存属性 (用于区分 L1D/L1I/L2/L3)
    cache_attrs = []
    for match in re.finditer(r'^\[.*?\]\s+Attributes\s*:\s*([0-9A-F]+)', content, re.MULTILINE):
        attr_hex = match.group(1)
        attr = int(attr_hex, 16)
        cache_attrs.append(attr)
    
    # 分类缓存
    # Attributes: 0x02 = Data, 0x04 = Instruction, 0x0A = Unified
    l1d_sizes = []
    l1i_sizes = []
    l2_sizes = []
    l3_sizes = []
    
    for size, attr in zip(cache_sizes, cache_attrs):
        if attr == 0x02:  # Data cache (L1D)
            l1d_sizes.append(size)
        elif attr == 0x04:  # Instruction cache (L1I)
            l1i_sizes.append(size)
        elif attr == 0x0A:  # Unified cache (L2/L3)
            if size >= 8192:
                l3_sizes.append(size)
            else:
                l2_sizes.append(size)
    
    return {
        'l1d': l1d_sizes,
        'l1i': l1i_sizes,
        'l2': l2_sizes,
        'l3': l3_sizes
    }


def verify_sm8550(caches):
    """验证 SM8550 的 per-core L2 配置"""
    print('╔═══════════════════════════════════════════════════════════════════╗')
    print('║         SM8550 Per-Core L2 验证                                   ║')
    print('╚═══════════════════════════════════════════════════════════════════╝')
    print()
    
    errors = []
    
    # 验证 L1D (8 个核心)
    if len(caches['l1d']) != 8:
        errors.append(f"L1D 数量错误: 期望 8, 实际 {len(caches['l1d'])}")
    expected_l1d = [32] * 3 + [64] * 4 + [64]  # 3E + 4M + 1P
    if caches['l1d'] != expected_l1d:
        errors.append(f"L1D 配置错误: 期望 {expected_l1d}, 实际 {caches['l1d']}")
    
    # 验证 L1I (8 个核心)
    if len(caches['l1i']) != 8:
        errors.append(f"L1I 数量错误: 期望 8, 实际 {len(caches['l1i'])}")
    expected_l1i = [32] * 3 + [64] * 4 + [64]  # 3E + 4M + 1P
    if caches['l1i'] != expected_l1i:
        errors.append(f"L1I 配置错误: 期望 {expected_l1i}, 实际 {caches['l1i']}")
    
    # 验证 L2 (8 个 per-core)
    if len(caches['l2']) != 8:
        errors.append(f"L2 数量错误: 期望 8, 实际 {len(caches['l2'])}")
    expected_l2 = [128] * 3 + [512] * 4 + [1024]  # 3E + 4M + 1P
    if caches['l2'] != expected_l2:
        errors.append(f"L2 配置错误: 期望 {expected_l2}, 实际 {caches['l2']}")
    
    # 验证 L3 (1 个共享)
    if len(caches['l3']) != 1:
        errors.append(f"L3 数量错误: 期望 1, 实际 {len(caches['l3'])}")
    if caches['l3'] != [8192]:
        errors.append(f"L3 配置错误: 期望 [8192], 实际 {caches['l3']}")
    
    # 打印结果
    print('缓存统计:')
    print(f'  L1D: {len(caches["l1d"])} 个 - {caches["l1d"]}')
    print(f'  L1I: {len(caches["l1i"])} 个 - {caches["l1i"]}')
    print(f'  L2:  {len(caches["l2"])} 个 - {caches["l2"]}')
    print(f'  L3:  {len(caches["l3"])} 个 - {caches["l3"]}')
    print()
    
    if errors:
        print('❌ 验证失败:')
        for err in errors:
            print(f'  • {err}')
        return False
    else:
        print('✅ 验证通过！')
        print()
        print('Per-Core L2 配置:')
        print('  • CPU0 (E): 128KB L2')
        print('  • CPU1 (E): 128KB L2 (与 CPU2 共享)')
        print('  • CPU2 (E): 128KB L2 (与 CPU1 共享)')
        print('  • CPU3 (M): 512KB L2')
        print('  • CPU4 (M): 512KB L2')
        print('  • CPU5 (M): 512KB L2')
        print('  • CPU6 (M): 512KB L2')
        print('  • CPU7 (P): 1024KB L2')
        print()
        print('✅ 完全匹配设备树拓扑！')
        return True


def main():
    if len(sys.argv) != 2:
        print('用法: verify_per_core_l2.py <platform>')
        print('示例: verify_per_core_l2.py sm8550')
        sys.exit(1)
    
    platform = sys.argv[1]
    base_path = Path(__file__).parent.parent
    dsl_file = base_path / 'build' / platform / 'builtin' / 'PPTT.dsl'
    
    if not dsl_file.exists():
        print(f'错误: 找不到 {dsl_file}')
        print(f'请先编译: make pptt_generator_{platform}')
        sys.exit(1)
    
    caches = parse_pptt_dsl(dsl_file)
    
    if platform == 'sm8550':
        success = verify_sm8550(caches)
    else:
        print(f'错误: 不支持平台 {platform}')
        print('当前仅支持: sm8550')
        sys.exit(1)
    
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()
