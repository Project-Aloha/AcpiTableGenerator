#!/usr/bin/env python3
"""
PPTT 拓扑结构验证脚本
跨平台支持（Windows/Linux/macOS）
"""

import sys
import re
from pathlib import Path


def print_box(text):
    """打印带边框的文本"""
    width = 50
    print(f"\n╔{'═' * width}╗")
    print(f"║ {text.center(width - 2)} ║")
    print(f"╚{'═' * width}╝")


def analyze_dsl(dsl_file):
    """分析 DSL 文件并提取关键信息"""
    if not dsl_file.exists():
        return None
    
    with open(dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    info = {
        'file_size': None,
        'revision': None,
        'checksum': None,
        'oem_id': None,
        'physical_packages': 0,
        'clusters': 0,
        'cores': 0,
        'caches': []
    }
    
    # 提取表头信息
    if m := re.search(r'Table Length : ([0-9A-F]+)', content):
        info['file_size'] = int(m.group(1), 16)
    
    if m := re.search(r'Revision : ([0-9A-F]+)', content):
        info['revision'] = int(m.group(1), 16)
    
    if m := re.search(r'Checksum : ([0-9A-F]+)', content):
        info['checksum'] = m.group(1)
    
    if m := re.search(r'Oem ID : "([^"]+)"', content):
        info['oem_id'] = m.group(1).strip()
    
    # 统计节点类型
    info['physical_packages'] = len(re.findall(r'Physical package : 1', content))
    
    # 统计 Processor Hierarchy 节点（排除 Physical package）
    hierarchy_nodes = re.findall(r'Subtable Type : 00 \[Processor Hierarchy Node\]', content)
    info['clusters'] = len(hierarchy_nodes) - info['physical_packages']  # 减去 package 节点
    
    # 提取缓存信息
    cache_pattern = re.compile(
        r'Subtable Type : 01 \[Cache Type\].*?'
        r'Size : ([0-9A-F]+).*?'
        r'Associativity : ([0-9A-F]+).*?'
        r'Cache Type : (\d)',
        re.DOTALL
    )
    
    for match in cache_pattern.finditer(content):
        size_hex = match.group(1)
        assoc_hex = match.group(2)
        cache_type = match.group(3)
        
        size_bytes = int(size_hex, 16)
        associativity = int(assoc_hex, 16)
        
        # 确定缓存类型
        type_names = {
            '0': 'Data',
            '1': 'Instruction',
            '2': 'Unified'
        }
        type_name = type_names.get(cache_type, 'Unknown')
        
        # 推测缓存级别（基于大小）
        if size_bytes < 256 * 1024:  # < 256KB
            level = 'L1'
        elif size_bytes < 2 * 1024 * 1024:  # < 2MB
            level = 'L2'
        else:
            level = 'L3'
        
        info['caches'].append({
            'level': level,
            'type': type_name,
            'size': size_bytes,
            'associativity': associativity
        })
    
    return info


def format_size(size_bytes):
    """格式化字节大小"""
    if size_bytes < 1024:
        return f"{size_bytes} B"
    elif size_bytes < 1024 * 1024:
        return f"{size_bytes // 1024} KB"
    else:
        return f"{size_bytes // (1024 * 1024)} MB"


def verify_platform(platform_name, build_dir):
    """验证单个平台"""
    print(f"\n📱 平台: {platform_name}")
    print("─" * 50)
    
    platform_dir = build_dir / platform_name
    aml_file = platform_dir / "builtin" / "PPTT.aml"
    dsl_file = platform_dir / "src" / "PPTT.dsl"
    
    if not aml_file.exists():
        print(f"❌ 未找到 PPTT.aml")
        return False
    
    # 显示文件大小
    size = aml_file.stat().st_size
    print(f"📦 文件大小: {size} bytes")
    
    # 分析 DSL（如果存在）
    if dsl_file.exists():
        print("🔍 缓存层次结构:")
        info = analyze_dsl(dsl_file)
        
        if info:
            print(f"  • Revision: {info['revision']}")
            print(f"  • Checksum: 0x{info['checksum']}")
            print(f"  • OEM ID: {info['oem_id']}")
            print(f"  • Physical Packages: {info['physical_packages']}")
            
            # 按级别和类型分组缓存
            cache_summary = {}
            for cache in info['caches']:
                key = f"{cache['level']} {cache['type']}"
                if key not in cache_summary:
                    cache_summary[key] = []
                cache_summary[key].append(cache)
            
            print(f"\n  缓存配置:")
            for key, caches in sorted(cache_summary.items()):
                unique_configs = {}
                for cache in caches:
                    config_key = (cache['size'], cache['associativity'])
                    unique_configs[config_key] = unique_configs.get(config_key, 0) + 1
                
                for (size, assoc), count in unique_configs.items():
                    count_str = f" × {count}" if count > 1 else ""
                    print(f"    • {key}: {format_size(size)}, {assoc}-way{count_str}")
        
        return True
    else:
        print("⚠️  DSL 文件不存在（iasl 未安装或未反编译）")
        return True


def main():
    """主函数"""
    print_box("PPTT 拓扑结构验证")
    
    # 检测项目根目录
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    build_dir = root_dir / "build"
    
    if not build_dir.exists():
        print("❌ 构建目录不存在")
        print(f"   期望目录: {build_dir}")
        return 1
    
    # 检测所有平台
    platforms = []
    for item in build_dir.iterdir():
        if item.is_dir() and (item / "builtin" / "PPTT.aml").exists():
            platforms.append(item.name)
    
    if not platforms:
        print("❌ 未找到任何已构建的平台")
        return 1
    
    # 验证每个平台
    all_passed = True
    for platform in sorted(platforms):
        if not verify_platform(platform, build_dir):
            all_passed = False
    
    # 总结
    print(f"\n{'✅' if all_passed else '⚠️'} 验证完成\n")
    return 0 if all_passed else 1


if __name__ == "__main__":
    sys.exit(main())
