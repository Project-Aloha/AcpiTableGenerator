#ifndef PPTT_SM8845_H
#define PPTT_SM8845_H

#include <common.h>

// ============================================
// Platform configuration - SM8845
// Auto-generated from device tree
// ============================================

// OEM information
#define PPTT_OEM_ID                     "QCOM  "
#define PPTT_OEM_TABLE_ID               "QCOMEDK2"
#define PPTT_OEM_REVISION               0x8845

// CPU topology configuration
#define NUM_CORES                       8
#define NUM_CLUSTERS                    2
#define CLUSTER0_CORES            6
#define CLUSTER1_CORES            2

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

// L2 Cache configuration
// Per-cluster L2 detected - emitting CLUSTERn_L2_* macros
#define CLUSTER0_L2_SIZE              0
#define CLUSTER0_L2_NUM_SETS          0
#define CLUSTER0_L2_ASSOCIATIVITY     0
#define CLUSTER0_L2_LINE_SIZE         0
#define CLUSTER0_L2_ATTRIBUTES        0

#define CLUSTER1_L2_SIZE              0
#define CLUSTER1_L2_NUM_SETS          0
#define CLUSTER1_L2_ASSOCIATIVITY     0
#define CLUSTER1_L2_LINE_SIZE         0
#define CLUSTER1_L2_ATTRIBUTES        0

// Default L2 macros (unused when per-cluster macros present)
#define L2_SIZE                         0
#define L2_NUM_SETS                     0
#define L2_ASSOCIATIVITY                0
#define L2_LINE_SIZE                    0
#define L2_ATTRIBUTES                   0

// General cache configuration
#define CACHE_LINE_SIZE                 64

#endif // PPTT_SM8845_H
