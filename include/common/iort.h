#pragma once
#include <acpi.h>
#include <common.h>

/** IO Remapping Table (IORT)
Reference:
  https://developer.arm.com/documentation/den0049/latest
*/

/* Table signature */
#define ACPI_IORT_SIGNATURE 'I', 'O', 'R', 'T'
#define ACPI_IORT_REVISION 7

#define ACPI_IORT_TABLE_STRUCTURE_NAME IO_REMAPPING_TABLE

/* Header Extra Data */
typedef struct {
  UINT32 NumOfNodes;
  UINT32 OffsetToNodeArray;
  UINT32 Reserved;
  /* Optional padding */
  /* Arrary of iort nodes */
} __attribute__((packed)) IORT_HEADER_EXTRA_DATA;
_Static_assert(sizeof(IORT_HEADER_EXTRA_DATA) == 12,
               "IORT_HEADER_EXTRA_DATA size incorrect");

/* Body Structures */
typedef struct {
  UINT8 Type; // Check iort node type
  UINT16 Length;
  UINT8 Revision;
  UINT32 Identifier;
  UINT32 NumOfIDMappings;
  UINT32 ReferenceToIdArray;
  // Data Specific to Node
  // Array of ID mappings
} __attribute__((packed)) IORT_NODE_FORMAT;
_Static_assert(sizeof(IORT_NODE_FORMAT) == 16,
               "IORT_NODE_FORMAT size incorrect");

typedef struct {
  UINT32 InputBase;
  UINT32 NumOfIds;
  UINT32 OutputBase;
  UINT32 OutputReference;
  UINT32 Flags;
} __attribute__((packed)) IORT_ID_MAPPING_FORMAT;
_Static_assert(sizeof(IORT_ID_MAPPING_FORMAT) == 20,
               "IORT_ID_MAPPING_FORMAT size incorrect");

// ID Mapping Flags
#define IORT_ID_MAPPING_FLAG_SINGLE_MAPPING BIT(0)
#define IORT_ID_MAPPING_FLAG_RESERVED GEN_MSK(31, 1)

enum IORT_NODE_TYPE {
  IORT_NODE_TYPE_ITS_GROUP = 0,
  IORT_NODE_TYPE_NAMED_COMPONENT = 1,
  IORT_NODE_TYPE_ROOT_COMPLEX = 2,
  IORT_NODE_TYPE_SMMU_V1_V2 = 3,
  IORT_NODE_TYPE_SMMU_V3 = 4,
  IORT_NODE_TYPE_PMCG = 5,
  IORT_NODE_TYPE_MEMORY_RANGE = 6,
  IORT_NODE_TYPE_IWB = 7,
  IORT_NODE_TYPE_RESERVED = 0xFF
};

typedef struct {
  UINT32 CCA; // Cache Coherency Attribute
  UINT8 AH;   // Allocation Hint
  UINT16 Reserved;
  UINT8 MAF; // Memory Access Flags
} __attribute__((packed)) IORT_MEMORY_ACCESS_PROPERTIES;
_Static_assert(sizeof(IORT_MEMORY_ACCESS_PROPERTIES) == 8,
               "IORT_MEMORY_ACCESS_PROPERTIES size incorrect");

// Coherent Path to Memory
#define IORT_MEMORY_ACCESS_FLAG_CPM BIT(0)
// Device attribute are cacheable and inner sharable
#define IORT_MEMORY_ACCESS_FLAG_DCAS BIT(1)
// Coherency of Accesses not marked inner/outer WB cacheable shareable
#define IORT_MEMORY_ACCESS_FLAG_CANWBS BIT(2)
#define IORT_MEMORY_ACCESS_FLAG_RESERVED GEN_MSK(31, 3)

// IORT node types (chapter 2.1)
// Revision should be 3
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // smmu v1/v2 specific data
  UINT64 BaseAddress;
  UINT64 Span;
  UINT32 Model; // Check IORT_SMMU_V1_V2_MODEL
  UINT32 Flags;
  UINT32 ReferenceToGlobalInterruptArray;
  UINT32 NumOfContextInterrupts;
  UINT32 ReferenceToContextInterruptArray;
  UINT32 NumOfPMUInterrupts;
  UINT32 ReferenceToPMUInterruptArray;
  // Global interrupt array section
  UINT32 SMMUNSgIrpt;
  UINT32 SMMUNSgIrptInterruptFlags;
  UINT32 SMMUNSgCfgIrpt;
  UINT32 SMMUNSgCfgIrptInterruptFlags;
  // Context interrupt array section
  // PMU interrupt array section
  // IDs for SMMUv1/v2 section
} __attribute__((packed)) IORT_SMMU_V1_V2_NODE;
_Static_assert(sizeof(IORT_SMMU_V1_V2_NODE) == 76,
               "IORT_SMMU_V1_V2_NODE size incorrect");

enum IORT_SMMU_V1_V2_MODEL {
  IORT_SMMU_V1_V2_MODEL_GENERIC_SMMU_V1 = 0,
  IORT_SMMU_V1_V2_MODEL_GENERIC_SMMU_V2 = 1,
  IORT_SMMU_V1_V2_MODEL_ARM_MMU400 = 2,
  IORT_SMMU_V1_V2_MODEL_ARM_MMU500 = 3,
  IORT_SMMU_V1_V2_MODEL_ARM_MMU401 = 4,
  IORT_SMMU_V1_V2_MODEL_CAVIUM_THUNDERX_SMMUV2 = 5,
  IORT_SMMU_V1_V2_MODEL_RESERVED = 0xFFFFFFFF
};

#define IORT_SMMU_V1_V2_DVM_SUPPORT BIT(0)
#define IORT_SMMU_V1_V2_COHERENT_PAGE_TABLE_WALK BIT(1)
#define IORT_SMMU_V1_V2_RESERVED GEN_MSK(31, 2)

// 0 => level triggered
// 1 => edge triggered
#define IORT_SMMU_V1_V2_INTERRUPT_FLAG BIT(0)
enum IORT_SMMU_V1_V2_INTERRUPT_FLAG_TYPE {
  IORT_SMMU_V1_V2_INTERRUPT_FLAG_TYPE_LEVEL = 0,
  IORT_SMMU_V1_V2_INTERRUPT_FLAG_TYPE_EDGE = 1,
};
#define IORT_SMMU_V1_V2_INTERRUPT_FLAG_RESERVED GEN_MSK(31, 1)

// SMMUv3 node type
// Revision should be 5
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // Smmu v3 specific data
  UINT64 BaseAddress;
  UINT32 Flags;
  UINT32 Reserved;
  UINT64 VATOSAddress;
  UINT32 Model; // Check IORT_SMMU_V3_MODEL
  UINT32 Event;
  UINT32 PRI;
  UINT32 GERR;
  UINT32 Sync;
  UINT32 ProximityDomain;
  UINT32 DeviceIDMappingIndex;
  // IDs for SMMUv3 section
  // Array of ID mappings
} __attribute__((packed)) IORT_SMMU_V3_NODE;
_Static_assert(sizeof(IORT_SMMU_V3_NODE) == 68,
               "IORT_SMMU_V3_NODE size incorrect");

enum IORT_SMMU_V3_MODEL {
  IORT_SMMU_V3_MODEL_GENERIC_SMMU_V3 = 0,
  IORT_SMMU_V3_MODEL_HISI_HI161_SMMU_V3 = 1,
  IORT_SMMU_V3_MODEL_CAVIUM_CN99XX_SMMU_V3 = 2,
  IORT_SMMU_V3_MODEL_RESERVED = 0xFFFFFFFF
};

#define IORT_SMMU_V3_COHACC_OVERRIDE BIT(0)
#define IORT_SMMU_V3_HTTU_OVERRIDE GEN_MSK(2, 1)
#define IORT_SMMU_V3_PROXIMITY_DOMAIN_VALID BIT(3)
#define IORT_SMMU_V3_DEVICE_ID_MAPPING_INDEX_VALID BIT(4)
#define IORT_SMMU_V3_RESERVED GEN_MSK(31, 5)

// Performance Monitoring counter group
// Revision should be 2
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // pmcg specific data
  UINT64 Page0BaseAddress;
  UINT32 OverflowInterruptGSIV;
  UINT32 NodeReference;
  UINT64 Page1BaseAddress;
  // IDs for named component
} __attribute__((packed)) IORT_PMCG_NODE;

// ITS Group node format
// Revision should be 1
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // its group specific data
  UINT32 NumOfITS;
  // UINT32 GicITSIdentifierArrary[];
} __attribute__((packed)) IORT_ITS_GROUP_NODE;

// Named component node format
// Revision should be 4
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // named component specific data
  UINT32 Flags;
  IORT_MEMORY_ACCESS_PROPERTIES MemAccessProps;
  UINT8 DeviceMemoryAddressSizeLimit;
  // CHAR8 DeviceObjectName[]; // Null-terminated string
  // UINT8 Padding[]; // to 32 bit word align
  // Array of IDs mapping
} __attribute__((packed)) IORT_NAMED_COMPONENT_NODE;
_Static_assert(sizeof(IORT_NAMED_COMPONENT_NODE) == 29,
               "IORT_NAMED_COMPONENT_NODE size incorrect");

#define IORT_NAMED_COMPONENT_FLAG_STALL_SUPPORTED BIT(0)
// Number of substreams bits supported by this device.
#define IORT_NAMED_COMPONENT_FLAG_SUBSTREAM_WIDTH GEN_MSK(5, 1)

// PCI Root Complex node format
// Revision should be 4
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // pci root complex specific data
  IORT_MEMORY_ACCESS_PROPERTIES MemAccessProps;
  UINT32 ATSAttribute;
  UINT32 PCISegmentNumber;
  UINT8 MemoryAddressSizeLimit;
  UINT16 PASIDCapabilities;
  UINT8 Reserved;
  UINT32 Flags;
  // IDs for root complex
} __attribute__((packed)) IORT_PCI_ROOT_COMPLEX_NODE;
_Static_assert(sizeof(IORT_PCI_ROOT_COMPLEX_NODE) == 40,
               "IORT_PCI_ROOT_COMPLEX_NODE size incorrect");

#define IORT_PCI_ROOT_COMPLEX_FLAG_PASID_SUPPORT BIT(0)
enum IORT_PCI_ROOT_COMPLEX_PASID_CAP {
  IORT_PCI_ROOT_COMPLEX_PASID_NOT_SUPPORTED = 0,
  IORT_PCI_ROOT_COMPLEX_PASID_SUPPORTED = 1
};

typedef struct {
  UINT64 PhysicalRangeOffset;
  UINT64 PhysicalRangeLength;
  UINT32 Reserved;
} __attribute__((packed)) IORT_MEMORY_RANGE_DESCIPTOR;
_Static_assert(sizeof(IORT_MEMORY_RANGE_DESCIPTOR) == 20,
               "IORT_MEMORY_RANGE_DESCIPTOR size incorrect");

// Reserved Memory Range node format
// RMR is used to describe memory ranges that are reserved for use by endpoints.
// Revision should be 3
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // reserved memory range specific data
  UINT32 Flags;
  UINT32 NumOfMemoryRangeDescriptors;
  UINT32 ReferenceToMemoryRangeDescriptor;
} __attribute__((packed)) IORT_RESERVED_MEMORY_RANGE_NODE;
_Static_assert(sizeof(IORT_RESERVED_MEMORY_RANGE_NODE) == 28,
               "IORT_RESERVED_MEMORY_RANGE_NODE size incorrect");

// 0x1 => allow, 0x0 => disallow
#define IORT_RMR_FLAG_REMAPPING_PERMITTED BIT(0)
#define IORT_RMR_FLAG_ACCESS_PRIVILEGED BIT(1)
#define IORT_RMR_FLAG_ACCESS_ATTRIBUTES GEN_MSK(9, 2)
#define IORT_RMR_FLAG_RESERVED GEN_MSK(31, 10)

// IWB node format
// Revision should be 1
typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  // Padding to 0x10 aligned
  UINT32 Reserved;
  // iwb specific data
  UINT64 ConfigFrameBase;
  UINT16 IWBIndex;
  // CHAR8 DeviceObjectName[]; // Null-terminated string
  // UINT8 Padding[]; // to 32 bit word align
  // Array of IDs mapping
} __attribute__((packed)) IORT_IWB_NODE;
_Static_assert(sizeof(IORT_IWB_NODE) == 30, "IORT_IWB_NODE size incorrect");

/* IORT Table with Magic */
#define IORT_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_IORT_TABLE_STRUCTURE_NAME)
#define IORT_START ACPI_TABLE_START(ACPI_IORT_TABLE_STRUCTURE_NAME)
#define IORT_END ACPI_TABLE_END(ACPI_IORT_TABLE_STRUCTURE_NAME)
