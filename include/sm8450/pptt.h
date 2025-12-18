#ifndef PPTT_H
#define PPTT_H

#include <common.h>
#include <common/pptt.h>

// ============================================
// 平台配置 - SM8450 (Snapdragon 8 Gen 1)
// ============================================

// OEM 信息
#define PPTT_OEM_ID                     {'Q', 'C', 'O', 'M', ' ', ' '}
#define PPTT_OEM_TABLE_ID               {'Q', 'C', 'O', 'M', 'E', 'D', 'K', '2'}
#define PPTT_OEM_REVISION               0x00008450

// CPU 拓扑配置（根据设备树 cpu-map 顺序）
#define NUM_CORES                       8
#define NUM_CLUSTERS                    3
#define CLUSTER0_CORES                  4       // E-Core (Cortex-A510)
#define CLUSTER1_CORES                  3       // M-Core (Cortex-A710)
#define CLUSTER2_CORES                  1       // P-Core (Cortex-X2)

// ============================================
// Cluster 0 (E-Core) - Cortex-A510
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
// Cluster 1 (M-Core) - Cortex-A710
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
// Cluster 2 (P-Core) - Cortex-X2
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
// Per-Core L2 配置（根据设备树 L2 节点）
// ============================================

// CPU0 (E-core): L2_0 与 CPU1 shared
#define CORE0_L2_SIZE                   SIZE_KB(128)
#define CORE0_L2_NUM_SETS               512
#define CORE0_L2_ASSOCIATIVITY          4
#define CORE0_L2_LINE_SIZE              64
#define CORE0_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU1 (E-core): shared CPU0 的 L2_0
#define CORE1_L2_SIZE                   SIZE_KB(128)
#define CORE1_L2_NUM_SETS               512
#define CORE1_L2_ASSOCIATIVITY          4
#define CORE1_L2_LINE_SIZE              64
#define CORE1_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU2 (E-core): L2_2 与 CPU3 shared
#define CORE2_L2_SIZE                   SIZE_KB(128)
#define CORE2_L2_NUM_SETS               512
#define CORE2_L2_ASSOCIATIVITY          4
#define CORE2_L2_LINE_SIZE              64
#define CORE2_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU3 (E-core): shared CPU2 的 L2_2
#define CORE3_L2_SIZE                   SIZE_KB(128)
#define CORE3_L2_NUM_SETS               512
#define CORE3_L2_ASSOCIATIVITY          4
#define CORE3_L2_LINE_SIZE              64
#define CORE3_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU4 (M-core): L2_4 独立
#define CORE4_L2_SIZE                   SIZE_KB(512)
#define CORE4_L2_NUM_SETS               1024
#define CORE4_L2_ASSOCIATIVITY          8
#define CORE4_L2_LINE_SIZE              64
#define CORE4_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU5 (M-core): L2_5 独立
#define CORE5_L2_SIZE                   SIZE_KB(512)
#define CORE5_L2_NUM_SETS               1024
#define CORE5_L2_ASSOCIATIVITY          8
#define CORE5_L2_LINE_SIZE              64
#define CORE5_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU6 (M-core): L2_6 独立
#define CORE6_L2_SIZE                   SIZE_KB(512)
#define CORE6_L2_NUM_SETS               1024
#define CORE6_L2_ASSOCIATIVITY          8
#define CORE6_L2_LINE_SIZE              64
#define CORE6_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// CPU7 (P-core): L2_7 独立
#define CORE7_L2_SIZE                   SIZE_KB(1024)
#define CORE7_L2_NUM_SETS               2048
#define CORE7_L2_ASSOCIATIVITY          8
#define CORE7_L2_LINE_SIZE              64
#define CORE7_L2_ATTRIBUTES             CACHE_ATTR_UNIFIED_WB

// ============================================
// L3 Cache (Shared)
// ============================================

#define L3_SIZE                         SIZE_MB(6)
#define L3_NUM_SETS                     4096
#define L3_ASSOCIATIVITY                12
#define L3_LINE_SIZE                    64
#define L3_ATTRIBUTES                   CACHE_ATTR_UNIFIED_WB

#endif /* PPTT_H */
