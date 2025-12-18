# 测试指南

**[English](TESTING.md)** | **简体中文**

---

本项目提供了完整的跨平台测试套件，支持 **Windows / Linux / macOS**。

## 📋 目录

- [测试工具列表](#测试工具列表)
  - [完整测试套件](#1-完整测试套件)
  - [拓扑验证工具](#2-拓扑验证工具)
  - [AML 验证工具](#3-aml-验证工具)
  - [Per-Core L2 验证](#4-per-core-l2-验证工具)
  - [配置对比工具](#5-配置对比工具)
- [依赖要求](#依赖要求)
- [快速开始](#快速开始)
- [常见问题](#常见问题)
- [最佳实践](#测试最佳实践)

## 测试工具列表

### 1. 完整测试套件

**文件**: `test/run_all_tests.py`  
**语言**: Python 3.6+  
**功能**: 运行所有测试并生成完整报告

#### 测试内容

| 测试项 | 描述 | 覆盖平台 |
|--------|------|----------|
| ✅ 编译验证 | 确保所有平台成功编译 | 全部 9 个平台 |
| ✅ 文件生成 | 验证 AML 和 DSL 文件正确生成 | 全部平台 |
| ✅ 拓扑结构 | 检查 CPU 核心数、Cluster 配置 | 全部平台 |
| ✅ 缓存配置 | 验证 L1/L2/L3 缓存参数 | 全部平台 |
| ✅ 校验和 | 确保 ACPI 表校验和正确 | 全部平台 |
| ✅ DSL 语法 | 验证 DSL 文件格式正确 | 全部平台 |

#### 使用方法

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

#### 输出示例

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

✅ 所有测试通过！
```

---

### 2. 拓扑验证工具

**文件**: `test/verify_topology.py`  
**语言**: Python 3.6+  
**功能**: 验证和可视化 PPTT 拓扑结构

#### 功能特性

- 🔍 自动检测所有已构建平台
- 📊 解析 DSL 文件提取关键信息
- 📈 显示缓存配置摘要
- 🎯 分组显示相同配置的缓存
- 📋 对比多个平台的差异

#### 使用方法

```bash
# 验证所有平台
python3 test/verify_topology.py

# 验证特定平台
python3 test/verify_topology.py sm8850

# 验证多个平台并对比
python3 test/verify_topology.py sm8850 sm8750

# 详细模式（显示所有缓存节点）
python3 test/verify_topology.py --verbose
```

#### 输出示例

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

### 3. AML 验证工具

**文件**: `test/aml_validator.py`  
**语言**: Python 3.6+  
**功能**: 验证 PPTT AML 文件是否符合 ACPI 规范

#### 验证项目

| 验证项 | 描述 | 错误等级 |
|--------|------|----------|
| ✅ ACPI 表头 | 验证 36 字节表头结构 | 严重 |
| ✅ 表签名 | 确认签名为 "PPTT" | 严重 |
| ✅ 表长度 | 验证长度字段与文件大小一致 | 严重 |
| ✅ 校验和 | 计算并验证校验和 | 严重 |
| ✅ PPTT 节点 | 解析并验证节点结构 | 警告 |
| ✅ 节点长度 | 确保节点长度合法 | 警告 |

#### 使用方法

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

#### 输出示例

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

✅ PPTT 文件验证通过！
```

---

### 4. Per-Core L2 验证工具

**文件**: `test/verify_per_core_l2.py`  
**语言**: Python 3.6+  
**功能**: 验证 Per-Core L2 架构是否正确实现

#### 验证内容

- ✅ 每个 CPU 核心是否有对应的 L2 节点
- ✅ L2 共享关系是否正确
- ✅ L2 大小/属性是否匹配配置
- ✅ NextLevelOfCache 指针是否正确

#### 使用方法

```bash
# 验证特定平台的 Per-Core L2
python3 test/verify_per_core_l2.py sm8650

# 验证所有使用 Per-Core L2 的平台
python3 test/verify_per_core_l2.py --all
```

---

### 5. 配置对比工具

**文件**: `test/pptt_validate.py`  
**语言**: Python 3.6+  
**功能**: 对比平台配置与生成的 DSL 是否一致

#### 验证内容

- 解析平台头文件配置（`include/<platform>/pptt.h`）
- 解析 iasl 生成的 DSL 文件（`build/<platform>/src/PPTT.dsl`）
- 对比配置是否一致（缓存大小、关联度、核心数等）

#### 使用方法

```bash
# 验证特定平台
python3 test/pptt_validate.py sm8850

# 验证所有平台
python3 test/pptt_validate.py --all
```

---

## 依赖要求

### Python 版本

- **Python 3.6+** （推荐 3.8+）
- 无需额外安装包，仅使用标准库

### 系统工具

| 工具 | 用途 | 必需性 | 安装方法 |
|------|------|--------|----------|
| `cmake` | 构建系统 | **必需** | 参见 CMake 官网 |
| `make` | 构建工具 | **必需** | 通常预装 |
| `iasl` | ACPI 反编译器 | 可选 | 见下方 |

#### 安装 iasl（可选）

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

**注意**：如果未安装 `iasl`，测试工具会跳过 DSL 验证部分，不影响 AML 文件生成和验证。

---

## 快速开始

### 最简单的测试流程

```bash
# 1. 构建所有平台
cd build
cmake ..
make

# 2. 运行完整测试
python3 ../test/run_all_tests.py

# 3. 查看详细拓扑
python3 ../test/verify_topology.py

# 4. 验证 AML 文件
python3 ../test/aml_validator.py
```

### 针对单个平台的测试

```bash
# 1. 构建特定平台
make pptt_generator_sm8850

# 2. 验证该平台
python3 ../test/verify_topology.py sm8850
python3 ../test/aml_validator.py sm8850
```

---

## 常见问题

### Q: Python 脚本提示找不到模块？

**A**: 检查 Python 版本是否 >= 3.6：

```bash
python3 --version
```

如果版本过低，升级 Python 或使用系统的 Python 3.8+。

---

### Q: Windows 上如何运行 Python 脚本？

**A**: 使用 `python` 而不是 `python3`：

```cmd
# PowerShell 或 CMD
python test\run_all_tests.py
python test\verify_topology.py
```

---

### Q: 测试脚本找不到构建目录？

**A**: 确保在项目根目录或 build 目录运行：

```bash
# 方法 1: 从根目录运行
cd /path/to/acpi-table-generator
python3 test/run_all_tests.py

# 方法 2: 从 build 目录运行
cd /path/to/acpi-table-generator/build
python3 ../test/run_all_tests.py
```

---

### Q: 为什么 DSL 验证被跳过？

**A**: 需要安装 `iasl` 工具。如果不需要 DSL 验证，可以忽略此警告。

```bash
# 检查是否安装
iasl -v

# 如果未安装，参考"依赖要求"章节
```

---

### Q: 测试失败如何调试？

**A**: 使用详细模式运行测试：

```bash
# 显示详细输出
python3 test/run_all_tests.py --verbose

# 只测试失败的平台
python3 test/verify_topology.py sm8850 --verbose

# 查看 DSL 文件内容
cat build/sm8850/src/PPTT.dsl
```

---

### Q: 可以在 CI/CD 中使用这些脚本吗？

**A**: 可以！Python 脚本返回标准的退出码：

- `0` = 所有测试通过
- `1` = 有测试失败

#### GitHub Actions 示例

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

#### GitLab CI 示例

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

**A**: 按以下步骤进行：

```bash
# 1. 添加平台配置文件
# include/sm8xxx/pptt.h

# 2. 更新 CMakeLists.txt
# add_platform_target("sm8xxx" "SM8XXX")

# 3. 构建新平台
cd build
cmake ..
make pptt_generator_sm8xxx

# 4. 验证新平台
python3 ../test/verify_topology.py sm8xxx
python3 ../test/aml_validator.py sm8xxx

# 5. 运行完整测试
python3 ../test/run_all_tests.py
```

---

## 添加自定义测试

如果你想添加自己的测试，可以参考现有脚本的结构：

### 示例：自定义验证脚本

创建 `test/my_custom_test.py`：

```python
#!/usr/bin/env python3
"""自定义 PPTT 验证脚本"""

import sys
from pathlib import Path

def validate_file_size(platform):
    """验证文件大小是否在预期范围内"""
    aml_file = Path("build") / platform / "builtin" / "PPTT.aml"
    
    if not aml_file.exists():
        print(f"❌ {platform}: 文件不存在")
        return False
    
    size = aml_file.stat().st_size
    
    # 文件大小应该在 800-1100 bytes 之间
    if not (800 <= size <= 1100):
        print(f"❌ {platform}: 文件大小异常 ({size} bytes)")
        return False
    
    print(f"✅ {platform}: 文件大小正常 ({size} bytes)")
    return True

def main():
    """主函数"""
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
        print("✅ 所有平台验证通过")
        return 0
    else:
        print("❌ 部分平台验证失败")
        return 1

if __name__ == "__main__":
    sys.exit(main())
```

运行自定义测试：

```bash
python3 test/my_custom_test.py
```

---

## 测试最佳实践

### 1. 开发流程中的测试

```bash
# 修改代码后
cd build

# 快速验证（只测试修改的平台）
make pptt_generator_sm8850
python3 ../test/verify_topology.py sm8850

# 完整验证（在提交前）
python3 ../test/run_all_tests.py
```

### 2. 提交前确保所有测试通过

```bash
# 使用 pre-commit hook
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash
cd build
python3 ../test/run_all_tests.py
exit $?
EOF

chmod +x .git/hooks/pre-commit
```

### 3. 调试时使用单独的验证工具

```bash
# 只查看拓扑结构
python3 test/verify_topology.py sm8850

# 只验证 AML 格式
python3 test/aml_validator.py sm8850

# 对比配置与输出
python3 test/pptt_validate.py sm8850

# 详细模式
python3 test/verify_topology.py sm8850 --verbose
```

### 4. 在不同平台上测试

```bash
# Linux/macOS
python3 test/run_all_tests.py

# Windows (PowerShell)
python test\run_all_tests.py

# 使用虚拟环境
python3 -m venv venv
source venv/bin/activate  # Linux/macOS
# 或
.\venv\Scripts\activate   # Windows
python test/run_all_tests.py
```

### 5. 性能测试

```bash
# 测量构建时间
time make

# 测量测试时间
time python3 test/run_all_tests.py

# 批量构建性能
time cmake --build . --parallel
```

---

## 持续集成建议

### 测试矩阵

建议在 CI/CD 中测试多个环境：

| 操作系统 | Python 版本 | CMake 版本 | iasl |
|----------|-------------|------------|------|
| Ubuntu 22.04 | 3.8, 3.10, 3.11 | 3.22+ | ✅ |
| Ubuntu 24.04 | 3.12 | 3.28+ | ✅ |
| macOS 13 | 3.11 | 3.25+ | ✅ |
| Windows Server 2022 | 3.9, 3.11 | 3.24+ | ❌ |

### 测试阶段

1. **编译阶段**：确保所有平台成功编译
2. **验证阶段**：运行 AML 验证和拓扑验证
3. **对比阶段**：对比配置与生成结果
4. **回归测试**：与上一版本对比文件差异

---

## 故障排除

### 测试脚本无法找到 build 目录

```bash
# 检查当前目录
pwd

# 检查 build 目录是否存在
ls -la build/

# 如果不存在，创建并构建
mkdir -p build && cd build
cmake ..
make
```

### Python 版本不兼容

```bash
# 检查 Python 版本
python3 --version

# 使用 pyenv 切换版本
pyenv install 3.11
pyenv local 3.11
```

### iasl 未找到

```bash
# 检查 iasl 是否在 PATH 中
which iasl

# 如果未安装，参考"依赖要求"章节安装
```

---

## 相关资源

- **[README.md](README.md)** - 项目主文档
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - 架构设计文档
- **[Python 官方文档](https://docs.python.org/3/)** - Python 标准库
- **[ACPICA 工具](https://www.acpica.org/)** - iasl 和其他 ACPI 工具

---

**💡 提示**：定期运行测试可以及早发现问题，建议在每次修改代码后都运行完整测试套件。
