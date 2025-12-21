#pragma once
#include "table_header.h"
#include <common/pptt.h>
#include <stddef.h>
#include <sys/cdefs.h>

/* Platform specific configuration */
#define NUM_CORES 8
#define NUM_CLUSTERS 2
#define NUM_SYSTEM 1
#define L1_CACHES_COUNT 2
#define L2_CACHES_COUNT 1
#define L3_CACHES_COUNT 0

#define SYSTEM_PRIVATE_RESOURCES_COUNT 1       // ID
#define CLUSTER_PRIVATE_RESOURCES_COUNT 1      // L2 Cache
#define PHYSICAL_CPU_PRIVATE_RESOURCES_COUNT 2 // L1I, L1D

/* Define structures */
PPTT_DEFINE_SYSTEM;
PPTT_DEFINE_CLUSTER;
PPTT_DEFINE_PHYSICAL_CPU;
PPTT_DEFINE_TABLE;
PPTT_DEFINE_WITH_MAGIC;

/* PPTT Stucture */
PPTT_START{
    /* Table Header */
    PPTT_DECLARE_HEADER,

    /* ID */
    PPTT_DECLARE_ID(),

    // System/LLCC
    PPTT_DECLARE_PROCESSOR_HIERARCHY_SYSTEM(0, 0, PPTT_REFERENCE_ID),

    /* Caches */
    // L2 Caches
    PPTT_DECLARE_SIMPLE_CACHE(0, 0), // Shared

    // L1 Caches
    PPTT_DECLARE_SIMPLE_CACHE(1, 0), // Cluster 0
    PPTT_DECLARE_SIMPLE_CACHE(2, 0), // Cluster 0

    /* Processor Hierarchy Nodes */
    // Clusters
    // Cluster 0
    //  - parents
    //    - System
    //  - private resources
    //    - L2 Cache 0
    PPTT_DECLARE_PROCESSOR_HIERARCHY_CLUSTER(0, 0, PPTT_REFERENCE_SYSTEM,
                                             PPTT_REFERENCE_CACHE(0)),
    // Cluster 1
    //  - parents
    //    - System
    //  - private resources
    //    - L2 Cache 1
    PPTT_DECLARE_PROCESSOR_HIERARCHY_CLUSTER(1, 0, PPTT_REFERENCE_SYSTEM,
                                             PPTT_REFERENCE_CACHE(0)),
    // Physical CPUs
    // Cluster 0 CPUs
    //  - parents
    //    - Cluster 0
    //  - private resources
    //    - L1 ICache 0
    //    - L1 DCache 0
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        0, 0, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        1, 1, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        2, 2, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        3, 3, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        4, 4, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        5, 5, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    // Cluster 1 CPUs
    // - parents
    //   - Cluster 1
    // - private resources
    //   - L1 ICache 1
    //   - L1 DCache 1
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        6, 6, PPTT_REFERENCE_CLUSTER(1), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        7, 7, PPTT_REFERENCE_CLUSTER(1), PPTT_REFERENCE_CACHE(1),
        PPTT_REFERENCE_CACHE(2)),
} PPTT_END
