# acpi-table-generator

**English** | **[简体中文](README.zh.md)**

---

**Universal ACPI Table Generator** - Multi-purpose ACPI table generator supporting various ACPI table types for complex hardware topologies

## ✨ Features

- **🔧 Multi-Table ACPI Generator** - Supports multiple ACPI table types (PPTT, MADT, and more planned)
- **🎯 PPTT Topology Builder** - Universal CPU topology builder supporting multiple cache architectures
- **🔄 Heterogeneous Support** - Supports mixed configurations of different core types (P-Core/M-Core/E-Core)
- **📐 Flexible Hierarchy** - Supports arbitrary combinations of 2-4 clusters and 2-4 level cache hierarchies
- **✅ Automated Validation** - Integrated iasl disassembly and Python validation tools
- **🚀 CMake Build** - Multi-platform parallel compilation, automatically generates AML and DSL
- **📈 Extensible Architecture** - Modular design for easy addition of new ACPI table types

## 🏗️ Supported Platforms

| Platform | SoC | Architecture | Cores | L2 Type | L3 | File Size |
|----------|-----|--------------|-------|---------|----|----|
| **MTK_MT1234** | MediaTek MT1234 | 2 Clusters (4+4) | 8 | Per-Core | - | 434 bytes |
| **SM8150** | Snapdragon 855 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 2MB | 486 bytes |
| **SM8250** | Snapdragon 865 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 4MB | 486 bytes |
| **SM8350** | Snapdragon 888 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 4MB | 486 bytes |
| **SM8450** | Snapdragon 8 Gen 1 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 6MB | 558 bytes |
| **SM8475** | Snapdragon 8+ Gen 1 | 3 Clusters (4E+3M+1P) | 8 | Per-Core | 8MB | 558 bytes |
| **SM8550** | Snapdragon 8 Gen 2 | 3 Clusters (3E+4M+1P) | 8 | Per-Core | 8MB | 474 bytes |
| **SM8650** | Snapdragon 8 Gen 3 | 4 Clusters (2E+3M+2M+1P) | 8 | Per-Core | 12MB | 494 bytes |
| **SM8750** | Snapdragon 8 Elite Gen 2 | 2 Clusters (6M+2P) | 8 | Per-Cluster | - | 434 bytes |
| **SM8850** | Snapdragon 8 Elite | 2 Clusters (6M+2P) | 8 | Per-Cluster | - | 434 bytes |

## 🚀 Quick Start

### Step 1: Clone

```bash
git clone https://github.com/Project-Aloha/acpi-table-generator.git
cd acpi-table-generator
```

### Step 2: Generate ACPI Tables (PPTT Currently Implemented)

```bash
mkdir build && cd build
cmake ..
make
# View generated AML files
ls -lh qcom_sm8850/PPTT.aml
ls -lh qcom_sm8550/PPTT.aml
ls -lh mtk_mt1234/PPTT.aml
```

### Step 3: Run Tests
Use cross-platform Python test scripts:
```bash
# Complete test suite (recommended)
make test

# Or standalone test like:
python3 ../test/aml_validator.py
```

## 📂 Directory Structure

```
.
├── src/
│   ├── acpi_extractor.c     # ACPI table extraction tool
│   └── dummy/
│       ├── pptt.c           # PPTT table generator (implemented)
│       └── madt.c           # MADT table generator (placeholder - planned)
├── include/
│   ├── common.h             # Common ACPI structure definitions and macros
│   ├── common/
│   │   ├── pptt.h           # PPTT common structure definitions
│   │   └── madt.h           # MADT common structure definitions (planned)
│   └── vendor/
│       ├── mtk/
│       │   └── mt1234/
│       │       └── pptt.h   # Placeholder PPTT header for multi-vendor test
│       └── qcom/
│           └── sm8850/
│               ├── pptt.h   # SM8850 PPTT configuration
│               └── madt.h   # SM8850 MADT configuration
├── build/                   # CMake build directory
│   ├── acpi_extractor       # ACPI table extraction tool
│   ├── lib*_*.a             # Static libraries for each device-table combination
│   ├── qcom_sm8850_pptt     # Executable for SM8850 PPTT generation
│   └── <device>/
│       ├── PPTT.aml         # Generated PPTT binary AML file
│       ├── PPTT.dsl         # PPTT iasl disassembled DSL source
│       └── PPTT_iasl.log    # PPTT iasl execution log
├── test/                    # Test tools (Python + Bash)
│   ├── run_all_tests.py     # Complete test suite (PPTT-focused currently)
│   ├── verify_topology.py   # PPTT topology structure validation
│   ├── aml_validator.py     # AML file validation (supports multiple table types)
│   ├── *_validate.py        # ACPI Table configuration validation
│   └── verify_*.py          # Additional verification scripts
├── CMakeLists.txt           # CMake configuration file
├── README.md                # This file
├── README.zh.md             # Chinese version
└── requirements.txt         # Python dependencies
```

## 🛠️ Adding New Platform

### Method 1: Manual Configuration Creation

#### Step 1: Create Platform Configuration Header

Create new directory under `include/vendor/` and copy template:

```bash
# Create new vendor/platform directory
mkdir -p include/vendor/qcom/sm8xxx

# Copy reference configuration
cp include/vendor/qcom/sm8850/pptt.h include/vendor/qcom/sm8xxx/pptt.h
```

#### Step 2: Modify Platform Configuration

Edit `include/vendor/qcom/sm8xxx/pptt.h` according to actual hardware.

#### Step 3: Rebuild

CMake will automatically detect the new platform:

```bash
cd build
cmake ..
make qcom_sm8xxx_pptt
```

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. **Fork this repository**
2. **Create a feature branch** (`git checkout -b feature/AmazingFeature`)
3. **Commit your changes** (`git commit -m 'Add some AmazingFeature'`)
4. **Push to the branch** (`git push origin feature/AmazingFeature`)
5. **Create a Pull Request**

### Contribution Areas

- ✅ Add support for new platforms
- ✅ Improve universal builder
- ✅ Enhance testing tools
- ✅ Fix bugs
- ✅ Improve documentation

### Code Standards

- - Follow existing code style
- - Add necessary comments
- - Update relevant documentation
- - Ensure all tests pass

## 📄 License

This project is licensed under GPL-3.0 - see LICENSE file for details.

## 📞 Contact

- **Project Homepage**: [https://github.com/Project-Aloha/acpi-table-generator](https://github.com/Project-Aloha/acpi-table-generator)
- **Issue Reports**: [GitHub Issues](https://github.com/Project-Aloha/acpi-table-generator/issues)
- **Discussions**: [GitHub Discussions](https://github.com/Project-Aloha/acpi-table-generator/discussions)

## 🙏 Acknowledgments

- [ACPICA](https://www.acpica.org/) - ACPI Component Architecture
- [EDK2](https://github.com/tianocore/edk2) - UEFI Development Kit
- All contributors and supporters

## 📚 Related Resources

- [ACPI Specification 6.6](https://uefi.org/specs/ACPI/6.6/) - ACPI Specification
- [ARM CPU Architecture](https://developer.arm.com/documentation/) - ARM Architecture Documentation
- [Qualcomm Snapdragon](https://www.qualcomm.com/snapdragon) - Qualcomm Snapdragon Processors

---

**🌟 If this project helps you, please give it a Star!**
