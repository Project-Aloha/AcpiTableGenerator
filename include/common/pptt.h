#pragma once
#include <acpi.h>
#include <common.h>

/* ACPI Header */
#define ACPI_PPTT_SIGNATURE 'P', 'P', 'T', 'T'
#define ACPI_PPTT_REVISION 1

#define ACPI_PPTT_TABLE_STRUCTURE_NAME PROCESSOR_PROPERTIES_TOPOLOGY_TABLE

// Processor Properties Topology Table
// Body
typedef struct {
  uint32_t
      reference; // Each resource is a reference to another PPTT structure. The
                 //  structure referred to must not be a processor hierarchy
                 //  node. Each resource structure pointed to represents
                 //  resources that are private the processor hierarchy node.
                 //  For example, for cache resources, the cache type structure
                 //  represents caches that are private to the instance of
                 //  processor topology represented by this processor hierarchy
                 //  node structure. The references are encoded as the
                 //  difference between the start of the PPTT table and the
                 //  start of the resource structure entry.
} ACPI_PPTT_PRIVATE_RESOURCE;

// Processor Hierarchy Node (Table 5.189)
typedef struct {
  uint8_t Type;      // 0 - processor structure
  uint8_t Length;    // Length of the local processor structure in bytes
  uint16_t Reserved; // Must be zero
  uint32_t Flags;    // See Processor Structure Flags.
  uint32_t
      Parent; // Reference to parent processor hierarchy node structure. The
              // reference is encoded as the difference between the start of the
              // PPTT table and the start of the parent processor structure
              // entry. A value of zero must be used where a node has no parent.
  uint32_t
      AcpiProcessorId; // If the processor structure represents an actual
                       // processor, this field must match the value of ACPI
                       // processor ID field in the processor’s entry in the
                       // MADT. If the processor structure represents a group of
                       // associated processors, the structure might match a
                       // processor container in the name space. In that case
                       // this entry will match the value of the _UID method of
                       // the associated processor container. Where there is a
                       // match it must be represented. The flags field,
                       // described in Processor Structure Flags, includes a bit
                       // to describe whether the ACPI processor ID is valid.
  uint32_t NumberOfPrivateResources; // Number of resource structure references
                                     // in Private Resources(below)
  //   ACPI_PPTT_PRIVATE_RESOURCE
  //   PrivateResources[]; // Array of references to
  //                       // resources private to this
  //                       // processor hierarchy node.
} __attribute__((packed)) ACPI_PPTT_PROCESSOR_HIERARCHY_NODE;
_Static_assert(sizeof(ACPI_PPTT_PROCESSOR_HIERARCHY_NODE) == 20,
               "ACPI_PPTT_PROCESSOR_HIERARCHY_NODE size is incorrect");

// Cache Type Structure (Table 5.191)
typedef struct {
  uint8_t Type;      // 1 - Cache type structure
  uint8_t Length;    // 28
  uint16_t Reserved; // Must be zero
  uint32_t Flags;    // See Cache Structure Flags.
  uint32_t
      NextLevelOfCache; // Reference to next level of cache that is private to
                        // the processor topology instance. The reference is
                        // encoded as the difference between the start of the
                        // PPTT table and the start of the cache type structure
                        // entry. This value will be zero if this entry
                        // represents the last cache level appropriate to the
                        // processor hierarchy node structures using this entry.
  uint32_t Size;        // Size of the cache in bytes.
  uint32_t NumberOfSets; // Number of sets in the cache
  uint8_t Associativity; // Integer number of ways.
  uint8_t Attributes;    // Bits 1:0: Allocation type:
                         //   0x0 - Read allocate
                         //   0x1 - Write allocate
                         //   0x2 or 0x03 indicate Read and Write allocate
                         // Bits:3:2: Cache type:
                         //   0x0 Data
                         //   0x1 Instruction
                         //   0x2 or 0x3 Indicate a unified cache
                         // Bits 4: Write policy:
                         //   0x0 Write back
                         //   0x1 Write through
                         // Bits:7:5 Reserved must be zero.
  uint16_t LineSize;     // Line size in bytes
#if ACPI_PPTT_REVISION == 3
  uint32_t
      CacheId; // Unique, non-zero identifier for this cache. If Cache ID is
               // valid as indicated by the Flags field, then this structure
               // defines a unique cache in the system. A Cache ID value of 0
               // indicates a NULL identifier that is not valid.
#endif
} __attribute__((packed)) ACPI_PPTT_CACHE_TYPE_STRUCTURE;
#if ACPI_PPTT_REVISION == 1
_Static_assert(sizeof(ACPI_PPTT_CACHE_TYPE_STRUCTURE) == 24,
               "ACPI_PPTT_CACHE_TYPE_STRUCTURE size is incorrect");
#elif ACPI_PPTT_REVISION == 3
_Static_assert(sizeof(ACPI_PPTT_CACHE_TYPE_STRUCTURE) == 28,
               "ACPI_PPTT_CACHE_TYPE_STRUCTURE size is incorrect");
#endif

typedef struct {
  uint8_t Type;           // 2 - ID structure
  uint8_t Length;         // 30
  uint16_t Reserved;      // Must be zero
  uint32_t VendorId;      // Vendor ID
  uint64_t Level1Id;      // Level 1 ID
  uint64_t Level2Id;      // Level 2 ID
  uint16_t MajorRevision; // Major revision
  uint16_t MinorRevision; // Minor revision
  uint16_t SpinRevision;  // Spin revision
} __attribute__((packed)) ACPI_PPTT_ID;
_Static_assert(sizeof(ACPI_PPTT_ID) == 30, "ACPI_PPTT_ID size is incorrect");

// Processor Structure Flags (Table 5.190)
#define PPTT_PROC_FLAG_PHYSICAL_PACKAGE BIT(0)
#define PPTT_PROC_FLAG_ACPI_PROC_ID_VALID BIT(1)
#define PPTT_PROC_FLAG_PROCESSOR_IS_THREAD BIT(2)
#define PPTT_PROC_FLAG_NODE_IS_LEAF BIT(3)
#define PPTT_PROC_FLAG_IDENTICAL_IMPLEMENTATION BIT(4)
#define PPTT_PROC_FLAG_RESERVED GEN_MSK(31, 5)

// Cache Structure Flags (Table 5.192)
#define PPTT_CACHE_FLAG_SIZE_PROPERTY_VALID BIT(0)
#define PPTT_CACHE_FLAG_NUMBER_OF_SETS_VALID BIT(1)
#define PPTT_CACHE_FLAG_ASSOCIATIVITY_VALID BIT(2)
#define PPTT_CACHE_FLAG_ALLOCATION_TYPE_VALID BIT(3)
#define PPTT_CACHE_FLAG_CACHE_TYPE_VALID BIT(4)
#define PPTT_CACHE_FLAG_WRITE_POLICY_VALID BIT(5)
#define PPTT_CACHE_FLAG_LINE_SIZE_VALID BIT(6)
#define PPTT_CACHE_FLAG_CACHE_ID_VALID BIT(7)
#define PPTT_CACHE_FLAG_RESERVED GEN_MSK(31, 8)

// Cache Attributes (Table 5.191)
// Bits 1:0: Allocation type:
//     0x0 - Read allocate
//     0x1 - Write allocate
//     0x2 or 0x03 indicate Read and Write allocate
#define PPTT_CACHE_ATTR_ALLOCATION_TYPE_MSK GEN_MSK(1, 0)
#define PPTT_CACHE_ATTR_ALLOCATION_TYPE_READ 0x0
#define PPTT_CACHE_ATTR_ALLOCATION_TYPE_WRITE 0x1
#define PPTT_CACHE_ATTR_ALLOCATION_TYPE_RW 0x2
// Bits:3:2: Cache type:
//     0x0 Data
//     0x1 Instruction
//     0x2 or 0x3 Indicate a unified cache
#define PPTT_CACHE_ATTR_CACHE_TYPE_MSK GEN_MSK(3, 2)
#define PPTT_CACHE_ATTR_CACHE_TYPE_DATA 0x0
#define PPTT_CACHE_ATTR_CACHE_TYPE_INSTRUCTION 0x1
#define PPTT_CACHE_ATTR_CACHE_TYPE_UNIFIED 0x2
// Bits 4: Write policy:
//     0x0 Write back
//     0x1 Write through
#define PPTT_CACHE_ATTR_WRITE_POLICY_MSK BIT(4)
#define PPTT_CACHE_ATTR_WRITE_POLICY_WB 0x0
#define PPTT_CACHE_ATTR_WRITE_POLICY_WT 0x1
// Bits:7:5 Reserved must be zero.
#define PPTT_CACHE_ATTR_RESERVED_MSK GEN_MSK(7, 5)

/* Helper macros */
#if ACPI_PPTT_REVISION == 3
#define PPTT_DECLARE_CACHE(index, cache_type_val, next_level_of_cache, flag)   \
  .CacheTypeStructures[index] = {                                              \
      .Type = 1,                                                               \
      .Length = sizeof(ACPI_PPTT_CACHE_TYPE_STRUCTURE),                        \
      .Reserved = 0,                                                           \
      .Flags = flag,                                                           \
      .NextLevelOfCache = next_level_of_cache,                                 \
      .Size = 0,                                                               \
      .NumberOfSets = 0,                                                       \
      .Associativity = 0,                                                      \
      .Attributes = SET_BITS(PPTT_CACHE_ATTR_CACHE_TYPE_MSK, cache_type_val),  \
      .LineSize = 0,                                                           \
      .CacheId = 0,                                                            \
  }
#elif ACPI_PPTT_REVISION == 1
#define PPTT_DECLARE_CACHE(index, cache_type_val, next_level_of_cache, flag)   \
  .CacheTypeStructures[index] = {                                              \
      .Type = 1,                                                               \
      .Length = sizeof(ACPI_PPTT_CACHE_TYPE_STRUCTURE),                        \
      .Reserved = 0,                                                           \
      .Flags = flag,                                                           \
      .NextLevelOfCache = next_level_of_cache,                                 \
      .Size = 0,                                                               \
      .NumberOfSets = 0,                                                       \
      .Associativity = 0,                                                      \
      .Attributes = SET_BITS(PPTT_CACHE_ATTR_CACHE_TYPE_MSK, cache_type_val),  \
      .LineSize = 0,                                                           \
  }
#endif

#define PPTT_DECLARE_L1_DCACHE(index, next_level_of_cache)                     \
  PPTT_DECLARE_CACHE(index, PPTT_CACHE_ATTR_CACHE_TYPE_DATA,                   \
                     next_level_of_cache, PPTT_CACHE_FLAG_CACHE_TYPE_VALID)
#define PPTT_DECLARE_L1_ICACHE(index, next_level_of_cache)                     \
  PPTT_DECLARE_CACHE(index, PPTT_CACHE_ATTR_CACHE_TYPE_INSTRUCTION,            \
                     next_level_of_cache, PPTT_CACHE_FLAG_CACHE_TYPE_VALID)
#define PPTT_DECLARE_L2_CACHE(index, next_level_of_cache)                      \
  PPTT_DECLARE_CACHE(index, PPTT_CACHE_ATTR_CACHE_TYPE_UNIFIED,                \
                     next_level_of_cache, PPTT_CACHE_FLAG_CACHE_TYPE_VALID)
#define PPTT_DECLARE_L3_CACHE(index, next_level_of_cache)                      \
  PPTT_DECLARE_CACHE(index, PPTT_CACHE_ATTR_CACHE_TYPE_UNIFIED,                \
                     next_level_of_cache, PPTT_CACHE_FLAG_CACHE_TYPE_VALID)
#define PPTT_DECLARE_SIMPLE_CACHE(index, next_level_of_cache)                  \
  PPTT_DECLARE_CACHE(index, 0, next_level_of_cache, 0)

#define PPTT_REFERENCE_CACHE(index)                                            \
  ((UINT32)(offsetof(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE,                      \
                     CacheTypeStructures) +                                    \
            ((index) * sizeof(ACPI_PPTT_CACHE_TYPE_STRUCTURE))))

#define PPTT_DECLARE_PROCESSOR_HIERARCHY(name, index, cpuid, flags, parent,    \
                                         type, ...)                            \
  .name[index] = {.ProcNode.Type = 0,                                          \
                  .ProcNode.Length = sizeof(type),                             \
                  .ProcNode.Reserved = 0,                                      \
                  .ProcNode.Parent = (parent),                                 \
                  .ProcNode.Flags = (flags),                                   \
                  .ProcNode.AcpiProcessorId = (cpuid),                         \
                  .ProcNode.NumberOfPrivateResources = 0 __VA_OPT__(           \
                      +(sizeof((ACPI_PPTT_PRIVATE_RESOURCE[]){__VA_ARGS__}) /  \
                        sizeof(ACPI_PPTT_PRIVATE_RESOURCE))),                  \
                  __VA_OPT__(.PrivateResources = {__VA_ARGS__}, )}

#define PPTT_DECLARE_PROCESSOR_HIERARCHY_SYSTEM(index, cpuid, ...)             \
  PPTT_DECLARE_PROCESSOR_HIERARCHY(                                            \
      SystemHierarchyNode, index, cpuid, PPTT_PROC_FLAG_PHYSICAL_PACKAGE, 0,   \
      ACPI_PPTT_PROCESSOR_HIERARCHY_SYSTEM, __VA_ARGS__)

#define PPTT_DECLARE_PROCESSOR_HIERARCHY_CLUSTER(index, cpuid, parent, ...)    \
  PPTT_DECLARE_PROCESSOR_HIERARCHY(                                            \
      ClusterHierarchyNodes, index, cpuid, 0, parent,                          \
      ACPI_PPTT_PROCESSOR_HIERARCHY_CLUSTER, __VA_ARGS__)

#define PPTT_DECLARE_PROCESSOR_HIERARCHY_PHYSICAL_CPU(index, cpuid, parent,    \
                                                      ...)                     \
  PPTT_DECLARE_PROCESSOR_HIERARCHY(PhysicalCpuHierarchyNodes, index, cpuid,    \
                                   PPTT_PROC_FLAG_ACPI_PROC_ID_VALID, parent,  \
                                   ACPI_PPTT_PROCESSOR_HIERARCHY_PHYSICAL_CPU, \
                                   __VA_ARGS__)

#define PPTT_DECLARE_ID()                                                      \
  .Id = {                                                                      \
      .Type = 2,                                                               \
      .Length = sizeof(ACPI_PPTT_ID),                                          \
      .Reserved = 0,                                                           \
      .VendorId = 0,                                                           \
      .Level1Id = 0,                                                           \
      .Level2Id = 0,                                                           \
      .MajorRevision = 0,                                                      \
      .MinorRevision = 0,                                                      \
      .SpinRevision = 0,                                                       \
  }

#define PPTT_REFERENCE_ID                                                      \
  ((UINT32)(offsetof(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE, Id)))

#define PPTT_REFERENCE_SYSTEM                                                  \
  ((UINT32)(offsetof(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE, SystemHierarchyNode)))

#define PPTT_REFERENCE_CLUSTER(index)                                          \
  ((UINT32)(offsetof(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE,                      \
                     ClusterHierarchyNodes) +                                  \
            ((index) * (sizeof(ACPI_PPTT_PROCESSOR_HIERARCHY_NODE) +           \
                        sizeof(ACPI_PPTT_PRIVATE_RESOURCE) *                   \
                            CLUSTER_PRIVATE_RESOURCES_COUNT))))

#define PPTT_DEFINE_SYSTEM                                                     \
  typedef struct {                                                             \
    ACPI_PPTT_PROCESSOR_HIERARCHY_NODE ProcNode;                               \
    ACPI_PPTT_PRIVATE_RESOURCE                                                 \
    PrivateResources[SYSTEM_PRIVATE_RESOURCES_COUNT];                          \
  } __attribute__((packed)) ACPI_PPTT_PROCESSOR_HIERARCHY_SYSTEM;

#define PPTT_DEFINE_CLUSTER                                                    \
  typedef struct {                                                             \
    ACPI_PPTT_PROCESSOR_HIERARCHY_NODE ProcNode;                               \
    ACPI_PPTT_PRIVATE_RESOURCE                                                 \
    PrivateResources[CLUSTER_PRIVATE_RESOURCES_COUNT];                         \
  } __attribute__((packed)) ACPI_PPTT_PROCESSOR_HIERARCHY_CLUSTER;

#define PPTT_DEFINE_PHYSICAL_CPU                                               \
  typedef struct {                                                             \
    ACPI_PPTT_PROCESSOR_HIERARCHY_NODE ProcNode;                               \
    ACPI_PPTT_PRIVATE_RESOURCE                                                 \
    PrivateResources[PHYSICAL_CPU_PRIVATE_RESOURCES_COUNT];                    \
  } __attribute__((packed)) ACPI_PPTT_PROCESSOR_HIERARCHY_PHYSICAL_CPU;

#define PPTT_DEFINE_TABLE                                                      \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    ACPI_PPTT_ID Id;                                                           \
    ACPI_PPTT_CACHE_TYPE_STRUCTURE                                             \
    CacheTypeStructures[L1_CACHES_COUNT + L2_CACHES_COUNT + L3_CACHES_COUNT];  \
    ACPI_PPTT_PROCESSOR_HIERARCHY_SYSTEM                                       \
    SystemHierarchyNode[NUM_SYSTEM];                                           \
    ACPI_PPTT_PROCESSOR_HIERARCHY_CLUSTER                                      \
    ClusterHierarchyNodes[NUM_CLUSTERS];                                       \
    ACPI_PPTT_PROCESSOR_HIERARCHY_PHYSICAL_CPU                                 \
    PhysicalCpuHierarchyNodes[NUM_CORES];                                      \
  } __attribute__((packed)) ACPI_PPTT_TABLE_STRUCTURE_NAME;

#define PPTT_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(                                                   \
      ACPI_PPTT_SIGNATURE, ACPI_PPTT_TABLE_STRUCTURE_NAME, ACPI_PPTT_REVISION)

/* PPTT Table with Magic */
#define PPTT_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_PPTT_TABLE_STRUCTURE_NAME)
#define PPTT_START ACPI_TABLE_START(ACPI_PPTT_TABLE_STRUCTURE_NAME)
#define PPTT_END ACPI_TABLE_END(ACPI_PPTT_TABLE_STRUCTURE_NAME)