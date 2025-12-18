#pragma once

#include <common.h>

#define PPTT_OEM_ID                     "QCOM  "
#define PPTT_OEM_TABLE_ID               "QCOMEDK2"
#define PPTT_OEM_REVISION               0x8850

#define NUM_CORES                       8
#define NUM_CLUSTERS                    2
#define CLUSTER0_CORES                  6
#define CLUSTER1_CORES                  2

#define CLUSTER0_L1D_SIZE               SIZE_KB(64)
#define CLUSTER0_L1D_NUM_SETS           256
#define CLUSTER0_L1D_ASSOCIATIVITY      4
#define CLUSTER0_L1D_LINE_SIZE          64
#define CLUSTER0_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

#define CLUSTER0_L1I_SIZE               SIZE_KB(64)
#define CLUSTER0_L1I_NUM_SETS           256
#define CLUSTER0_L1I_ASSOCIATIVITY      4
#define CLUSTER0_L1I_LINE_SIZE          64
#define CLUSTER0_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// Cluster 0 L2 Cache (M-cores shared)
#define CLUSTER0_L2_SIZE                SIZE_MB(12)
#define CLUSTER0_L2_NUM_SETS            12288
#define CLUSTER0_L2_ASSOCIATIVITY       16
#define CLUSTER0_L2_LINE_SIZE           64
#define CLUSTER0_L2_ATTRIBUTES          CACHE_ATTR_UNIFIED_WB

#define CLUSTER1_L1D_SIZE               SIZE_KB(64)
#define CLUSTER1_L1D_NUM_SETS           256
#define CLUSTER1_L1D_ASSOCIATIVITY      4
#define CLUSTER1_L1D_LINE_SIZE          64
#define CLUSTER1_L1D_ATTRIBUTES         CACHE_ATTR_DATA_WB

#define CLUSTER1_L1I_SIZE               SIZE_KB(64)
#define CLUSTER1_L1I_NUM_SETS           256
#define CLUSTER1_L1I_ASSOCIATIVITY      4
#define CLUSTER1_L1I_LINE_SIZE          64
#define CLUSTER1_L1I_ATTRIBUTES         CACHE_ATTR_INSTRUCTION

// Cluster 1 L2 Cache (P-cores shared)
#define CLUSTER1_L2_SIZE                SIZE_MB(12)
#define CLUSTER1_L2_NUM_SETS            12288
#define CLUSTER1_L2_ASSOCIATIVITY       16
#define CLUSTER1_L2_LINE_SIZE           64
#define CLUSTER1_L2_ATTRIBUTES          CACHE_ATTR_UNIFIED_WB

#define CACHE_LINE_SIZE                 64