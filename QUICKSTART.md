# 快速开始指南

## 5 分钟快速上手

### 第一步：克隆和构建

```bash
git clone <repository>
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

使用跨平台的 Python 测试脚本：

```bash
# 完整测试套件（推荐）
python3 ../test/run_all_tests.py

# 拓扑验证
python3 ../test/verify_topology.py

# AML 文件验证
python3 ../test/aml_validator.py
```

也可以使用 Bash 脚本（仅限 Linux/macOS）：

```bash
../test/run_all_tests.sh
../test/verify_topology.sh
```

---

## 添加新平台（以 SM8650 为例）

### 步骤 1：创建平台配置头文件

创建 `include/sm8650/pptt.h`：

```c
#ifndef SM8650_PPTT_H
#define SM8650_PPTT_H

#include <common.h>
#include <common/pptt.h>

// 基本信息
#define NUM_CORES 8
#define NUM_CLUSTERS 2
#define PPTT_OEM_ID {'Q', 'C', 'O', 'M', ' ', ' '}
#define PPTT_OEM_TABLE_ID {'Q', 'C', 'O', 'M', 'E', 'D', 'K', '2'}
#define PPTT_OEM_REVISION 0x8650

// Cluster 配置
#define CLUSTER0_CORES 6  // E-Core
#define CLUSTER1_CORES 2  // P-Core

// Cluster 0 (E-Core) L1 缓存
#define CLUSTER0_L1D_SIZE SIZE_KB(64)
#define CLUSTER0_L1D_NUM_SETS 256
#define CLUSTER0_L1D_ASSOCIATIVITY 4
#define CLUSTER0_L1D_ATTRIBUTES CACHE_ATTR_DATA_WB
#define CLUSTER0_L1D_LINE_SIZE 64

#define CLUSTER0_L1I_SIZE SIZE_KB(64)
#define CLUSTER0_L1I_NUM_SETS 256
#define CLUSTER0_L1I_ASSOCIATIVITY 4
#define CLUSTER0_L1I_ATTRIBUTES CACHE_ATTR_INSTRUCTION
#define CLUSTER0_L1I_LINE_SIZE 64

// Cluster 1 (P-Core) L1 缓存
#define CLUSTER1_L1D_SIZE SIZE_KB(128)
#define CLUSTER1_L1D_NUM_SETS 512
#define CLUSTER1_L1D_ASSOCIATIVITY 2
#define CLUSTER1_L1D_ATTRIBUTES CACHE_ATTR_DATA_WB
#define CLUSTER1_L1D_LINE_SIZE 64

#define CLUSTER1_L1I_SIZE SIZE_KB(64)
#define CLUSTER1_L1I_NUM_SETS 256
#define CLUSTER1_L1I_ASSOCIATIVITY 4
#define CLUSTER1_L1I_ATTRIBUTES CACHE_ATTR_INSTRUCTION
#define CLUSTER1_L1I_LINE_SIZE 64

// 共享 L2 缓存
#define L2_SIZE SIZE_MB(10)
#define L2_NUM_SETS 16384
#define L2_ASSOCIATIVITY 10
#define L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define L2_LINE_SIZE 64

#endif // SM8650_PPTT_H
```

### 步骤 2：更新 CMakeLists.txt

在 `CMakeLists.txt` 中添加新平台：

```cmake
# 在文件末尾添加
set(PLATFORM_NAME "sm8650")
set(PLATFORM_UPPER "SM8650")

add_executable(pptt_generator_${PLATFORM_NAME}
    src/pptt.c
)

target_include_directories(pptt_generator_${PLATFORM_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/${PLATFORM_NAME}
)

target_compile_definitions(pptt_generator_${PLATFORM_NAME} PRIVATE
    -DPLATFORM_NAME="${PLATFORM_UPPER}"
)

set_target_properties(pptt_generator_${PLATFORM_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${PLATFORM_NAME}/builtin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${PLATFORM_NAME}/src)

add_custom_command(TARGET pptt_generator_${PLATFORM_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "生成 ${PLATFORM_NAME} 的 PPTT.aml 到 ${CMAKE_BINARY_DIR}/${PLATFORM_NAME}/builtin"
    COMMAND ${CMAKE_BINARY_DIR}/pptt_generator_${PLATFORM_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy PPTT.aml ${CMAKE_BINARY_DIR}/${PLATFORM_NAME}/builtin/
    COMMAND ${CMAKE_COMMAND} -E echo "反编译 ${PLATFORM_NAME} 的 PPTT.aml 到 DSL"
    COMMAND iasl -d ${CMAKE_BINARY_DIR}/${PLATFORM_NAME}/builtin/PPTT.aml || true
    COMMAND ${CMAKE_COMMAND} -E copy_if_different PPTT.dsl ${CMAKE_BINARY_DIR}/${PLATFORM_NAME}/src/ || true
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
```

### 步骤 3：编译新平台

```bash
cd build
cmake ..
cmake --build . --target pptt_generator_sm8650
```

### 步骤 4：验证输出

```bash
./pptt_generator_sm8650
cat sm8650/src/PPTT.dsl | head -50
```

---

## 常见问题

### Q: 如何支持 per-cluster L2？

A: 在平台头文件中定义 `CLUSTER*_L2_SIZE` 而不是 `L2_SIZE`：

```c
// 不要定义 L2_SIZE

// 为每个 cluster 定义独立的 L2
#define CLUSTER0_L2_SIZE SIZE_KB(1024)
#define CLUSTER0_L2_NUM_SETS 512
#define CLUSTER0_L2_ASSOCIATIVITY 8
// ...

#define CLUSTER1_L2_SIZE SIZE_KB(512)
#define CLUSTER1_L2_NUM_SETS 256
#define CLUSTER1_L2_ASSOCIATIVITY 8
// ...
```

### Q: 如何添加共享 L3？

A: 只需定义 `L3_SIZE` 等宏：

```c
#define L3_SIZE SIZE_MB(8)
#define L3_NUM_SETS 8192
#define L3_ASSOCIATIVITY 16
#define L3_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define L3_LINE_SIZE 64
```

构建器会自动：
- 在 Package 节点添加 L3 引用
- 将 L2 的 `NextLevelOfCache` 指向 L3
- 将 L1 的 `NextLevelOfCache` 指向 L2（如果有）或 L3

### Q: 支持多少个 Cluster？

A: 当前支持最多 4 个 Cluster。如需更多，修改 `src/pptt.c`：

```c
#define MAX_CLUSTERS 8  // 改为 8
```

并在平台头文件中定义 `CLUSTER3_*`, `CLUSTER4_*` 等。

### Q: 如何验证生成的 PPTT 是否正确？

A: 运行测试脚本：

```bash
cd build
../test/run_all_tests.sh
```

或者手动检查 DSL：

```bash
iasl -d sm8650/builtin/PPTT.aml
cat PPTT.dsl
```

### Q: SIZE_KB 宏未定义怎么办？

A: 确保在平台头文件顶部包含：

```c
#include <common.h>
#include <common/pptt.h>
```

---

## 调试技巧

### 1. 启用详细输出

生成器会自动打印拓扑结构：

```bash
./pptt_generator_sm8850
```

输出示例：
```
PPTT 表结构：
  总大小: 800 bytes
  Package (Physical): 1
  Clusters: 2
    - Cluster 0: 6 cores
      L1D: 64 KB, 4-way
      ...
```

### 2. 检查偏移量

在 `src/pptt.c` 中临时添加调试输出：

```c
static void build_cluster(PpttBuilder *builder, uint32_t cluster_id) {
    offsets.cluster_offsets[cluster_id] = builder->size;
    printf("DEBUG: Cluster %d offset = 0x%X\n", cluster_id, builder->size);
    // ...
}
```

### 3. 对比 DSL 和配置

使用 `grep` 快速验证：

```bash
# 检查 L2 大小是否匹配
grep "Size :" sm8850/src/PPTT.dsl | head -5

# 检查 Cluster 数量
grep "Processor Hierarchy" sm8850/src/PPTT.dsl | wc -l
```

---

## 进阶使用

### 自定义 OEM 信息

修改平台头文件：

```c
#define PPTT_OEM_ID {'M', 'Y', 'O', 'E', 'M', ' '}
#define PPTT_OEM_TABLE_ID {'M', 'Y', 'T', 'A', 'B', 'L', 'E', '0'}
#define PPTT_OEM_REVISION 0x1234
```

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

### 集成到 EDK2

将生成的 `PPTT.aml` 文件复制到 EDK2 项目：

```bash
cp build/sm8850/builtin/PPTT.aml \
   edk2-platforms/Platform/Qualcomm/Sm8850Pkg/AcpiTables/
```

并在 `.inf` 文件中引用。

---

## 获取帮助

- 📖 详细架构文档: [ARCHITECTURE.md](ARCHITECTURE.md)
- 📋 更新日志: [CHANGELOG.md](CHANGELOG.md)
- 🐛 问题报告: GitHub Issues
- 💡 贡献指南: CONTRIBUTING.md
