#pragma once
#include "table_header.h"
#include <common/pptt.h>
#include <stddef.h>
#include <sys/cdefs.h>

/* Platform specific configuration */
#define NUM_CORES 8
#define NUM_CLUSTERS 3
#define NUM_SYSTEM 1
#define L1_CACHES_COUNT 2
#define L2_CACHES_COUNT 1
#define L3_CACHES_COUNT 1

#define SYSTEM_PRIVATE_RESOURCES_COUNT 2       // ID, L3 Cache
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

    /* Caches */
    // L3 Cache
    PPTT_DECLARE_SIMPLE_CACHE(0, 0), // L3 Shared

    // L2 Caches (per-core)
    PPTT_DECLARE_SIMPLE_CACHE(1, 0), // L2 Cache (per core)

    // L1 Caches
    PPTT_DECLARE_SIMPLE_CACHE(2, PPTT_REFERENCE_CACHE(1)), // L1I
    PPTT_DECLARE_SIMPLE_CACHE(3, PPTT_REFERENCE_CACHE(1)), // L1D

    // System/LLCC
    PPTT_DECLARE_PROCESSOR_HIERARCHY_SYSTEM(0, 0, PPTT_REFERENCE_ID,
                                            PPTT_REFERENCE_CACHE(0)),

    /* Processor Hierarchy Nodes */
    // Clusters
    // Cluster 0 (4 cores)
    //  - parents: System
    //  - private resources: L2 Cache
    PPTT_DECLARE_PROCESSOR_HIERARCHY_CLUSTER(0, 0, PPTT_REFERENCE_SYSTEM),
    // Cluster 1 (3 cores)
    //  - parents: System
    //  - private resources: L2 Cache
    PPTT_DECLARE_PROCESSOR_HIERARCHY_CLUSTER(1, 0, PPTT_REFERENCE_SYSTEM),
    // Cluster 2 (1 core)
    //  - parents: System
    //  - private resources: none
    PPTT_DECLARE_PROCESSOR_HIERARCHY_CLUSTER(2, 0, PPTT_REFERENCE_SYSTEM),

    // Physical CPUs
    // Cluster 0 CPUs (4 cores)
    //  - parents: Cluster 0
    //  - private resources: L1I, L1D, L2
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        0, 0, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        1, 1, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        2, 2, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        3, 3, PPTT_REFERENCE_CLUSTER(0), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),

    // Cluster 1 CPUs (3 cores)
    //  - parents: Cluster 1
    //  - private resources: L1I, L1D, L2
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        4, 4, PPTT_REFERENCE_CLUSTER(1), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        5, 5, PPTT_REFERENCE_CLUSTER(1), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        6, 6, PPTT_REFERENCE_CLUSTER(1), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),

    // Cluster 2 CPU (1 core)
    //  - parents: Cluster 2
    //  - private resources: L1I, L1D, L2
    PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(
        7, 7, PPTT_REFERENCE_CLUSTER(2), PPTT_REFERENCE_CACHE(2),
        PPTT_REFERENCE_CACHE(3)),

} PPTT_END
