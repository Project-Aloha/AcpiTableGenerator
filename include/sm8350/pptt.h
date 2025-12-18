#ifndef PPTT_H
#define PPTT_H

#include <common.h>
#include <common/pptt.h>

// ============================================
// 平台配置 - SM8350 (Snapdragon 888)
// ============================================

// OEM 信息
#define PPTT_OEM_ID                     "QCOM  "
#define PPTT_OEM_TABLE_ID               "QCOMEDK2"
#define PPTT_OEM_REVISION               0x8350

// CPU 拓扑配置
#define NUM_CORES                       8
#define NUM_CLUSTERS                    3
#define CLUSTER0_CORES                  4       // E-Core (Cortex-A55)
#define CLUSTER1_CORES                  3       // M-Core (Cortex-A78)
#define CLUSTER2_CORES                  1       // P-Core (Cortex-X1)

// ============================================
// Cluster 0 (E-Core) - Cortex-A55
// ============================================

// L1 Data Cache
#define CLUSTER0_L1D_SIZE               SIZE_KB(32)
#define CLUSTER0_L1D_NUM_SETS           128
#define CLUSTER0_L1D_ASSOCIATIVITY      4
#define CLUSTER0_L1D_LINE_SIZE          64
#define CLUSTER0_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

// L1 Instruction Cache
#define CLUSTER0_L1I_SIZE               SIZE_KB(32)
#define CLUSTER0_L1I_NUM_SETS           128
#define CLUSTER0_L1I_ASSOCIATIVITY      4
#define CLUSTER0_L1I_LINE_SIZE          64
#define CLUSTER0_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// ============================================
// Cluster 1 (M-Core) - Cortex-A78
// ============================================

// L1 Data Cache
#define CLUSTER1_L1D_SIZE               SIZE_KB(64)
#define CLUSTER1_L1D_NUM_SETS           256
#define CLUSTER1_L1D_ASSOCIATIVITY      4
#define CLUSTER1_L1D_LINE_SIZE          64
#define CLUSTER1_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

// L1 Instruction Cache
#define CLUSTER1_L1I_SIZE               SIZE_KB(64)
#define CLUSTER1_L1I_NUM_SETS           256
#define CLUSTER1_L1I_ASSOCIATIVITY      4
#define CLUSTER1_L1I_LINE_SIZE          64
#define CLUSTER1_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// ============================================
// Cluster 2 (P-Core) - Cortex-X1
// ============================================

// L1 Data Cache
#define CLUSTER2_L1D_SIZE               SIZE_KB(64)
#define CLUSTER2_L1D_NUM_SETS           256
#define CLUSTER2_L1D_ASSOCIATIVITY      4
#define CLUSTER2_L1D_LINE_SIZE          64
#define CLUSTER2_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

// L1 Instruction Cache
#define CLUSTER2_L1I_SIZE               SIZE_KB(64)
#define CLUSTER2_L1I_NUM_SETS           256
#define CLUSTER2_L1I_ASSOCIATIVITY      4
#define CLUSTER2_L1I_LINE_SIZE          64
#define CLUSTER2_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// ============================================
// Per-Core L2 Cache (每个cores独立 L2)
// ============================================

// CPU0 L2 (E-Core)
#define CORE0_L2_SIZE                   SIZE_KB(128)
#define CORE0_L2_NUM_SETS               512
#define CORE0_L2_ASSOCIATIVITY          4
#define CORE0_L2_LINE_SIZE              64
#define CORE0_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU1 L2 (E-Core)
#define CORE1_L2_SIZE                   SIZE_KB(128)
#define CORE1_L2_NUM_SETS               512
#define CORE1_L2_ASSOCIATIVITY          4
#define CORE1_L2_LINE_SIZE              64
#define CORE1_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU2 L2 (E-Core)
#define CORE2_L2_SIZE                   SIZE_KB(128)
#define CORE2_L2_NUM_SETS               512
#define CORE2_L2_ASSOCIATIVITY          4
#define CORE2_L2_LINE_SIZE              64
#define CORE2_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU3 L2 (E-Core)
#define CORE3_L2_SIZE                   SIZE_KB(128)
#define CORE3_L2_NUM_SETS               512
#define CORE3_L2_ASSOCIATIVITY          4
#define CORE3_L2_LINE_SIZE              64
#define CORE3_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU4 L2 (M-Core)
#define CORE4_L2_SIZE                   SIZE_KB(512)
#define CORE4_L2_NUM_SETS               1024
#define CORE4_L2_ASSOCIATIVITY          8
#define CORE4_L2_LINE_SIZE              64
#define CORE4_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU5 L2 (M-Core)
#define CORE5_L2_SIZE                   SIZE_KB(512)
#define CORE5_L2_NUM_SETS               1024
#define CORE5_L2_ASSOCIATIVITY          8
#define CORE5_L2_LINE_SIZE              64
#define CORE5_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU6 L2 (M-Core)
#define CORE6_L2_SIZE                   SIZE_KB(512)
#define CORE6_L2_NUM_SETS               1024
#define CORE6_L2_ASSOCIATIVITY          8
#define CORE6_L2_LINE_SIZE              64
#define CORE6_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU7 L2 (P-Core)
#define CORE7_L2_SIZE                   SIZE_KB(1024)
#define CORE7_L2_NUM_SETS               2048
#define CORE7_L2_ASSOCIATIVITY          8
#define CORE7_L2_LINE_SIZE              64
#define CORE7_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// ============================================
// L3 Cache (Shared)
// ============================================

#define L3_SIZE                         SIZE_MB(4)
#define L3_NUM_SETS                     4096
#define L3_ASSOCIATIVITY                16
#define L3_LINE_SIZE                    64
#define L3_ATTRIBUTES                   CACHE_ATTR_UNIFIED_WB

// ============================================
// 架构检测标志
// ============================================

#define HAS_SHARED_L3                   1

#endif /* PPTT_H */
