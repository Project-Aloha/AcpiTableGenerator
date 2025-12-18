# Testing Guide

**English** | **[简体中文](TESTING.zh.md)**

---

This project provides a complete cross-platform test suite supporting **Windows / Linux / macOS**.

## 📋 Table of Contents

- [Testing Tools List](#testing-tools-list)
  - [Complete Test Suite](#1-complete-test-suite)
  - [Topology Validation Tool](#2-topology-validation-tool)
  - [AML Validation Tool](#3-aml-validation-tool)
  - [Per-Core L2 Verification](#4-per-core-l2-verification-tool)
  - [Configuration Comparison Tool](#5-configuration-comparison-tool)
- [Dependencies](#dependencies)
- [Quick Start](#quick-start)
- [Common Issues](#common-issues)
- [Best Practices](#testing-best-practices)

## Testing Tools List

### 1. Complete Test Suite

**File**: `test/run_all_tests.py`  
**Language**: Python 3.6+  
**Function**: Run all tests and generate complete report

#### Test Coverage

| 测试项 | 描述 | 覆盖平台 |
|--------|------|----------|
| ✅ 编译验证 | 确保所有平台成功编译 | 全部 9 个平台 |
| ✅ 文件生成 | 验证 AML 和 DSL 文件正确生成 | 全部平台 |
| ✅ 拓扑结构 | 检查 CPU 核心数、Cluster 配置 | 全部平台 |
| ✅ 缓存配置 | 验证 L1/L2/L3 缓存参数 | 全部平台 |
| ✅ 校验和 | 确保 ACPI 表校验和正确 | 全部平台 |
| ✅ DSL 语法 | 验证 DSL 文件格式正确 | 全部平台 |

#### Usage

```bash
# 方法 1: 在项目根目录运行
python3 test/run_all_tests.py

# 方法 2: 在 build 目录运行
cd build
python3 ../test/run_all_tests.py

# 方法 3: 指定平台
python3 test/run_all_tests.py --platform sm8850

# 方法 4: 跳过编译（仅验证）
python3 test/run_all_tests.py --skip-build
```

#### Example Output

```
╔═══════════════════════════════════════════════════════════════════╗
║                  PPTT 通用拓扑构建器 - 完整测试套件               ║
╚═══════════════════════════════════════════════════════════════════╝

📦 测试 1: 编译所有平台
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
🔨 编译 sm8150...
   ✅ sm8150 编译成功 (1044 bytes, checksum: 0x84)
🔨 编译 sm8250...
   ✅ sm8250 编译成功 (1044 bytes, checksum: 0x65)
🔨 编译 sm8850...
   ✅ sm8850 编译成功 (832 bytes, checksum: 0x70)
...

📊 测试结果统计:
  • 总测试数: 45
  • 通过: 45
  • 失败: 0
  • 跳过: 0
  • 成功率: 100%

✅ All tests passed！
```

---

### 2. Topology Validation Tool

**File**: `test/verify_topology.py`  
**Language**: Python 3.6+  
**Function**: Verify and visualize PPTT topology structure

#### Features

- 🔍 自动检测所有已构建平台
- 📊 解析 DSL 文件提取关键信息
- 📈 显示缓存配置摘要
- 🎯 分组显示相同配置的缓存
- 📋 对比多个平台的差异

#### Usage

```bash
# 验证所有平台
python3 test/verify_topology.py

# 验证特定平台
python3 test/verify_topology.py sm8850

# 验证多个平台并对比
python3 test/verify_topology.py sm8850 sm8750

# # Verbose mode（显示所有缓存节点）
python3 test/verify_topology.py --verbose
```

#### Example Output

```
╔══════════════════════════════════════════════════╗
║                PPTT 拓扑结构验证                  ║
╚══════════════════════════════════════════════════╝

📱 平台: sm8850
──────────────────────────────────────────────────
📦 文件大小: 832 bytes
🔍 缓存层次结构:
  • Revision: 3
  • Checksum: 0x70
  • OEM ID: QCOM
  • OEM Table ID: QCOMEDK2
  • Physical Packages: 1

  缓存配置:
    • L1 Data: 64 KB, 4-way × 8
    • L1 Instruction: 64 KB, 4-way × 8
    • L3 Unified: 12 MB, 16-way × 2

  拓扑结构:
    • 2 Clusters
    • Cluster 0: 6 cores (M-Core)
    • Cluster 1: 2 cores (P-Core)
    • L2 架构: Per-Cluster (12MB × 2)
```

---

### 3. AML Validation Tool

**File**: `test/aml_validator.py`  
**Language**: Python 3.6+  
**Function**: Verify PPTT AML file compliance with ACPI specification

#### Validation Items

| 验证项 | 描述 | 错误等级 |
|--------|------|----------|
| ✅ ACPI 表头 | 验证 36 字节表头结构 | 严重 |
| ✅ 表签名 | 确认签名为 "PPTT" | 严重 |
| ✅ 表长度 | 验证长度字段与文件大小一致 | 严重 |
| ✅ 校验和 | 计算并验证校验和 | 严重 |
| ✅ PPTT 节点 | 解析并验证节点结构 | 警告 |
| ✅ 节点长度 | 确保节点长度合法 | 警告 |

#### Usage

```bash
# 验证所有平台
python3 test/aml_validator.py

# 验证指定平台
python3 test/aml_validator.py sm8850

# 验证指定文件
python3 test/aml_validator.py build/sm8850/builtin/PPTT.aml

# 严格模式（将警告视为错误）
python3 test/aml_validator.py --strict
```

#### Example Output

```
============================================================
平台: sm8850
============================================================
验证文件: build/sm8850/builtin/PPTT.aml

ACPI 表头:
  Signature: PPTT
  Length: 832 bytes
  Revision: 3
  Checksum: 0x70
  OEM ID: QCOM
  OEM Table ID: QCOMEDK2
  OEM Revision: 0x00008850
  Creator ID: ALHA
  Creator Revision: 0x00000001

✅ 表签名正确: PPTT
✅ 表长度匹配: 832 bytes
✅ 校验和正确: 0x70
✅ 找到 12 个 PPTT 节点
✅ 所有节点长度合法

✅ PPTT 文件Validation passed！
```

---

### 4. Per-Core L2 Validation Tool

**File**: `test/verify_per_core_l2.py`  
**Language**: Python 3.6+  
**Function**: Verify Per-Core L2 architecture is correctly implemented

#### Validation Content

- ✅ 每个 CPU 核心是否有对应的 L2 节点
- ✅ L2 共享关系是否正确
- ✅ L2 大小/属性是否匹配配置
- ✅ NextLevelOfCache 指针是否正确

#### Usage

```bash
# 验证特定平台的 Per-Core L2
python3 test/verify_per_core_l2.py sm8650

# 验证所有Use Per-Core L2 的平台
python3 test/verify_per_core_l2.py --all
```

---

### 5. Configuration Comparison Tool

**File**: `test/pptt_validate.py`  
**Language**: Python 3.6+  
**Function**: Compare platform configuration with generated DSL consistency

#### Validation Content

- 解析平台头文件配置（`include/<platform>/pptt.h`）
- 解析 iasl 生成的 DSL 文件（`build/<platform>/src/PPTT.dsl`）
- 对比配置是否一致（缓存大小、关联度、核心数等）

#### Usage

```bash
# 验证特定平台
python3 test/pptt_validate.py sm8850

# 验证所有平台
python3 test/pptt_validate.py --all
```

---

## Dependencies

### Python 版本

- **Python 3.6+** （推荐 3.8+）
- 无需额外安装包，仅UseStandard library

### 系统工具

| 工具 | Purpose | Required | Installation |
|------|------|--------|----------|
| `cmake` | 构建系统 | **Required** | See CMake official website |
| `make` | 构建工具 | **Required** | Usually pre-installed |
| `iasl` | ACPI 反编译器 | Optional | See below |

#### 安装 iasl（Optional）

```bash
# Ubuntu/Debian
sudo apt install acpica-tools

# Arch Linux
sudo pacman -S acpica

# macOS
brew install acpica

# 检查是否安装
iasl -v
```

**Note**：If not installed `iasl`，test tools will skip DSL validation, does not affect AML file generation and validation.

---

## Quick Start

### Simplest Test Flow

```bash
# 1. 构建所有平台
cd build
cmake ..
make

# 2. # Run complete tests
python3 ../test/run_all_tests.py

# 3. 查看详细拓扑
python3 ../test/verify_topology.py

# 4. 验证 AML 文件
python3 ../test/aml_validator.py
```

### Test for Single Platform

```bash
# 1. # Build specific platform
make pptt_generator_sm8850

# 2. # Verify the platform
python3 ../test/verify_topology.py sm8850
python3 ../test/aml_validator.py sm8850
```

---

## Common Issues

### Q: Python 脚本提示找不到模块？

**A**: Check if Python version is >= 3.6：

```bash
python3 --version
```

If version is too low, upgrade Python or use system's Python 3.8+。

---

### Q: Windows 上如何运行 Python 脚本？

**A**: Use `python` instead of `python3`：

```cmd
# PowerShell # or CMD
python test\run_all_tests.py
python test\verify_topology.py
```

---

### Q: 测试脚本找不到构建目录？

**A**: Ensure running in project root or build directory:

```bash
# 方法 1: # Run from root directory
cd /path/to/acpi-table-generator
python3 test/run_all_tests.py

# 方法 2: # Run from build directory
cd /path/to/acpi-table-generator/build
python3 ../test/run_all_tests.py
```

---

### Q: 为什么 DSL 验证被跳过？

**A**: Need to install `iasl` tool. If DSL validation is not needed, this warning can be ignored.

```bash
# 检查是否安装
iasl -v

# If not installed，refer to "Dependencies" section
```

---

### Q: Test failed如何调试？

**A**: Use# Verbose mode运行测试：

```bash
# # Show verbose output
python3 test/run_all_tests.py --verbose

# 只Test failed的平台
python3 test/verify_topology.py sm8850 --verbose

# # View DSL file content
cat build/sm8850/src/PPTT.dsl
```

---

### Q: 可以在 CI/CD 中Use这些脚本吗？

**A**: Yes! Python scripts return standard exit codes:

- `0` = All tests passed
- `1` = 有Test failed

#### GitHub Actions Example

```yaml
name: Test PPTT

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    
    steps:
      - uses: actions/checkout@v3
      
      - name: Install dependencies
        run: |
          sudo apt update
          sudo apt install -y cmake acpica-tools
      
      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          make
      
      - name: Run tests
        run: |
          cd build
          python3 ../test/run_all_tests.py
```

#### GitLab CI Example

```yaml
test:
  image: ubuntu:latest
  before_script:
    - apt update && apt install -y cmake acpica-tools python3
  script:
    - mkdir build && cd build
    - cmake ..
    - make
    - python3 ../test/run_all_tests.py
```

---

### Q: 如何测试新添加的平台？

**A**: Follow these steps:

```bash
# 1. # Add platform configuration file
# include/sm8xxx/pptt.h

# 2. # Update CMakeLists.txt
# add_platform_target("sm8xxx" "SM8XXX")

# 3. # Build new platform
cd build
cmake ..
make pptt_generator_sm8xxx

# 4. # Verify new platform
python3 ../test/verify_topology.py sm8xxx
python3 ../test/aml_validator.py sm8xxx

# 5. # Run complete tests
python3 ../test/run_all_tests.py
```

---

## 添加自定义测试

如果你想添加自己的测试，可以参考现有脚本的结构：

### Example：Custom validation script

Create `test/my_custom_test.py`：

```python
#!/usr/bin/env python3
"""自定义 PPTT 验证脚本"""

import sys
from pathlib import Path

def validate_file_size(platform):
    """Validate if file size is within expected range"""
    aml_file = Path("build") / platform / "builtin" / "PPTT.aml"
    
    if not aml_file.exists():
        print(f"❌ {platform}: File does not exist")
        return False
    
    size = aml_file.stat().st_size
    
    # 文件大小应该在 800-1100 bytes 之间
    if not (800 <= size <= 1100):
        print(f"❌ {platform}: File size anomaly ({size} bytes)")
        return False
    
    print(f"✅ {platform}: File size normal ({size} bytes)")
    return True

def main():
    """Main function"""
    platforms = ["sm8150", "sm8250", "sm8350", "sm8450", 
                 "sm8475", "sm8550", "sm8650", "sm8750", "sm8850"]
    
    print("=" * 60)
    print("自定义文件大小验证")
    print("=" * 60)
    
    all_passed = True
    for platform in platforms:
        if not validate_file_size(platform):
            all_passed = False
    
    print("=" * 60)
    if all_passed:
        print("✅ 所有平台Validation passed")
        return 0
    else:
        print("❌ 部分平台Validation failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
```

Run custom test:

```bash
python3 test/my_custom_test.py
```

---

## Testing Best Practices

### 1. Testing in Development Process

```bash
# # After modifying code
cd build

# # Quick verification (test only modified platform)
make pptt_generator_sm8850
python3 ../test/verify_topology.py sm8850

# # Complete verification (before commit)
python3 ../test/run_all_tests.py
```

### 2. 提交前确保All tests passed

```bash
# Use pre-commit hook
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
cd build
python3 ../test/run_all_tests.py
exit $?
EOF

chmod +x .git/hooks/pre-commit
```

### 3. 调试时Use单独的验证工具

```bash
# # View topology structure only
python3 test/verify_topology.py sm8850

# # Validate AML format only
python3 test/aml_validator.py sm8850

# # Compare configuration with output
python3 test/pptt_validate.py sm8850

# # Verbose mode
python3 test/verify_topology.py sm8850 --verbose
```

### 4. Test on Different Platforms

```bash
# Linux/macOS
python3 test/run_all_tests.py

# Windows (PowerShell)
python test\run_all_tests.py

# Use虚拟环境
python3 -m venv venv
source venv/bin/activate  # Linux/macOS
# # or
.\venv\Scripts\activate   # Windows
python test/run_all_tests.py
```

### 5. Performance Testing

```bash
# # Measure build time
time make

# # Measure test time
time python3 test/run_all_tests.py

# # Batch build performance
time cmake --build . --parallel
```

---

## Continuous Integration Recommendations

### Test Matrix

Recommended to test multiple environments in CI/CD:

| OS | Python 版本 | CMake 版本 | iasl |
|----------|-------------|------------|------|
| Ubuntu 22.04 | 3.8, 3.10, 3.11 | 3.22+ | ✅ |
| Ubuntu 24.04 | 3.12 | 3.28+ | ✅ |
| macOS 13 | 3.11 | 3.25+ | ✅ |
| Windows Server 2022 | 3.9, 3.11 | 3.24+ | ❌ |

### Test Phases

1. **Compilation Phase**: Ensure all platforms compile successfully
2. **Validation Phase**: Run AML validation and topology validation
3. **Comparison Phase**: Compare configuration with generation results
4. **Regression Testing**: Compare file differences with previous version

---

## Troubleshooting

### Test script cannot find build directory

```bash
# # Check current directory
pwd

# # Check if build directory exists
ls -la build/

# 如果不存在，Create并构建
mkdir -p build && cd build
cmake ..
make
```

### Python Version Incompatible

```bash
# # Check Python version
python3 --version

# Use pyenv 切换版本
pyenv install 3.11
pyenv local 3.11
```

### iasl not found

```bash
# # Check if iasl is in PATH
which iasl

# If not installed，refer to "Dependencies" section安装
```

---

## Related Resources

- **[README.md](README.md)** - Project main documentation
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Architecture design documentation
- **[Python Official documentation](https://docs.python.org/3/)** - Python Standard library
- **[ACPICA 工具](https://www.acpica.org/)** - iasl and other ACPI tools

---

**💡 Tip**: 定期运行测试可以及早发现问题，建议在每次# After modifying code都# Run complete tests套件。
