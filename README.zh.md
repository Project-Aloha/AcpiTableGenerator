# acpi-table-generator

**[English](README.md)** | **简体中文**

---

**通用 ACPI 拓扑构建器** - 使用 C 动态生成支持任意复杂 CPU 拓扑的 ACPI 表

## ✨ 特性

- **🔧 通用拓扑构建器** - 单一代码库支持多种缓存架构（共享 L2、Per-Cluster L2、Per-Core L2、共享 L3）
- **🎯 零硬编码** - 完全由宏定义驱动，自动检测拓扑特性
- **🔄 异构支持** - 支持 P-Core/M-Core/E-Core 等不同类型核心的混合配置
- **📐 灵活层次** - 支持 2-4 cluster 和 2-4 级缓存层次结构的任意组合
- **✅ 自动验证** - 集成 iasl 反编译和 Python 验证工具
- **🚀 CMake 构建** - 多平台并行编译，自动生成 AML 和 DSL

## 📖 文档导航

- **[测试指南 (TESTING.md)](TESTING.md)** - 完整的测试工具和验证方法
- **[架构设计 (ARCHITECTURE.md)](ARCHITECTURE.md)** - 通用拓扑构建器的设计细节

## 🏗️ 支持的平台

| 平台 | SoC | 架构 | 核心数 | L2 类型 | L3 | 文件大小 |
|------|-----|------|--------|---------|----|----|
| **SM8150** | Snapdragon 855 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 2MB | 1044 bytes |
| **SM8250** | Snapdragon 865 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 4MB | 1044 bytes |
| **SM8350** | Snapdragon 888 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 4MB | 1044 bytes |
| **SM8450** | Snapdragon 8 Gen 1 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 6MB | 1044 bytes |
| **SM8475** | Snapdragon 8+ Gen 1 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 8MB | 1044 bytes |
| **SM8550** | Snapdragon 8 Gen 2 | 3 Clusters (3E+4M+1P) | 8 | Per-Core | 8MB | 1044 bytes |
| **SM8650** | Snapdragon 8 Gen 3 | 4 Clusters (2E+3M+2M+1P) | 8 | Per-Core | 12MB | 1064 bytes |
| **SM8750** | Snapdragon 8 Elite Gen 2 | 2 Clusters (6M+2P) | 8 | Per-Cluster | - | 832 bytes |
| **SM8850** | Snapdragon 8 Elite | 2 Clusters (6M+2P) | 8 | Per-Cluster | - | 832 bytes |

### L2 缓存架构说明

- **Per-Core L2**: 每个 CPU 核心有独立的 L2 缓存（可通过相同大小/属性实现 2-way 共享）
- **Per-Cluster L2**: 每个 Cluster 的所有核心共享一个 L2 缓存
- **Shared L2**: 所有核心共享单个 L2 缓存

## 🚀 快速开始（5 分钟上手）

### 第一步：克隆和构建

```bash
git clone https://github.com/Project-Aloha/acpi-table-generator.git
cd acpi-table-generator
mkdir build && cd build
cmake ..
make
```

### 第二步：生成 PPTT 表

```bash
# 生成 SM8850 的 PPTT
./pptt_generator_sm8850

# 生成 SM8550 的 PPTT
./pptt_generator_sm8550

# 查看生成的文件
ls -lh sm8850/builtin/PPTT.aml
ls -lh sm8550/builtin/PPTT.aml
```

### 第三步：查看反编译结果

```bash
# 查看 SM8850 拓扑
cat sm8850/src/PPTT.dsl | head -100

# 查看 SM8550 拓扑
cat sm8550/src/PPTT.dsl | head -100
```

### 第四步：运行测试

使用跨平台的 Python 测试脚本（详见 [TESTING.md](TESTING.md)）：

```bash
# 完整测试套件（推荐）
python3 ../test/run_all_tests.py

# 拓扑验证
python3 ../test/verify_topology.py

# AML 文件验证
python3 ../test/aml_validator.py
```

## 📂 目录结构

```
.
├── src/
│   ├── pptt.c              # PPTT 生成器主程序（通用拓扑构建器）
│   └── madt.c              # MADT 生成器（待实现）
├── include/
│   ├── common.h            # 通用 ACPI 结构定义和宏
│   ├── common/
│   │   └── pptt.h          # PPTT 通用结构定义
│   ├── sm8150/pptt.h       # SM8150 平台配置
│   ├── sm8250/pptt.h       # SM8250 平台配置
│   ├── sm8350/pptt.h       # SM8350 平台配置
│   ├── sm8450/pptt.h       # SM8450 平台配置
│   ├── sm8475/pptt.h       # SM8475 平台配置
│   ├── sm8550/pptt.h       # SM8550 平台配置
│   ├── sm8650/pptt.h       # SM8650 平台配置
│   ├── sm8750/pptt.h       # SM8750 平台配置
│   └── sm8850/pptt.h       # SM8850 平台配置
├── build/                  # CMake 构建目录
│   ├── pptt_generator_*    # 各平台生成器可执行文件
│   └── <platform>/
│       ├── builtin/
│       │   └── PPTT.aml    # 生成的二进制 AML 文件
│       └── src/
│           └── PPTT.dsl    # iasl 反编译的 DSL 源码
├── test/                   # 测试工具（Python + Bash）
│   ├── run_all_tests.py    # 完整测试套件
│   ├── verify_topology.py  # 拓扑结构验证
│   ├── aml_validator.py    # AML 文件验证
│   └── pptt_validate.py    # PPTT 配置验证
├── tools/
│   └── parse_dt.py         # 设备树解析工具
├── CMakeLists.txt          # CMake 配置文件
├── Makefile                # 传统 Makefile（已废弃）
├── README.md               # 本文件
├── TESTING.md              # 测试指南
└── requirements.txt        # Python 依赖
```

## 🛠️ 添加新平台

### 方法 1: 手动创建配置文件

#### 步骤 1：创建平台配置头文件

在 `include/` 下创建新目录并复制模板：

```bash
# 创建新平台目录
mkdir include/sm8xxx

# 复制参考配置
cp include/sm8850/pptt.h include/sm8xxx/pptt.h
```

#### 步骤 2：修改平台配置

编辑 `include/sm8xxx/pptt.h`，根据实际硬件修改：

```c
#pragma once

#include <common.h>

// OEM 信息
#define PPTT_OEM_ID                     "QCOM  "
#define PPTT_OEM_TABLE_ID               "QCOMEDK2"
#define PPTT_OEM_REVISION               0x8xxx

// 基本拓扑
#define NUM_CORES                       8
#define NUM_CLUSTERS                    2
#define CLUSTER0_CORES                  6  // E-Core
#define CLUSTER1_CORES                  2  // P-Core

// Cluster 0 L1 缓存配置
#define CLUSTER0_L1D_SIZE               SIZE_KB(64)
#define CLUSTER0_L1D_NUM_SETS           256
#define CLUSTER0_L1D_ASSOCIATIVITY      4
#define CLUSTER0_L1D_LINE_SIZE          64
#define CLUSTER0_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

#define CLUSTER0_L1I_SIZE               SIZE_KB(64)
#define CLUSTER0_L1I_NUM_SETS           256
#define CLUSTER0_L1I_ASSOCIATIVITY      4
#define CLUSTER0_L1I_LINE_SIZE          64
#define CLUSTER0_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// L2 缓存配置（选择其中一种）

// 选项 A: Per-Cluster L2
#define CLUSTER0_L2_SIZE                SIZE_MB(12)
#define CLUSTER0_L2_NUM_SETS            12288
#define CLUSTER0_L2_ASSOCIATIVITY       16
#define CLUSTER0_L2_LINE_SIZE           64
#define CLUSTER0_L2_ATTRIBUTES          CACHE_ATTR_UNIFIED_WB

// 选项 B: Per-Core L2（为每个 core 定义）
// #define CORE0_L2_SIZE                SIZE_KB(128)
// #define CORE0_L2_NUM_SETS            128
// ...

// 选项 C: Shared L2（所有核心共享）
// #define L2_SIZE                      SIZE_MB(12)
// #define L2_NUM_SETS                  12288
// ...

// L3 缓存配置（可选）
#define L3_SIZE                         SIZE_MB(8)
#define L3_NUM_SETS                     4096
#define L3_ASSOCIATIVITY                16
#define L3_LINE_SIZE                    64
#define L3_ATTRIBUTES                   CACHE_ATTR_UNIFIED_WB

#define CACHE_LINE_SIZE                 64
```

#### 步骤 3：更新 CMakeLists.txt

在 `CMakeLists.txt` 中添加新平台目标：

```cmake
# 在文件末尾添加
add_platform_target("sm8xxx" "SM8XXX")
```

#### 步骤 4：编译和验证

```bash
cd build
cmake ..
make pptt_generator_sm8xxx

# 查看生成结果
./pptt_generator_sm8xxx
cat sm8xxx/src/PPTT.dsl
```

### 方法 2: 从设备树自动生成（推荐）

使用 `parse_dt.py` 工具自动从设备树提取拓扑信息：

```bash
# 安装依赖
pip install -r requirements.txt

# 从 DTB 文件生成头文件
python3 tools/parse_dt.py <platform>.dtb -p <platform_name>

# 示例：从 sm8xxx.dtb 生成配置
python3 tools/parse_dt.py sm8xxx.dtb -p sm8xxx

# 查看设备树信息（不生成文件）
python3 tools/parse_dt.py sm8xxx.dtb --info
```

工具会自动：
1. 解析 DTB 中的 CPU 拓扑（cpus 节点和 cpu-map）
2. 提取 cluster 和 core 信息
3. 检测 L2/L3 缓存层次结构
4. 生成头文件到 `include/<platform>/pptt.h`

## 🎯 L2 缓存架构选择指南

通用构建器支持三种 L2 缓存架构，通过宏定义自动检测：

### 1. Per-Core L2（每核独立 L2）

每个 CPU 核心有独立的 L2 缓存。适用于 SM8150-SM8650。

**配置方式**：为每个 core 定义独立的 L2 参数

```c
#define CORE0_L2_SIZE               SIZE_KB(128)
#define CORE0_L2_NUM_SETS           128
#define CORE0_L2_ASSOCIATIVITY      8
#define CORE0_L2_LINE_SIZE          64
#define CORE0_L2_ATTRIBUTES         CACHE_ATTR_UNIFIED_WB

#define CORE1_L2_SIZE               SIZE_KB(128)
// ... 为所有 core 定义
```

**共享支持**：如果多个 core 共享 L2，使用相同的大小和属性：

```c
// CPU0 和 CPU1 共享 128KB L2
#define CORE0_L2_SIZE               SIZE_KB(128)
#define CORE1_L2_SIZE               SIZE_KB(128)  // 相同大小表示共享
```

### 2. Per-Cluster L2（每簇共享 L2）

每个 Cluster 的所有核心共享一个 L2 缓存。适用于 SM8750、SM8850。

**配置方式**：为每个 cluster 定义 L2 参数

```c
#define CLUSTER0_L2_SIZE            SIZE_MB(12)
#define CLUSTER0_L2_NUM_SETS        12288
#define CLUSTER0_L2_ASSOCIATIVITY   16
#define CLUSTER0_L2_LINE_SIZE       64
#define CLUSTER0_L2_ATTRIBUTES      CACHE_ATTR_UNIFIED_WB

#define CLUSTER1_L2_SIZE            SIZE_MB(12)
// ... 为所有 cluster 定义
```

### 3. Shared L2（全局共享 L2）

所有核心共享单个 L2 缓存。

**配置方式**：定义单个 L2 参数

```c
#define L2_SIZE                     SIZE_MB(12)
#define L2_NUM_SETS                 12288
#define L2_ASSOCIATIVITY            12
#define L2_LINE_SIZE                64
#define L2_ATTRIBUTES               CACHE_ATTR_UNIFIED_WB
```

**自动检测逻辑**：

```c
// 构建器按以下优先级检测：
if (defined CORE0_L2_SIZE)         → Per-Core L2
else if (defined CLUSTER0_L2_SIZE) → Per-Cluster L2
else if (defined L2_SIZE)          → Shared L2
```

## 🔧 修改现有平台配置

### 修改缓存大小

编辑 `include/<platform>/pptt.h`：

```c
// 修改 L1D 大小
#define CLUSTER0_L1D_SIZE           SIZE_KB(128)  // 从 64KB 改为 128KB

// 修改 L2 大小
#define CLUSTER0_L2_SIZE            SIZE_MB(16)   // 从 12MB 改为 16MB

// 修改关联度
#define CLUSTER0_L2_ASSOCIATIVITY   20            // 从 16-way 改为 20-way
```

### 修改核心数量

```c
#define NUM_CORES                   12            // 总核心数
#define NUM_CLUSTERS                3             // 簇数量
#define CLUSTER0_CORES              6
#define CLUSTER1_CORES              4
#define CLUSTER2_CORES              2
```

### 添加/删除 L3 缓存

```c
// 添加 L3：定义 L3 参数
#define L3_SIZE                     SIZE_MB(8)
#define L3_NUM_SETS                 4096
#define L3_ASSOCIATIVITY            16
#define L3_LINE_SIZE                64
#define L3_ATTRIBUTES               CACHE_ATTR_UNIFIED_WB

// 删除 L3：注释掉所有 L3_* 定义
// #define L3_SIZE                  ...
```

### 修改 OEM 信息

```c
#define PPTT_OEM_ID                 "MYOEM "
#define PPTT_OEM_TABLE_ID           "MYTABLE0"
#define PPTT_OEM_REVISION           0x1234
```

## 📤 输出文件

### 二进制 AML 文件（`builtin/PPTT.aml`）

标准的 ACPI 二进制表格式，可以：
- 集成到 UEFI 固件或 ACPI 表集合中
- 通过 bootloader 加载到系统
- 用于实际设备的 ACPI 表部署

**位置**：`build/<platform>/builtin/PPTT.aml`

### DSL 源码文件（`src/PPTT.dsl`）

如果系统中安装了 iasl 工具，构建过程会自动反编译 AML 文件为 DSL 源码：
- 人类可读的 ACPI 表描述
- 方便验证生成的表结构
- 可作为参考文档

**位置**：`build/<platform>/src/PPTT.dsl`

**依赖**：需要安装 ACPICA 工具包
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

## 🧪 测试和验证

完整的测试指南请参考 **[TESTING.md](TESTING.md)**。

### 快速测试

```bash
# 在 build 目录中
cd build

# 运行完整测试套件
python3 ../test/run_all_tests.py

# 验证拓扑结构
python3 ../test/verify_topology.py

# 验证 AML 文件格式
python3 ../test/aml_validator.py
```

### 主要测试工具

| 工具 | 功能 | 文档链接 |
|------|------|---------|
| `run_all_tests.py` | 完整测试套件：编译、生成、验证 | [TESTING.md#完整测试套件](TESTING.md) |
| `verify_topology.py` | 验证和可视化 PPTT 拓扑结构 | [TESTING.md#拓扑验证工具](TESTING.md) |
| `aml_validator.py` | 验证 AML 文件是否符合 ACPI 规范 | [TESTING.md#aml-验证工具](TESTING.md) |
| `pptt_validate.py` | 对比配置与 DSL 输出一致性 | [TESTING.md](TESTING.md) |

## 🔍 调试和故障排除

### 启用详细输出

生成器会自动打印拓扑结构：

```bash
./pptt_generator_sm8850
```

输出示例：
```
PPTT 表结构：
  总大小: 832 bytes
  Package (Physical): 1
  Clusters: 2
    - Cluster 0: 6 cores
      L1D: 64 KB, 4-way
      L1I: 64 KB, 4-way
      L2: 12288 KB, 16-way
    - Cluster 1: 2 cores
      L1D: 64 KB, 4-way
      L1I: 64 KB, 4-way
      L2: 12288 KB, 16-way
```

### 检查缓存配置

使用 `grep` 快速验证 DSL 输出：

```bash
# 检查 L2 大小
grep "Size :" build/sm8850/src/PPTT.dsl

# 检查 Cluster 数量
grep "Processor Hierarchy" build/sm8850/src/PPTT.dsl | wc -l

# 检查缓存关联度
grep "Associativity :" build/sm8850/src/PPTT.dsl
```

### 常见问题

#### Q: 编译时提示 `SIZE_KB` 宏未定义？

A: 确保在平台头文件顶部包含：

```c
#include <common.h>
```

#### Q: 如何支持 4 个以上的 Cluster？

A: 修改 `src/pptt.c` 中的 `MAX_CLUSTERS` 定义：

```c
#define MAX_CLUSTERS 8  // 改为 8
```

并在平台头文件中定义 `CLUSTER3_*`, `CLUSTER4_*` 等。

#### Q: 生成的文件大小与预期不符？

A: 检查：
1. 是否定义了不必要的缓存层级
2. 核心数量是否正确
3. 运行 `verify_topology.py` 查看实际结构

#### Q: DSL 文件未生成？

A: 检查是否安装了 `iasl`：

```bash
iasl -v
# 如未安装，参考上文"输出文件"一节
```

## 🚀 进阶使用

### 批量构建所有平台

```bash
cd build

# Linux/macOS
cmake --build . --parallel $(nproc)

# Windows (PowerShell)
cmake --build . --parallel $env:NUMBER_OF_PROCESSORS

# 跨平台（自动检测）
cmake --build . --parallel
```

### 集成到 EDK2/UEFI 固件

将生成的 `PPTT.aml` 文件复制到 EDK2 项目：

```bash
# 示例：集成 SM8850 PPTT 到 EDK2
cp build/sm8850/builtin/PPTT.aml \
   edk2-platforms/Platform/Qualcomm/Sm8850Pkg/AcpiTables/

# 在 .inf 文件中引用
# [Sources]
#   AcpiTables/PPTT.aml
```

### 自定义 OEM 信息

编辑平台头文件 `include/<platform>/pptt.h`：

```c
#define PPTT_OEM_ID                 "MYOEM "      // 6 字符
#define PPTT_OEM_TABLE_ID           "MYTABLE0"   // 8 字符
#define PPTT_OEM_REVISION           0x1234       // 自定义版本号
```

重新编译后，OEM 信息会更新到 PPTT 表头。

## 🤝 贡献指南

欢迎贡献！请遵循以下步骤：

1. **Fork 本仓库**
2. **创建特性分支** (`git checkout -b feature/AmazingFeature`)
3. **提交更改** (`git commit -m 'Add some AmazingFeature'`)
4. **推送到分支** (`git push origin feature/AmazingFeature`)
5. **创建 Pull Request**

### 贡献内容

- ✅ 添加新平台支持
- ✅ 改进通用构建器
- ✅ 完善测试工具
- ✅ 修复 Bug
- ✅ 改进文档

### 代码规范

- 遵循现有代码风格
- 添加必要的注释
- 更新相关文档
- 确保所有测试通过

## 📄 许可证

本项目采用 GPL-3.0 许可证 - 详见 LICENSE 文件。

## 📞 联系方式

- **项目主页**: [https://github.com/Project-Aloha/acpi-table-generator](https://github.com/Project-Aloha/acpi-table-generator)
- **问题反馈**: [GitHub Issues](https://github.com/Project-Aloha/acpi-table-generator/issues)
- **讨论交流**: [GitHub Discussions](https://github.com/Project-Aloha/acpi-table-generator/discussions)

## 🙏 致谢

- [ACPICA](https://www.acpica.org/) - ACPI 组件架构
- [EDK2](https://github.com/tianocore/edk2) - UEFI 开发套件
- [Linux Kernel Device Tree](https://www.devicetree.org/) - 设备树规范
- 所有贡献者和支持者

## 📚 相关资源

- [ACPI Specification 6.5](https://uefi.org/specs/ACPI/6.5/) - ACPI 规范
- [PPTT Table Specification](https://uefi.org/specs/ACPI/6.5/05_ACPI_Software_Programming_Model.html#processor-properties-topology-table-pptt) - PPTT 表规范
- [ARM CPU Architecture](https://developer.arm.com/documentation/) - ARM 架构文档
- [Qualcomm Snapdragon](https://www.qualcomm.com/snapdragon) - 高通骁龙处理器

---

**🌟 如果本项目对你有帮助，请给个 Star！**
