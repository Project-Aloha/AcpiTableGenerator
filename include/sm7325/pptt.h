#ifndef PPTT_SM7325_H
#define PPTT_SM7325_H

#include <common.h>

// ============================================
// Platform configuration - SM7325
// Auto-generated from device tree
// ============================================

// OEM information
#define PPTT_OEM_ID                     "QCOM  "
#define PPTT_OEM_TABLE_ID               "QCOMEDK2"
#define PPTT_OEM_REVISION               0x7325

// CPU topology configuration
#define NUM_CORES                       8
#define NUM_CLUSTERS                    3
#define CLUSTER0_CORES            4
#define CLUSTER1_CORES            3
#define CLUSTER2_CORES            1

// L1 Data Cache configuration
// Note: The kernel reads L1/L2 parameters from registers. Values below are used only if explicitly provided in the DT; otherwise 0.
#define L1D_SIZE                        0
#define L1D_NUM_SETS                    0
#define L1D_ASSOCIATIVITY               0
#define L1D_LINE_SIZE                   0
#define L1D_ATTRIBUTES                  0
#define CLUSTER0_L1D_SIZE              L1D_SIZE
#define CLUSTER0_L1D_NUM_SETS          L1D_NUM_SETS
#define CLUSTER0_L1D_ASSOCIATIVITY     L1D_ASSOCIATIVITY
#define CLUSTER0_L1D_LINE_SIZE         L1D_LINE_SIZE
#define CLUSTER0_L1D_ATTRIBUTES        L1D_ATTRIBUTES
#define CLUSTER1_L1D_SIZE              L1D_SIZE
#define CLUSTER1_L1D_NUM_SETS          L1D_NUM_SETS
#define CLUSTER1_L1D_ASSOCIATIVITY     L1D_ASSOCIATIVITY
#define CLUSTER1_L1D_LINE_SIZE         L1D_LINE_SIZE
#define CLUSTER1_L1D_ATTRIBUTES        L1D_ATTRIBUTES
#define CLUSTER2_L1D_SIZE              L1D_SIZE
#define CLUSTER2_L1D_NUM_SETS          L1D_NUM_SETS
#define CLUSTER2_L1D_ASSOCIATIVITY     L1D_ASSOCIATIVITY
#define CLUSTER2_L1D_LINE_SIZE         L1D_LINE_SIZE
#define CLUSTER2_L1D_ATTRIBUTES        L1D_ATTRIBUTES

// L1 Instruction Cache configuration
#define L1I_SIZE                        0
#define L1I_NUM_SETS                    0
#define L1I_ASSOCIATIVITY               0
#define L1I_LINE_SIZE                   0
#define L1I_ATTRIBUTES                  0
#define CLUSTER0_L1I_SIZE              L1I_SIZE
#define CLUSTER0_L1I_NUM_SETS          L1I_NUM_SETS
#define CLUSTER0_L1I_ASSOCIATIVITY     L1I_ASSOCIATIVITY
#define CLUSTER0_L1I_LINE_SIZE         L1I_LINE_SIZE
#define CLUSTER0_L1I_ATTRIBUTES        L1I_ATTRIBUTES
#define CLUSTER1_L1I_SIZE              L1I_SIZE
#define CLUSTER1_L1I_NUM_SETS          L1I_NUM_SETS
#define CLUSTER1_L1I_ASSOCIATIVITY     L1I_ASSOCIATIVITY
#define CLUSTER1_L1I_LINE_SIZE         L1I_LINE_SIZE
#define CLUSTER1_L1I_ATTRIBUTES        L1I_ATTRIBUTES
#define CLUSTER2_L1I_SIZE              L1I_SIZE
#define CLUSTER2_L1I_NUM_SETS          L1I_NUM_SETS
#define CLUSTER2_L1I_ASSOCIATIVITY     L1I_ASSOCIATIVITY
#define CLUSTER2_L1I_LINE_SIZE         L1I_LINE_SIZE
#define CLUSTER2_L1I_ATTRIBUTES        L1I_ATTRIBUTES

// L2 Cache configuration
// Per-core L2 detected - each CPU has private L2
#define CORE0_L2_SIZE              0
#define CORE0_L2_NUM_SETS          0
#define CORE0_L2_ASSOCIATIVITY     0
#define CORE0_L2_LINE_SIZE         0
#define CORE0_L2_ATTRIBUTES        0

#define CORE1_L2_SIZE              0
#define CORE1_L2_NUM_SETS          0
#define CORE1_L2_ASSOCIATIVITY     0
#define CORE1_L2_LINE_SIZE         0
#define CORE1_L2_ATTRIBUTES        0

#define CORE2_L2_SIZE              0
#define CORE2_L2_NUM_SETS          0
#define CORE2_L2_ASSOCIATIVITY     0
#define CORE2_L2_LINE_SIZE         0
#define CORE2_L2_ATTRIBUTES        0

#define CORE3_L2_SIZE              0
#define CORE3_L2_NUM_SETS          0
#define CORE3_L2_ASSOCIATIVITY     0
#define CORE3_L2_LINE_SIZE         0
#define CORE3_L2_ATTRIBUTES        0

#define CORE4_L2_SIZE              0
#define CORE4_L2_NUM_SETS          0
#define CORE4_L2_ASSOCIATIVITY     0
#define CORE4_L2_LINE_SIZE         0
#define CORE4_L2_ATTRIBUTES        0

#define CORE5_L2_SIZE              0
#define CORE5_L2_NUM_SETS          0
#define CORE5_L2_ASSOCIATIVITY     0
#define CORE5_L2_LINE_SIZE         0
#define CORE5_L2_ATTRIBUTES        0

#define CORE6_L2_SIZE              0
#define CORE6_L2_NUM_SETS          0
#define CORE6_L2_ASSOCIATIVITY     0
#define CORE6_L2_LINE_SIZE         0
#define CORE6_L2_ATTRIBUTES        0

#define CORE7_L2_SIZE              0
#define CORE7_L2_NUM_SETS          0
#define CORE7_L2_ASSOCIATIVITY     0
#define CORE7_L2_LINE_SIZE         0
#define CORE7_L2_ATTRIBUTES        0

// Default L2 macros (unused when per-core macros present)
#define L2_SIZE                         0
#define L2_NUM_SETS                     0
#define L2_ASSOCIATIVITY                0
#define L2_LINE_SIZE                    0
#define L2_ATTRIBUTES                   0

// L3 Cache configuration (shared)
// Note: L3 cache is shared across all clusters
#define HAS_L3_CACHE                    1
#define L3_SIZE                         0
#define L3_NUM_SETS                     0
#define L3_ASSOCIATIVITY                0
#define L3_LINE_SIZE                    0
#define L3_ATTRIBUTES                   0

// General cache configuration
#define CACHE_LINE_SIZE                 64

#endif // PPTT_SM7325_H
