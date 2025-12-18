#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <common/pptt.h>
#include <pptt.h>

#define MAX_CORES 16
#define MAX_CLUSTERS 4

typedef struct {
    uint8_t *buffer;
    size_t size;
    size_t capacity;
} PpttBuilder;

typedef struct {
    uint32_t package_offset;
    uint32_t l2_shared_offset;
    uint32_t l3_shared_offset;
    uint32_t cluster_offsets[MAX_CLUSTERS];
    uint32_t cluster_l2_offsets[MAX_CLUSTERS];
    uint32_t core_l2_offsets[MAX_CORES];
    uint32_t core_offsets[MAX_CORES];
    uint32_t l1d_offsets[MAX_CORES];
    uint32_t l1i_offsets[MAX_CORES];
} OffsetTable;

static OffsetTable offsets = {0};

// Detect topology features
#ifdef L3_SIZE
#define HAS_SHARED_L3 1
#else
#define HAS_SHARED_L3 0
#endif

#if defined(CLUSTER0_L2_SIZE) || defined(CLUSTER1_L2_SIZE) || defined(CLUSTER2_L2_SIZE)
#define HAS_PER_CLUSTER_L2 1
#else
#define HAS_PER_CLUSTER_L2 0
#endif

#if defined(CORE0_L2_SIZE) || defined(CORE1_L2_SIZE) || defined(CORE2_L2_SIZE) || \
    defined(CORE3_L2_SIZE) || defined(CORE4_L2_SIZE) || defined(CORE5_L2_SIZE) || \
    defined(CORE6_L2_SIZE) || defined(CORE7_L2_SIZE)
#define HAS_PER_CORE_L2 1
#else
#define HAS_PER_CORE_L2 0
#endif

// Shared L2 if neither per-core nor per-cluster L2 is defined
#if !HAS_PER_CORE_L2 && !HAS_PER_CLUSTER_L2
#define HAS_SHARED_L2 1
#else
#define HAS_SHARED_L2 0
#endif

static void builder_init(PpttBuilder *builder, size_t initial_capacity) {
    builder->buffer = malloc(initial_capacity);
    builder->size = 0;
    builder->capacity = initial_capacity;
    memset(builder->buffer, 0, initial_capacity);
}

static uint32_t builder_append(PpttBuilder *builder, const void *data, size_t len) {
    uint32_t offset = builder->size;
    
    if (builder->size + len > builder->capacity) {
        builder->capacity *= 2;
        builder->buffer = realloc(builder->buffer, builder->capacity);
    }
    
    memcpy(builder->buffer + builder->size, data, len);
    builder->size += len;
    
    return offset;
}

uint8_t calculate_checksum(uint8_t *data, uint32_t length) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return (uint8_t)(0 - sum);
}

static void build_header(PpttBuilder *builder) {
    AcpiTableHeader header = {
        .Signature = {'P', 'P', 'T', 'T'},
        .Length = 0,
        .Revision = ACPI_REVISION_3,
        .Checksum = 0,
        .OemId = PPTT_OEM_ID,
        .OemTableId = PPTT_OEM_TABLE_ID,
        .OemRevision = PPTT_OEM_REVISION,
        .CreatorId = ACPI_CREATOR_ID,
        .CreatorRevision = ACPI_CREATOR_REVISION
    };
    
    builder_append(builder, &header, sizeof(header));
}

static void build_package(PpttBuilder *builder) {
    offsets.package_offset = builder->size;
    
    uint32_t num_refs = 0;
    if (HAS_SHARED_L2) num_refs++;
    if (HAS_SHARED_L3) num_refs++;
    
    PpttProcessorNode package = {
        .Type = 0,
        .Length = sizeof(PpttProcessorNode) + num_refs * sizeof(uint32_t),
        .Reserved = 0,
        .Flags = PPTT_FLAG_PHYSICAL_PACKAGE,
        .Parent = 0,
        .AcpiProcessorId = 0,
        .NumberOfPrivateResources = num_refs
    };
    
    builder_append(builder, &package, sizeof(package));
    
    for (uint32_t i = 0; i < num_refs; i++) {
        uint32_t ref = 0;
        builder_append(builder, &ref, sizeof(ref));
    }
}

static void build_shared_l2(PpttBuilder *builder) {
#if HAS_SHARED_L2
    offsets.l2_shared_offset = builder->size;
    
    PpttCacheNode l2 = {
        .Type = 1,
        .Length = sizeof(PpttCacheNode),
        .Reserved = 0,
        .Flags = CACHE_FLAG_ALL_VALID,
        .NextLevelOfCache = HAS_SHARED_L3 ? 0 : 0,  // Will be backfilled with L3 later
        .Size = L2_SIZE,
        .NumberOfSets = L2_NUM_SETS,
        .Associativity = L2_ASSOCIATIVITY,
        .Attributes = L2_ATTRIBUTES,
        .LineSize = L2_LINE_SIZE,
        .CacheId = 0
    };
    
    builder_append(builder, &l2, sizeof(l2));
    
    // Backfill Package's L2 reference
    uint32_t *refs = (uint32_t *)(builder->buffer + offsets.package_offset + sizeof(PpttProcessorNode));
    refs[0] = offsets.l2_shared_offset;
#endif
}

static void build_shared_l3(PpttBuilder *builder) {
#if HAS_SHARED_L3
    offsets.l3_shared_offset = builder->size;
    
    PpttCacheNode l3 = {
        .Type = 1,
        .Length = sizeof(PpttCacheNode),
        .Reserved = 0,
        .Flags = CACHE_FLAG_ALL_VALID,
        .NextLevelOfCache = 0,
        .Size = L3_SIZE,
        .NumberOfSets = L3_NUM_SETS,
        .Associativity = L3_ASSOCIATIVITY,
        .Attributes = L3_ATTRIBUTES,
        .LineSize = L3_LINE_SIZE,
        .CacheId = 0
    };
    
    builder_append(builder, &l3, sizeof(l3));
    
    // Backfill Package's L3 reference
    uint32_t *refs = (uint32_t *)(builder->buffer + offsets.package_offset + sizeof(PpttProcessorNode));
    uint32_t ref_idx = HAS_SHARED_L2 ? 1 : 0;
    refs[ref_idx] = offsets.l3_shared_offset;
    
    // If there is shared L2, backfill L2's NextLevelOfCache
    if (HAS_SHARED_L2) {
        PpttCacheNode *l2 = (PpttCacheNode *)(builder->buffer + offsets.l2_shared_offset);
        l2->NextLevelOfCache = offsets.l3_shared_offset;
    }
#endif
}

static void build_per_core_l2(PpttBuilder *builder, uint32_t core_id) {
#if HAS_PER_CORE_L2
    offsets.core_l2_offsets[core_id] = builder->size;
    
    PpttCacheNode l2 = {
        .Type = 1,
        .Length = sizeof(PpttCacheNode),
        .Reserved = 0,
        .Flags = CACHE_FLAG_ALL_VALID,
        .NextLevelOfCache = HAS_SHARED_L3 ? offsets.l3_shared_offset : 0,
        .LineSize = 64,
        .CacheId = 0
    };
    
    // Set different L2 configuration based on core_id
    switch(core_id) {
#ifdef CORE0_L2_SIZE
        case 0:
            l2.Size = CORE0_L2_SIZE;
            l2.NumberOfSets = CORE0_L2_NUM_SETS;
            l2.Associativity = CORE0_L2_ASSOCIATIVITY;
            l2.Attributes = CORE0_L2_ATTRIBUTES;
            l2.LineSize = CORE0_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE1_L2_SIZE
        case 1:
            l2.Size = CORE1_L2_SIZE;
            l2.NumberOfSets = CORE1_L2_NUM_SETS;
            l2.Associativity = CORE1_L2_ASSOCIATIVITY;
            l2.Attributes = CORE1_L2_ATTRIBUTES;
            l2.LineSize = CORE1_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE2_L2_SIZE
        case 2:
            l2.Size = CORE2_L2_SIZE;
            l2.NumberOfSets = CORE2_L2_NUM_SETS;
            l2.Associativity = CORE2_L2_ASSOCIATIVITY;
            l2.Attributes = CORE2_L2_ATTRIBUTES;
            l2.LineSize = CORE2_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE3_L2_SIZE
        case 3:
            l2.Size = CORE3_L2_SIZE;
            l2.NumberOfSets = CORE3_L2_NUM_SETS;
            l2.Associativity = CORE3_L2_ASSOCIATIVITY;
            l2.Attributes = CORE3_L2_ATTRIBUTES;
            l2.LineSize = CORE3_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE4_L2_SIZE
        case 4:
            l2.Size = CORE4_L2_SIZE;
            l2.NumberOfSets = CORE4_L2_NUM_SETS;
            l2.Associativity = CORE4_L2_ASSOCIATIVITY;
            l2.Attributes = CORE4_L2_ATTRIBUTES;
            l2.LineSize = CORE4_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE5_L2_SIZE
        case 5:
            l2.Size = CORE5_L2_SIZE;
            l2.NumberOfSets = CORE5_L2_NUM_SETS;
            l2.Associativity = CORE5_L2_ASSOCIATIVITY;
            l2.Attributes = CORE5_L2_ATTRIBUTES;
            l2.LineSize = CORE5_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE6_L2_SIZE
        case 6:
            l2.Size = CORE6_L2_SIZE;
            l2.NumberOfSets = CORE6_L2_NUM_SETS;
            l2.Associativity = CORE6_L2_ASSOCIATIVITY;
            l2.Attributes = CORE6_L2_ATTRIBUTES;
            l2.LineSize = CORE6_L2_LINE_SIZE;
            break;
#endif
#ifdef CORE7_L2_SIZE
        case 7:
            l2.Size = CORE7_L2_SIZE;
            l2.NumberOfSets = CORE7_L2_NUM_SETS;
            l2.Associativity = CORE7_L2_ASSOCIATIVITY;
            l2.Attributes = CORE7_L2_ATTRIBUTES;
            l2.LineSize = CORE7_L2_LINE_SIZE;
            break;
#endif
        default:
            fprintf(stderr, "Error: L2 configuration for Core %d is not defined\n", core_id);
            return;
    }
    
    builder_append(builder, &l2, sizeof(l2));
#endif
}

static void build_cluster_l2(PpttBuilder *builder, uint32_t cluster_id) {
#if HAS_PER_CLUSTER_L2
    offsets.cluster_l2_offsets[cluster_id] = builder->size;
    
    PpttCacheNode l2 = {
        .Type = 1,
        .Length = sizeof(PpttCacheNode),
        .Reserved = 0,
        .Flags = CACHE_FLAG_ALL_VALID,
        .NextLevelOfCache = HAS_SHARED_L3 ? offsets.l3_shared_offset : 0,
        .LineSize = 64,
        .CacheId = 0
    };
    
    // Set different L2 configuration based on cluster_id
    switch(cluster_id) {
#ifdef CLUSTER0_L2_SIZE
        case 0:
            l2.Size = CLUSTER0_L2_SIZE;
            l2.NumberOfSets = CLUSTER0_L2_NUM_SETS;
            l2.Associativity = CLUSTER0_L2_ASSOCIATIVITY;
            l2.Attributes = CLUSTER0_L2_ATTRIBUTES;
            l2.LineSize = CLUSTER0_L2_LINE_SIZE;
            break;
#endif
#ifdef CLUSTER1_L2_SIZE
        case 1:
            l2.Size = CLUSTER1_L2_SIZE;
            l2.NumberOfSets = CLUSTER1_L2_NUM_SETS;
            l2.Associativity = CLUSTER1_L2_ASSOCIATIVITY;
            l2.Attributes = CLUSTER1_L2_ATTRIBUTES;
            l2.LineSize = CLUSTER1_L2_LINE_SIZE;
            break;
#endif
#ifdef CLUSTER2_L2_SIZE
        case 2:
            l2.Size = CLUSTER2_L2_SIZE;
            l2.NumberOfSets = CLUSTER2_L2_NUM_SETS;
            l2.Associativity = CLUSTER2_L2_ASSOCIATIVITY;
            l2.Attributes = CLUSTER2_L2_ATTRIBUTES;
            l2.LineSize = CLUSTER2_L2_LINE_SIZE;
            break;
#endif
        default:
            fprintf(stderr, "Error: L2 configuration for Cluster %d is not defined\n", cluster_id);
            return;
    }
    
    builder_append(builder, &l2, sizeof(l2));
#endif
}

static void build_cluster(PpttBuilder *builder, uint32_t cluster_id) {
    offsets.cluster_offsets[cluster_id] = builder->size;
    
    uint32_t num_refs = HAS_PER_CLUSTER_L2 ? 1 : 0;
    
    PpttProcessorNode cluster = {
        .Type = 0,
        .Length = sizeof(PpttProcessorNode) + num_refs * sizeof(uint32_t),
        .Reserved = 0,
        .Flags = 0,
        .Parent = offsets.package_offset,
        .AcpiProcessorId = 0,
        .NumberOfPrivateResources = num_refs
    };
    
    builder_append(builder, &cluster, sizeof(cluster));
    
    if (HAS_PER_CLUSTER_L2) {
        uint32_t l2_ref = 0;  // Will be backfilled later
        builder_append(builder, &l2_ref, sizeof(l2_ref));
    }
}

static uint32_t get_next_cache_level_for_core(uint32_t core_id) {
    if (HAS_PER_CORE_L2) {
        return offsets.core_l2_offsets[core_id];
    }
    return 0;
}

static uint32_t get_next_cache_level(uint32_t cluster_id) {
    if (HAS_PER_CLUSTER_L2) {
        return offsets.cluster_l2_offsets[cluster_id];
    } else if (HAS_SHARED_L2) {
        return offsets.l2_shared_offset;
    } else if (HAS_SHARED_L3) {
        return offsets.l3_shared_offset;
    }
    return 0;
}

static uint32_t build_l1d_cache(PpttBuilder *builder, uint32_t cluster_id, uint32_t core_id) {
    uint32_t offset = builder->size;
    
    PpttCacheNode l1d = {
        .Type = 1,
        .Length = sizeof(PpttCacheNode),
        .Reserved = 0,
        .Flags = CACHE_FLAG_ALL_VALID,
        .NextLevelOfCache = HAS_PER_CORE_L2 ? get_next_cache_level_for_core(core_id) : get_next_cache_level(cluster_id),
        .LineSize = 64,
        .CacheId = 0
    };
    
    switch(cluster_id) {
        case 0:
            l1d.Size = CLUSTER0_L1D_SIZE;
            l1d.NumberOfSets = CLUSTER0_L1D_NUM_SETS;
            l1d.Associativity = CLUSTER0_L1D_ASSOCIATIVITY;
            l1d.Attributes = CLUSTER0_L1D_ATTRIBUTES;
            l1d.LineSize = CLUSTER0_L1D_LINE_SIZE;
            break;
#ifdef CLUSTER1_L1D_SIZE
        case 1:
            l1d.Size = CLUSTER1_L1D_SIZE;
            l1d.NumberOfSets = CLUSTER1_L1D_NUM_SETS;
            l1d.Associativity = CLUSTER1_L1D_ASSOCIATIVITY;
            l1d.Attributes = CLUSTER1_L1D_ATTRIBUTES;
            l1d.LineSize = CLUSTER1_L1D_LINE_SIZE;
            break;
#endif
#ifdef CLUSTER2_L1D_SIZE
        case 2:
            l1d.Size = CLUSTER2_L1D_SIZE;
            l1d.NumberOfSets = CLUSTER2_L1D_NUM_SETS;
            l1d.Associativity = CLUSTER2_L1D_ASSOCIATIVITY;
            l1d.Attributes = CLUSTER2_L1D_ATTRIBUTES;
            l1d.LineSize = CLUSTER2_L1D_LINE_SIZE;
            break;
#endif
#ifdef CLUSTER3_L1D_SIZE
        case 3:
            l1d.Size = CLUSTER3_L1D_SIZE;
            l1d.NumberOfSets = CLUSTER3_L1D_NUM_SETS;
            l1d.Associativity = CLUSTER3_L1D_ASSOCIATIVITY;
            l1d.Attributes = CLUSTER3_L1D_ATTRIBUTES;
            l1d.LineSize = CLUSTER3_L1D_LINE_SIZE;
            break;
#endif
        default:
            break;
    }
    
    builder_append(builder, &l1d, sizeof(l1d));
    return offset;
}

static uint32_t build_l1i_cache(PpttBuilder *builder, uint32_t cluster_id, uint32_t core_id) {
    uint32_t offset = builder->size;
    
    PpttCacheNode l1i = {
        .Type = 1,
        .Length = sizeof(PpttCacheNode),
        .Reserved = 0,
        .Flags = CACHE_FLAG_ALL_VALID,
        .NextLevelOfCache = HAS_PER_CORE_L2 ? get_next_cache_level_for_core(core_id) : get_next_cache_level(cluster_id),
        .LineSize = 64,
        .CacheId = 0
    };
    
    switch(cluster_id) {
        case 0:
            l1i.Size = CLUSTER0_L1I_SIZE;
            l1i.NumberOfSets = CLUSTER0_L1I_NUM_SETS;
            l1i.Associativity = CLUSTER0_L1I_ASSOCIATIVITY;
            l1i.Attributes = CLUSTER0_L1I_ATTRIBUTES;
            l1i.LineSize = CLUSTER0_L1I_LINE_SIZE;
            break;
#ifdef CLUSTER1_L1I_SIZE
        case 1:
            l1i.Size = CLUSTER1_L1I_SIZE;
            l1i.NumberOfSets = CLUSTER1_L1I_NUM_SETS;
            l1i.Associativity = CLUSTER1_L1I_ASSOCIATIVITY;
            l1i.Attributes = CLUSTER1_L1I_ATTRIBUTES;
            l1i.LineSize = CLUSTER1_L1I_LINE_SIZE;
            break;
#endif
#ifdef CLUSTER2_L1I_SIZE
        case 2:
            l1i.Size = CLUSTER2_L1I_SIZE;
            l1i.NumberOfSets = CLUSTER2_L1I_NUM_SETS;
            l1i.Associativity = CLUSTER2_L1I_ASSOCIATIVITY;
            l1i.Attributes = CLUSTER2_L1I_ATTRIBUTES;
            l1i.LineSize = CLUSTER2_L1I_LINE_SIZE;
            break;
#endif
#ifdef CLUSTER3_L1I_SIZE
        case 3:
            l1i.Size = CLUSTER3_L1I_SIZE;
            l1i.NumberOfSets = CLUSTER3_L1I_NUM_SETS;
            l1i.Associativity = CLUSTER3_L1I_ASSOCIATIVITY;
            l1i.Attributes = CLUSTER3_L1I_ATTRIBUTES;
            l1i.LineSize = CLUSTER3_L1I_LINE_SIZE;
            break;
#endif
        default:
            break;
    }
    
    builder_append(builder, &l1i, sizeof(l1i));
    return offset;
}

static void build_core(PpttBuilder *builder, uint32_t cluster_id, uint32_t acpi_id) {
    offsets.core_offsets[acpi_id] = builder->size;
    
    PpttProcessorNode core = {
        .Type = 0,
        .Length = sizeof(PpttProcessorNode) + 2 * sizeof(uint32_t),
        .Reserved = 0,
        .Flags = PPTT_FLAG_ACPI_PROC_ID_VALID | PPTT_FLAG_PROCESSOR_IS_THREAD | PPTT_FLAG_NODE_IS_LEAF,
        .Parent = offsets.cluster_offsets[cluster_id],
        .AcpiProcessorId = acpi_id,
        .NumberOfPrivateResources = 2
    };
    
    builder_append(builder, &core, sizeof(core));
    
    uint32_t l1d_ref_offset = builder->size;
    uint32_t l1d_ref = 0;
    builder_append(builder, &l1d_ref, sizeof(l1d_ref));
    
    uint32_t l1i_ref_offset = builder->size;
    uint32_t l1i_ref = 0;
    builder_append(builder, &l1i_ref, sizeof(l1i_ref));
    
    offsets.l1d_offsets[acpi_id] = build_l1d_cache(builder, cluster_id, acpi_id);
    offsets.l1i_offsets[acpi_id] = build_l1i_cache(builder, cluster_id, acpi_id);
    
    *(uint32_t *)(builder->buffer + l1d_ref_offset) = offsets.l1d_offsets[acpi_id];
    *(uint32_t *)(builder->buffer + l1i_ref_offset) = offsets.l1i_offsets[acpi_id];
}

static void build_pptt_table(PpttBuilder *builder) {
    build_header(builder);
    build_package(builder);
    
    // Build shared caches
    build_shared_l2(builder);
    build_shared_l3(builder);
    
    // In per-core L2 architecture, build L2 for all cores first
#if HAS_PER_CORE_L2
    uint32_t total_cores = 0;
#ifdef CLUSTER0_CORES
    total_cores += CLUSTER0_CORES;
#endif
#ifdef CLUSTER1_CORES
    total_cores += CLUSTER1_CORES;
#endif
#ifdef CLUSTER2_CORES
    total_cores += CLUSTER2_CORES;
#endif
#ifdef CLUSTER3_CORES
    total_cores += CLUSTER3_CORES;
#endif
    for (uint32_t core = 0; core < total_cores; core++) {
        build_per_core_l2(builder, core);
    }
#endif
    
    // Build each cluster
    uint32_t acpi_id = 0;
    
#ifdef CLUSTER0_CORES
    build_cluster(builder, 0);
    if (HAS_PER_CLUSTER_L2) {
        build_cluster_l2(builder, 0);
        // Backfill cluster's L2 reference
        uint32_t *l2_ref = (uint32_t *)(builder->buffer + offsets.cluster_offsets[0] + sizeof(PpttProcessorNode));
        *l2_ref = offsets.cluster_l2_offsets[0];
    }
    for (uint32_t i = 0; i < CLUSTER0_CORES; i++) {
        build_core(builder, 0, acpi_id++);
    }
#endif
    
#ifdef CLUSTER1_CORES
    build_cluster(builder, 1);
    if (HAS_PER_CLUSTER_L2) {
        build_cluster_l2(builder, 1);
        uint32_t *l2_ref = (uint32_t *)(builder->buffer + offsets.cluster_offsets[1] + sizeof(PpttProcessorNode));
        *l2_ref = offsets.cluster_l2_offsets[1];
    }
    for (uint32_t i = 0; i < CLUSTER1_CORES; i++) {
        build_core(builder, 1, acpi_id++);
    }
#endif
    
#ifdef CLUSTER2_CORES
    build_cluster(builder, 2);
    if (HAS_PER_CLUSTER_L2) {
        build_cluster_l2(builder, 2);
        uint32_t *l2_ref = (uint32_t *)(builder->buffer + offsets.cluster_offsets[2] + sizeof(PpttProcessorNode));
        *l2_ref = offsets.cluster_l2_offsets[2];
    }
    for (uint32_t i = 0; i < CLUSTER2_CORES; i++) {
        build_core(builder, 2, acpi_id++);
    }
#endif
    
#ifdef CLUSTER3_CORES
    build_cluster(builder, 3);
    if (HAS_PER_CLUSTER_L2) {
        build_cluster_l2(builder, 3);
        uint32_t *l2_ref = (uint32_t *)(builder->buffer + offsets.cluster_offsets[3] + sizeof(PpttProcessorNode));
        *l2_ref = offsets.cluster_l2_offsets[3];
    }
    for (uint32_t i = 0; i < CLUSTER3_CORES; i++) {
        build_core(builder, 3, acpi_id++);
    }
#endif
    
    AcpiTableHeader *header = (AcpiTableHeader *)builder->buffer;
    header->Length = builder->size;
    header->Checksum = 0;
    header->Checksum = calculate_checksum(builder->buffer, builder->size);
}

static int write_aml_file(const char *filename, const uint8_t *data, size_t size) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("Unable to create file");
        return -1;
    }
    
    size_t written = fwrite(data, 1, size, fp);
    fclose(fp);
    
    if (written != size) {
        fprintf(stderr, "Failed to write file\n");
        return -1;
    }
    
    return 0;
}

int main(void) {
    printf("Initializing PPTT table...\n");
    
    PpttBuilder builder;
    builder_init(&builder, 4096);
    
    build_pptt_table(&builder);
    
    printf("PPTT Table Structure:\n");
    printf("  Total size: %zu bytes\n", builder.size);
    printf("  Package (Physical): 1\n");
    printf("  Clusters: %d\n", NUM_CLUSTERS);
    
#ifdef CLUSTER0_CORES
    printf("    - Cluster 0: %d cores\n", CLUSTER0_CORES);
    printf("      L1D: %llu KB, %d-way\n", (unsigned long long)(CLUSTER0_L1D_SIZE / 1024), CLUSTER0_L1D_ASSOCIATIVITY);
    printf("      L1I: %llu KB, %d-way\n", (unsigned long long)(CLUSTER0_L1I_SIZE / 1024), CLUSTER0_L1I_ASSOCIATIVITY);
#ifdef CLUSTER0_L2_SIZE
    printf("      L2: %llu KB, %d-way\n", (unsigned long long)(CLUSTER0_L2_SIZE / 1024), CLUSTER0_L2_ASSOCIATIVITY);
#endif
#endif

#ifdef CLUSTER1_CORES
    printf("    - Cluster 1: %d cores\n", CLUSTER1_CORES);
    printf("      L1D: %llu KB, %d-way\n", (unsigned long long)(CLUSTER1_L1D_SIZE / 1024), CLUSTER1_L1D_ASSOCIATIVITY);
    printf("      L1I: %llu KB, %d-way\n", (unsigned long long)(CLUSTER1_L1I_SIZE / 1024), CLUSTER1_L1I_ASSOCIATIVITY);
#ifdef CLUSTER1_L2_SIZE
    printf("      L2: %llu KB, %d-way\n", (unsigned long long)(CLUSTER1_L2_SIZE / 1024), CLUSTER1_L2_ASSOCIATIVITY);
#endif
#endif

#ifdef CLUSTER2_CORES
    printf("    - Cluster 2: %d cores\n", CLUSTER2_CORES);
    printf("      L1D: %llu KB, %d-way\n", (unsigned long long)(CLUSTER2_L1D_SIZE / 1024), CLUSTER2_L1D_ASSOCIATIVITY);
    printf("      L1I: %llu KB, %d-way\n", (unsigned long long)(CLUSTER2_L1I_SIZE / 1024), CLUSTER2_L1I_ASSOCIATIVITY);
#ifdef CLUSTER2_L2_SIZE
    printf("      L2: %llu KB, %d-way\n", (unsigned long long)(CLUSTER2_L2_SIZE / 1024), CLUSTER2_L2_ASSOCIATIVITY);
#endif
#endif

#ifdef CLUSTER3_CORES
    printf("    - Cluster 3: %d cores\n", CLUSTER3_CORES);
    printf("      L1D: %llu KB, %d-way\n", (unsigned long long)(CLUSTER3_L1D_SIZE / 1024), CLUSTER3_L1D_ASSOCIATIVITY);
    printf("      L1I: %llu KB, %d-way\n", (unsigned long long)(CLUSTER3_L1I_SIZE / 1024), CLUSTER3_L1I_ASSOCIATIVITY);
#ifdef CLUSTER3_L2_SIZE
    printf("      L2: %llu KB, %d-way\n", (unsigned long long)(CLUSTER3_L2_SIZE / 1024), CLUSTER3_L2_ASSOCIATIVITY);
#endif
#endif

#if HAS_PER_CORE_L2
    printf("  L2 Cache (Per-core): Each CPU has private L2\n");
#endif

#if HAS_SHARED_L2
    printf("  L2 Cache (Shared): %llu KB, %d-way\n", (unsigned long long)(L2_SIZE / 1024), L2_ASSOCIATIVITY);
#endif

#if HAS_SHARED_L3
    printf("  L3 Cache (Shared): %llu KB, %d-way\n", (unsigned long long)(L3_SIZE / 1024), L3_ASSOCIATIVITY);
#endif
    
    printf("\n");
    
    if (write_aml_file("PPTT.aml", builder.buffer, builder.size) == 0) {
        printf("Successfully generated PPTT.aml (%zu bytes)\n", builder.size);
        AcpiTableHeader *header = (AcpiTableHeader *)builder.buffer;
        printf("Checksum: 0x%02X\n", header->Checksum);
    } else {
        fprintf(stderr, "Generation failed\n");
        free(builder.buffer);
        return 1;
    }
    
    free(builder.buffer);
    return 0;
}
