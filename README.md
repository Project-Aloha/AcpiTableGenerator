# ACPI table genetor
This is a tool aim to generate common acpi table aml files for multiple platforms. It also provides tools to gather infomations from android device tree.

---

## 🚀 Quick Start
### Step 1: Clone
```bash
git clone https://github.com/Project-Aloha/acpi-table-generator.git
cd acpi-table-generator
```

### Step 2: Generate ACPI Tables
```bash
mkdir build && cd build
cmake ..
make
# View generated AML files
ls -lh qcom_sm8850/*.aml
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
│       ├── *.c              # dummy C file for a table
├── include/
│   ├── common.h             # Common ACPI structure definitions and macros
│   ├── common/
│   │   ├── *.h              # Common structure definitions for a table
│   └── vendor/
│       ├── mtk/
│       │   └── mt1234/
│       │       └── *.h      # Placeholder
│       └── qcom/
│           └── sm8850/
│               └── *.h      # SM8850 MADT/PPTT/etc configuration
├── build/                   # CMake build directory
│   ├── acpi_extractor       # ACPI table extraction tool
│   ├── iort_reader          # IORT table extraction tool for qcom devcies
│   └── <vendor>/
│       └── <device>/
│           ├── *.aml        # Generated AML file
│           ├── *.dsl        # iasl disassembled DSL source
│           └── *_iasl.log   # iasl execution log
├── test/                    # Test tools (Python + Bash)
│   ├── *.py                 # Complete test suite
├── CMakeLists.txt           # CMake configuration file
├── README.md                # This file
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
cp include/vendor/qcom/sm8850/xxxx.h include/vendor/qcom/sm8xxx/xxxx.h
```

#### Step 2: Modify Platform Configuration

Edit `include/vendor/qcom/sm8xxx/xxxx.h` according to actual hardware.

#### Step 3: Rebuild

CMake will automatically detect the new platform:

```bash
cd build
cmake ..
make qcom_sm8xxx_xxxx
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
This project is licensed under GPL-3.0 - see [LICENSE](LICENSE.md) file for details.

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
- [ARM IORT](  https://developer.arm.com/documentation/den0049/latest) - ARM IO Remapping Table
- [PCI Sig](https://pcisig.com/) - PCI Firmware Spec v3.3
- [Qualcomm Snapdragon](https://www.qualcomm.com/snapdragon) - Qualcomm Snapdragon Processors
- [Microsoft SPCR](https://learn.microsoft.com/en-us/windows-hardware/drivers/bringup/serial-port-console-redirection-table) - Microsoft Serial Port Console Redirection Table
- [Microsoft DBG2](https://learn.microsoft.com/en-us/windows-hardware/drivers/bringup/acpi-debug-port-table) - Microsoft Debug Port Table 2

---
**🌟 If this project helps you, please give it a Star!**
