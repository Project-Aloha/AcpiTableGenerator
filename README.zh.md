# acpi-table-generator

**English** | **简体中文**

---

**通用 ACPI 表生成器** - 一个多用途的 ACPI 表生成器，支持多种 ACPI 表类型，用于复杂硬件拓扑

## ✨ 功能

- **🔧 多表类型生成** - 支持多种 ACPI 表类型（PPTT、MADT，后续将增加更多）
- **🎯 PPTT 拓扑构建器** - 支持多种缓存架构的通用 CPU 拓扑构建器
- **🔄 异构支持** - 支持混合的不同核心类型配置（P-Core/M-Core/E-Core）
- **📐 灵活层级** - 支持任意组合的 2-4 个簇和 2-4 级缓存层次结构
- **✅ 自动验证** - 集成 `iasl` 反汇编和 Python 验证工具
- **🚀 CMake 构建** - 跨平台并行编译，自动生成 AML 和 DSL
- **📈 可扩展架构** - 模块化设计，便于添加新的 ACPI 表类型

## 🏗️ 支持平台

| 平台 | SoC | 架构 | 核心数 | L2 类型 | L3 | 文件大小 |
|------|-----|------:|:------:|:------:|:--:|:-------:|
| **MTK_MT1234** | MediaTek MT1234 | 2 簇 (4+4) | 8 | 每核 | - | 434 字节 |
| **SM8150** | Snapdragon 855 | 3 簇 (4E+3M+1P) | 8 | 每核 | 2MB | 486 字节 |
| **SM8250** | Snapdragon 865 | 3 簇 (4E+3M+1P) | 8 | 每核 | 4MB | 486 字节 |
| **SM8350** | Snapdragon 888 | 3 簇 (4E+3M+1P) | 8 | 每核 | 4MB | 486 字节 |
| **SM8450** | Snapdragon 8 Gen 1 | 3 簇 (4E+3M+1P) | 8 | 每核 | 6MB | 558 字节 |
| **SM8475** | Snapdragon 8+ Gen 1 | 3 簇 (4E+3M+1P) | 8 | 每核 | 8MB | 558 字节 |
| **SM8550** | Snapdragon 8 Gen 2 | 3 簇 (3E+4M+1P) | 8 | 每核 | 8MB | 474 字节 |
| **SM8650** | Snapdragon 8 Gen 3 | 4 簇 (2E+3M+2M+1P) | 8 | 每核 | 12MB | 494 字节 |
| **SM8750** | Snapdragon 8 Elite Gen 2 | 2 簇 (6M+2P) | 8 | 每簇 | - | 434 字节 |
| **SM8850** | Snapdragon 8 Elite | 2 簇 (6M+2P) | 8 | 每簇 | - | 434 字节 |

## 🚀 快速开始

### 第 1 步：克隆

```bash
git clone https://github.com/Project-Aloha/acpi-table-generator.git
cd acpi-table-generator
```

### 第 2 步：生成 ACPI 表（当前已实现 PPTT）

```bash
mkdir build && cd build
cmake ..
make
# 查看生成的 AML 文件
ls -lh qcom_sm8850/PPTT.aml
ls -lh qcom_sm8550/PPTT.aml
ls -lh mtk_mt1234/PPTT.aml
```

### 第 3 步：运行测试
使用跨平台的 Python 测试脚本：

```bash
# 运行完整测试套件（推荐）
make test

# 或者单独运行某个测试，例如：
python3 ../test/aml_validator.py
```

## 📂 目录结构

```
.
├── src/
│   ├── acpi_extractor.c     # ACPI 表提取工具
│   └── dummy/
│       ├── pptt.c           # PPTT 表生成器（已实现）
│       └── madt.c           # MADT 表生成器（占位 - 计划中）
├── include/
│   ├── common.h             # 通用 ACPI 结构定义和宏
│   ├── common/
│   │   ├── pptt.h           # PPTT 通用结构定义
│   │   └── madt.h           # MADT 通用结构定义（计划中）
│   └── vendor/
│       ├── mtk/
│       │   └── mt1234/
│       │       └── pptt.h   # 多厂商测试用的 PPTT 占位头文件
│       └── qcom/
│           └── sm8850/
│               ├── pptt.h   # SM8850 PPTT 配置
│               └── madt.h   # SM8850 MADT 配置
├── build/                   # CMake 构建目录
│   ├── acpi_extractor       # ACPI 表提取工具
│   ├── lib*_*.a             # 每个设备-表组合的静态库
│   ├── qcom_sm8850_pptt     # SM8850 PPTT 生成可执行文件
│   └── <device>/
│       ├── PPTT.aml         # 生成的 PPTT 二进制 AML 文件
│       ├── PPTT.dsl         # PPTT iasl 反汇编的 DSL 源
│       └── PPTT_iasl.log    # PPTT iasl 执行日志
├── test/                    # 测试工具（Python + Bash）
│   ├── run_all_tests.py     # 完整测试套件（当前以 PPTT 为主）
│   ├── verify_topology.py   # PPTT 拓扑结构校验
│   ├── aml_validator.py     # AML 文件验证（支持多种表类型）
│   ├── *_validate.py        # ACPI 表配置验证脚本
│   └── verify_*.py          # 其他校验脚本
├── CMakeLists.txt           # CMake 配置文件
├── README.md                # 英文说明
├── README.zh.md             # 本文件（中文）
└── requirements.txt         # Python 依赖
```

## 🛠️ 添加新平台

### 方法 1：手动创建配置

#### 第 1 步：创建平台配置头文件目录

创建 `include/vendor/` 下的新目录并复制模板：

```bash
# 创建新的 vendor/platform 目录
mkdir -p include/vendor/qcom/sm8xxx

# 复制参考配置
cp include/vendor/qcom/sm8850/pptt.h include/vendor/qcom/sm8xxx/pptt.h
```

#### 第 2 步：修改平台配置

编辑 `include/vendor/qcom/sm8xxx/pptt.h`，根据实际硬件修改配置。

#### 第 3 步：重新构建

CMake 会自动检测新平台：

```bash
cd build
cmake ..
make qcom_sm8xxx_pptt
```

## 🤝 贡献

欢迎贡献！请按以下步骤进行：

1. **Fork 本仓库**
2. **创建特性分支** (`git checkout -b feature/AmazingFeature`)
3. **提交更改** (`git commit -m 'Add some AmazingFeature'`)
4. **推送到分支** (`git push origin feature/AmazingFeature`)
5. **创建 Pull Request**

### 可贡献内容

- ✅ 添加新平台支持
- ✅ 改进通用构建器
- ✅ 完善测试工具
- ✅ 修复 Bug
- ✅ 改进文档

### 代码规范

- 遵循现有代码风格
- 添加必要注释
- 更新相关文档
- 确保所有测试通过

## 📄 许可

本项目采用 GPL-3.0 许可 - 详见 LICENSE 文件。

## 📞 联系方式

- **项目主页**: https://github.com/Project-Aloha/acpi-table-generator
- **问题报告**: https://github.com/Project-Aloha/acpi-table-generator/issues
- **讨论**: https://github.com/Project-Aloha/acpi-table-generator/discussions

## 🙏 鸣谢

- [ACPICA](https://www.acpica.org/) - ACPI 组件架构
- [EDK2](https://github.com/tianocore/edk2) - UEFI 开发套件
- 感谢所有贡献者与支持者

## 📚 参考资源

- [ACPI Specification 6.5](https://uefi.org/specs/ACPI/6.6/) - ACPI 规范
- [PPTT Table Specification](https://uefi.org/specs/ACPI/6.6/05_ACPI_Software_Programming_Model.html#processor-properties-topology-table-pptt) - PPTT 表规范
- [ARM CPU Architecture](https://developer.arm.com/documentation/) - ARM 架构文档
- [Qualcomm Snapdragon](https://www.qualcomm.com/snapdragon) - 高通骁龙处理器

---

**🌟 如果该项目对你有帮助，欢迎给个 Star！**
