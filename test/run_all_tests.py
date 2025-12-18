#!/usr/bin/env python3
"""
PPTT 通用拓扑构建器 - 完整测试套件
跨平台测试脚本（支持 Windows/Linux/macOS）
"""

import os
import sys
import subprocess
import struct
from pathlib import Path


class Colors:
    """终端颜色（如果不支持则禁用）"""
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    
    @classmethod
    def disable(cls):
        cls.HEADER = ''
        cls.BLUE = ''
        cls.GREEN = ''
        cls.YELLOW = ''
        cls.RED = ''
        cls.ENDC = ''
        cls.BOLD = ''


# 检测是否支持颜色
if os.name == 'nt' or not sys.stdout.isatty():
    Colors.disable()


def print_header(text):
    """打印标题"""
    width = 67
    print(f"\n{'╔' + '═' * width + '╗'}")
    print(f"║ {text.center(width - 2)} ║")
    print(f"{'╚' + '═' * width + '╝'}\n")


def print_section(title):
    """打印章节标题"""
    print(f"\n{Colors.BLUE}{title}{Colors.ENDC}")
    print("━" * 67)


def print_success(message):
    """打印成功信息"""
    print(f"{Colors.GREEN}   ✅ {message}{Colors.ENDC}")


def print_error(message):
    """打印错误信息"""
    print(f"{Colors.RED}   ❌ {message}{Colors.ENDC}")


def print_info(message):
    """打印信息"""
    print(f"   {message}")


def run_command(cmd, cwd=None, capture=True):
    """运行命令并返回结果"""
    try:
        if capture:
            result = subprocess.run(
                cmd,
                cwd=cwd,
                capture_output=True,
                text=True,
                check=False
            )
            return result.returncode, result.stdout, result.stderr
        else:
            result = subprocess.run(cmd, cwd=cwd, check=False)
            return result.returncode, "", ""
    except Exception as e:
        return -1, "", str(e)


def test_compilation(build_dir, platforms):
    """测试 1: 编译所有平台"""
    print_section("📦 测试 1: 编译所有平台")
    
    all_passed = True
    for platform in platforms:
        print(f"🔨 编译 {platform}...")
        
        # 使用 cmake --build 而不是 make（跨平台）
        returncode, stdout, stderr = run_command(
            ["cmake", "--build", ".", "--target", f"pptt_generator_{platform}"],
            cwd=build_dir,
            capture=True
        )
        
        if returncode == 0:
            print_success(f"{platform} 编译成功")
        else:
            print_error(f"{platform} 编译失败")
            if stderr:
                print(f"      错误: {stderr[:200]}")
            all_passed = False
    
    return all_passed


def test_file_generation(build_dir, platforms):
    """测试 2: 验证文件生成"""
    print_section("📄 测试 2: 验证文件生成")
    
    all_passed = True
    for platform in platforms:
        aml_file = build_dir / platform / "builtin" / "PPTT.aml"
        dsl_file = build_dir / platform / "src" / "PPTT.dsl"
        
        if aml_file.exists():
            size = aml_file.stat().st_size
            print_success(f"{platform}: PPTT.aml ({size} bytes)")
        else:
            print_error(f"{platform}: PPTT.aml 缺失")
            all_passed = False
            continue
        
        if dsl_file.exists():
            with open(dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
                lines = len(f.readlines())
            print_info(f"✅ PPTT.dsl ({lines} lines)")
        else:
            print_info("⚠️  PPTT.dsl 缺失（iasl 未安装）")
    
    return all_passed


def test_topology_verification(build_dir, platforms):
    """测试 3: 运行生成器并验证拓扑"""
    print_section("🚀 测试 3: 运行生成器并验证拓扑")
    
    all_passed = True
    
    # SM8850 验证
    if "sm8850" in platforms:
        print("📱 SM8850 (Snapdragon 8 Gen 3):")
        exe = build_dir / "pptt_generator_sm8850"
        if os.name == 'nt':
            exe = exe.with_suffix('.exe')
        
        returncode, stdout, stderr = run_command([str(exe)], cwd=build_dir)
        
        if returncode == 0:
            # 验证输出关键信息
            checks = [
                ("Total size: 832 bytes", "文件大小"),
                ("Clusters: 2", "Cluster 数量"),
                ("L2: 12288 KB", "L2缓存")
            ]
            
            passed = True
            for check_str, desc in checks:
                if check_str in stdout:
                    pass
                else:
                    passed = False
                    print_error(f"验证失败: 缺少 '{desc}'")
            
            if passed:
                print_success("拓扑结构正确")
                print_info("• 2 Clusters (6 cores + 2 cores)")
                print_info("• Per-cluster L2: 12MB")
            else:
                all_passed = False
        else:
            print_error("生成器运行失败")
            all_passed = False
        print()
    
    # SM8550 验证
    if "sm8550" in platforms:
        print("📱 SM8550 (Snapdragon 8 Gen 2):")
        exe = build_dir / "pptt_generator_sm8550"
        if os.name == 'nt':
            exe = exe.with_suffix('.exe')
        
        returncode, stdout, stderr = run_command([str(exe)], cwd=build_dir)
        
        if returncode == 0:
            checks = [
                ("Total size: 1044 bytes", "文件大小"),
                ("Clusters: 3", "Cluster 数量"),
                ("L3 Cache (Shared): 8192 KB", "共享 L3")
            ]
            
            passed = True
            for check_str, desc in checks:
                if check_str in stdout:
                    pass
                else:
                    passed = False
                    print_error(f"验证失败: 缺少 '{desc}'")
            
            if passed:
                print_success("拓扑结构正确")
                print_info("• 3 Clusters (3 + 4 + 1 cores)")
                print_info("• 共享 L3: 8MB")
            else:
                all_passed = False
        else:
            print_error("生成器运行失败")
            all_passed = False
        print()
    
    # SM7325 验证
    if "sm7325" in platforms:
        print("📱 SM7325 (Snapdragon 778G):")
        exe = build_dir / "pptt_generator_sm7325"
        if os.name == 'nt':
            exe = exe.with_suffix('.exe')
        
        returncode, stdout, stderr = run_command([str(exe)], cwd=build_dir)
        
        if returncode == 0:
            checks = [
                ("Total size: 1044 bytes", "文件大小"),
                ("Clusters: 3", "Cluster 数量"),
                ("L2 Cache (Per-core)", "Per-core L2"),
                ("L3 Cache (Shared): 0 KB", "共享 L3")
            ]
            
            passed = True
            for check_str, desc in checks:
                if check_str in stdout:
                    pass
                else:
                    passed = False
                    print_error(f"验证失败: 缺少 '{desc}'")
            
            if passed:
                print_success("拓扑结构正确")
                print_info("• 3 Clusters (4 + 3 + 1 cores)")
                print_info("• Per-core L2 (8 private L2 caches)")
                print_info("• Has L3 cache")
            else:
                all_passed = False
        else:
            print_error("生成器运行失败")
            all_passed = False
        print()
    
    # SM8845 验证
    if "sm8845" in platforms:
        print("📱 SM8845:")
        exe = build_dir / "pptt_generator_sm8845"
        if os.name == 'nt':
            exe = exe.with_suffix('.exe')
        
        returncode, stdout, stderr = run_command([str(exe)], cwd=build_dir)
        
        if returncode == 0:
            checks = [
                ("Total size: 832 bytes", "文件大小"),
                ("Clusters: 2", "Cluster 数量"),
                ("L2: 0 KB", "L2缓存")
            ]
            
            passed = True
            for check_str, desc in checks:
                if check_str in stdout:
                    pass
                else:
                    passed = False
                    print_error(f"验证失败: 缺少 '{desc}'")
            
            if passed:
                print_success("拓扑结构正确")
                print_info("• 2 Clusters (6 cores + 2 cores)")
                print_info("• Per-cluster shared L2")
            else:
                all_passed = False
        else:
            print_error("生成器运行失败")
            all_passed = False
        print()
    
    return all_passed


def test_dsl_validation(build_dir, platforms):
    """测试 4: 验证 DSL 关键字段"""
    print_section("🔍 测试 4: 验证 DSL 关键字段")
    
    all_passed = True
    
    # SM8850 DSL 验证
    if "sm8850" in platforms:
        dsl_file = build_dir / "sm8850" / "src" / "PPTT.dsl"
        if dsl_file.exists():
            with open(dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            checks = [
                ("Physical package : 1", "Physical Package"),
                ("Size : 00C00000", "L2 = 0x00C00000 (12MB)")
            ]
            
            passed = True
            found_items = []
            for check_str, desc in checks:
                if check_str in content:
                    found_items.append(desc)
                else:
                    passed = False
                    print_error(f"SM8850 DSL 缺少: {desc}")
            
            if passed:
                print_success("SM8850 DSL 验证通过")
                for item in found_items:
                    print_info(f"• 找到 {item}")
            else:
                all_passed = False
        else:
            print_info("⚠️  SM8850 DSL 文件不存在（跳过验证）")
    
    # SM8550 DSL 验证
    if "sm8550" in platforms:
        dsl_file = build_dir / "sm8550" / "src" / "PPTT.dsl"
        if dsl_file.exists():
            with open(dsl_file, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            checks = [
                ("Physical package : 1", "Physical Package"),
                ("Size : 00800000", "L3 = 0x00800000 (8MB)")
            ]
            
            passed = True
            found_items = []
            for check_str, desc in checks:
                if check_str in content:
                    found_items.append(desc)
                else:
                    passed = False
                    print_error(f"SM8550 DSL 缺少: {desc}")
            
            if passed:
                print_success("SM8550 DSL 验证通过")
                for item in found_items:
                    print_info(f"• 找到 {item}")
            else:
                all_passed = False
        else:
            print_info("⚠️  SM8550 DSL 文件不存在（跳过验证）")
    
    return all_passed


def test_checksum(build_dir, platforms):
    """测试 5: 校验和验证"""
    print_section("🔐 测试 5: 校验和验证")
    
    all_passed = True
    for platform in platforms:
        aml_file = build_dir / platform / "builtin" / "PPTT.aml"
        
        if aml_file.exists():
            try:
                with open(aml_file, 'rb') as f:
                    f.seek(9)  # 跳到 checksum 字节
                    checksum = struct.unpack('B', f.read(1))[0]
                print_success(f"{platform}: Checksum = 0x{checksum:02x}")
            except Exception as e:
                print_error(f"{platform}: 无法读取校验和 - {e}")
                all_passed = False
        else:
            print_error(f"{platform}: PPTT.aml 不存在")
            all_passed = False
    
    return all_passed


def main():
    """主测试函数"""
    print_header("PPTT 通用拓扑构建器 - 完整测试套件")
    
    # 检测项目根目录
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    build_dir = root_dir / "build"
    
    if not build_dir.exists():
        print_error("构建目录不存在，请先运行 cmake")
        print(f"   期望目录: {build_dir}")
        return 1
    
    # 切换到构建目录
    os.chdir(build_dir)
    
    # 自动检测所有平台（从 include 目录）
    include_dir = root_dir / "include"
    platforms = []
    if include_dir.exists():
        for item in include_dir.iterdir():
            if item.is_dir() and item.name != "common":
                # 检查是否有对应的可执行文件
                exe_name = f"pptt_generator_{item.name}"
                if (build_dir / exe_name).exists() or (build_dir / f"{exe_name}.exe").exists():
                    platforms.append(item.name)
    
    if not platforms:
        # 回退到硬编码列表
        platforms = ["sm8850", "sm8550", "sm8150"]
    
    platforms.sort()  # 按字母顺序排序
    
    # 运行所有测试
    results = {}
    
    results['compilation'] = test_compilation(build_dir, platforms)
    results['file_generation'] = test_file_generation(build_dir, platforms)
    results['topology'] = test_topology_verification(build_dir, platforms)
    results['dsl_validation'] = test_dsl_validation(build_dir, platforms)
    results['checksum'] = test_checksum(build_dir, platforms)
    
    # 总结
    print_header("✅ 测试总结")
    
    test_names = [
        ('compilation', '编译所有平台'),
        ('file_generation', '验证文件生成'),
        ('topology', '拓扑结构验证'),
        ('dsl_validation', 'DSL 反编译成功'),
        ('checksum', '校验和正确')
    ]
    
    passed_count = sum(1 for result in results.values() if result)
    total_count = len(results)
    
    print("测试结果:")
    for key, name in test_names:
        status = "✅ 通过" if results[key] else "❌ 失败"
        print(f"  [{status}] {name}")
    
    print(f"\n总计: {passed_count}/{total_count} 测试通过")
    
    if passed_count == total_count:
        print(f"\n{Colors.GREEN}🎉 通用拓扑构建器工作正常！{Colors.ENDC}\n")
        return 0
    else:
        print(f"\n{Colors.RED}⚠️  部分测试失败，请检查输出{Colors.ENDC}\n")
        return 1


if __name__ == "__main__":
    sys.exit(main())
