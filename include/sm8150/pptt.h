#ifndef SM8150_PPTT_H
#define SM8150_PPTT_H

#include <common.h>
#include <common/pptt.h>

// Platform: Snapdragon 855 (SM8150)
// Architecture: 3 Clusters with per-core L2 + shared L3

// Basic Information
#define NUM_CORES 8
#define NUM_CLUSTERS 3
#define HAS_PER_CORE_L2

// ACPI OEM Information
#define PPTT_OEM_ID {'Q', 'C', 'O', 'M', ' ', ' '}
#define PPTT_OEM_TABLE_ID {'Q', 'C', 'O', 'M', 'E', 'D', 'K', '2'}
#define PPTT_OEM_REVISION 0x00008150

// ============================================================================
// Cluster Configuration
// ============================================================================

// Cluster 0: 4 × E-Cores (Efficiency Cores - Cortex-A55)
#define CLUSTER0_CORES 4

// Cluster 1: 3 × M-Cores (Middle Cores - Cortex-A76)
#define CLUSTER1_CORES 3

// Cluster 2: 1 × P-Core (Prime Core - Cortex-A76)
#define CLUSTER2_CORES 1

// ============================================================================
// Cluster 0 Cache Configuration (E-Cores)
// ============================================================================

// L1 Data Cache
#define CLUSTER0_L1D_SIZE SIZE_KB(32)
#define CLUSTER0_L1D_NUM_SETS 128           // 32KB / 64B / 4-way
#define CLUSTER0_L1D_ASSOCIATIVITY 4
#define CLUSTER0_L1D_ATTRIBUTES CACHE_ATTR_DATA_WB
#define CLUSTER0_L1D_LINE_SIZE 64

// L1 Instruction Cache
#define CLUSTER0_L1I_SIZE SIZE_KB(32)
#define CLUSTER0_L1I_NUM_SETS 128           // 32KB / 64B / 4-way
#define CLUSTER0_L1I_ASSOCIATIVITY 4
#define CLUSTER0_L1I_ATTRIBUTES CACHE_ATTR_INSTRUCTION
#define CLUSTER0_L1I_LINE_SIZE 64

// ============================================================================
// Cluster 1 Cache Configuration (M-Cores)
// ============================================================================

// L1 Data Cache
#define CLUSTER1_L1D_SIZE SIZE_KB(128)
#define CLUSTER1_L1D_NUM_SETS 512           // 128KB / 64B / 4-way
#define CLUSTER1_L1D_ASSOCIATIVITY 4
#define CLUSTER1_L1D_ATTRIBUTES CACHE_ATTR_DATA_WB
#define CLUSTER1_L1D_LINE_SIZE 64

// L1 Instruction Cache
#define CLUSTER1_L1I_SIZE SIZE_KB(128)
#define CLUSTER1_L1I_NUM_SETS 512           // 128KB / 64B / 4-way
#define CLUSTER1_L1I_ASSOCIATIVITY 4
#define CLUSTER1_L1I_ATTRIBUTES CACHE_ATTR_INSTRUCTION
#define CLUSTER1_L1I_LINE_SIZE 64

// ============================================================================
// Cluster 2 Cache Configuration (P-Core)
// ============================================================================

// L1 Data Cache
#define CLUSTER2_L1D_SIZE SIZE_KB(128)
#define CLUSTER2_L1D_NUM_SETS 512           // 128KB / 64B / 4-way
#define CLUSTER2_L1D_ASSOCIATIVITY 4
#define CLUSTER2_L1D_ATTRIBUTES CACHE_ATTR_DATA_WB
#define CLUSTER2_L1D_LINE_SIZE 64

// L1 Instruction Cache
#define CLUSTER2_L1I_SIZE SIZE_KB(128)
#define CLUSTER2_L1I_NUM_SETS 512           // 128KB / 64B / 4-way
#define CLUSTER2_L1I_ASSOCIATIVITY 4
#define CLUSTER2_L1I_ATTRIBUTES CACHE_ATTR_INSTRUCTION
#define CLUSTER2_L1I_LINE_SIZE 64

// ============================================================================
// Per-Core L2 Cache Configuration
// ============================================================================

// CPU0-3: E-Cores L2 (128KB each)
#define CORE0_L2_SIZE SIZE_KB(128)
#define CORE0_L2_NUM_SETS 512               // 128KB / 64B / 4-way
#define CORE0_L2_ASSOCIATIVITY 4
#define CORE0_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE0_L2_LINE_SIZE 64

#define CORE1_L2_SIZE SIZE_KB(128)
#define CORE1_L2_NUM_SETS 512
#define CORE1_L2_ASSOCIATIVITY 4
#define CORE1_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE1_L2_LINE_SIZE 64

#define CORE2_L2_SIZE SIZE_KB(128)
#define CORE2_L2_NUM_SETS 512
#define CORE2_L2_ASSOCIATIVITY 4
#define CORE2_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE2_L2_LINE_SIZE 64

#define CORE3_L2_SIZE SIZE_KB(128)
#define CORE3_L2_NUM_SETS 512
#define CORE3_L2_ASSOCIATIVITY 4
#define CORE3_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE3_L2_LINE_SIZE 64

// CPU4-6: M-Cores L2 (256KB each)
#define CORE4_L2_SIZE SIZE_KB(256)
#define CORE4_L2_NUM_SETS 1024              // 256KB / 64B / 4-way
#define CORE4_L2_ASSOCIATIVITY 4
#define CORE4_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE4_L2_LINE_SIZE 64

#define CORE5_L2_SIZE SIZE_KB(256)
#define CORE5_L2_NUM_SETS 1024
#define CORE5_L2_ASSOCIATIVITY 4
#define CORE5_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE5_L2_LINE_SIZE 64

#define CORE6_L2_SIZE SIZE_KB(256)
#define CORE6_L2_NUM_SETS 1024
#define CORE6_L2_ASSOCIATIVITY 4
#define CORE6_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE6_L2_LINE_SIZE 64

// CPU7: P-Core L2 (512KB)
#define CORE7_L2_SIZE SIZE_KB(512)
#define CORE7_L2_NUM_SETS 2048              // 512KB / 64B / 4-way
#define CORE7_L2_ASSOCIATIVITY 4
#define CORE7_L2_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define CORE7_L2_LINE_SIZE 64

// ============================================================================
// L3 Cache Configuration (Shared by all clusters)
// ============================================================================

#define L3_SIZE SIZE_MB(2)
#define L3_NUM_SETS 2048                    // 2MB / 64B / 16-way
#define L3_ASSOCIATIVITY 16
#define L3_ATTRIBUTES CACHE_ATTR_UNIFIED_WB
#define L3_LINE_SIZE 64

#endif // SM8150_PPTT_H
