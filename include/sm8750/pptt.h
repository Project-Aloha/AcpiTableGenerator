#ifndef PPTT_H
#define PPTT_H

#include <common.h>
#include <common/pptt.h>

// ============================================
// 平台配置 - SM8750 (Snapdragon 8 Elite Gen 2)
// ============================================

// OEM 信息
#define PPTT_OEM_ID                     {'Q', 'C', 'O', 'M', ' ', ' '}
#define PPTT_OEM_TABLE_ID               {'Q', 'C', 'O', 'M', 'E', 'D', 'K', '2'}
#define PPTT_OEM_REVISION               0x00008750

// CPU 拓扑配置
#define NUM_CORES                       8
#define NUM_CLUSTERS                    2
#define CLUSTER0_CORES                  6       // M-Core (Oryon)
#define CLUSTER1_CORES                  2       // P-Core (Oryon)

// ============================================
// Cluster 0 (M-Core) - Oryon
// ============================================

// L1 Data Cache
#define CLUSTER0_L1D_SIZE               SIZE_KB(64)
#define CLUSTER0_L1D_NUM_SETS           256
#define CLUSTER0_L1D_ASSOCIATIVITY      4
#define CLUSTER0_L1D_LINE_SIZE          64
#define CLUSTER0_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

// L1 Instruction Cache
#define CLUSTER0_L1I_SIZE               SIZE_KB(64)
#define CLUSTER0_L1I_NUM_SETS           256
#define CLUSTER0_L1I_ASSOCIATIVITY      4
#define CLUSTER0_L1I_LINE_SIZE          64
#define CLUSTER0_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// L2 Cache (6 M-cores shared)
#define CLUSTER0_L2_SIZE                SIZE_MB(12)
#define CLUSTER0_L2_NUM_SETS            16384
#define CLUSTER0_L2_ASSOCIATIVITY       12
#define CLUSTER0_L2_LINE_SIZE           64
#define CLUSTER0_L2_ATTRIBUTES          CACHE_ATTR_UNIFIED_WB

// ============================================
// Cluster 1 (P-Core) - Oryon
// ============================================

// L1 Data Cache
#define CLUSTER1_L1D_SIZE               SIZE_KB(64)
#define CLUSTER1_L1D_NUM_SETS           512
#define CLUSTER1_L1D_ASSOCIATIVITY      2
#define CLUSTER1_L1D_LINE_SIZE          64
#define CLUSTER1_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

// L1 Instruction Cache
#define CLUSTER1_L1I_SIZE               SIZE_KB(128)
#define CLUSTER1_L1I_NUM_SETS           512
#define CLUSTER1_L1I_ASSOCIATIVITY      4
#define CLUSTER1_L1I_LINE_SIZE          64
#define CLUSTER1_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// L2 Cache (2 P-cores shared)
#define CLUSTER1_L2_SIZE                SIZE_MB(12)
#define CLUSTER1_L2_NUM_SETS            16384
#define CLUSTER1_L2_ASSOCIATIVITY       12
#define CLUSTER1_L2_LINE_SIZE           64
#define CLUSTER1_L2_ATTRIBUTES          CACHE_ATTR_UNIFIED_WB

// ============================================
// 架构检测标志
// ============================================

#define HAS_PER_CLUSTER_L2              1

#endif /* PPTT_H */
