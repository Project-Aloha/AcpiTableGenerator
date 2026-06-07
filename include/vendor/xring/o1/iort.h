#pragma once
#include "table_header.h"
#include <common/iort.h>

/*
  This table is AI-generated.
  Currently only for testing purposes.
*/

#define IORT_ITS_COUNT 1
#define IORT_NODE_COUNT 7

#define IORT_SMMU_TO_ITS_MAPPING_COUNT 1
#define MEDIA1_SMMU_MAPPING_COUNT 2
#define MEDIA2_SMMU_MAPPING_COUNT 2
#define NPU_SMMU_MAPPING_COUNT 1
#define PCIE_SMMU_MAPPING_COUNT 1
#define PCI_ROOT_COMPLEX_MAPPING_COUNT 1

#define IORT_ITS_IDENTIFIER 0

#define MEDIA1_SMMU_BASE_ADDRESS 0xE7800000ULL
#define MEDIA1_SMMU_EVENT_GSIV GIC_SPI(0xDB)
#define MEDIA1_SMMU_SYNC_GSIV GIC_SPI(0xDC)
#define MEDIA1_SMMU_GERR_GSIV GIC_SPI(0xDD)

#define MEDIA2_SMMU_BASE_ADDRESS 0xE5400000ULL
#define MEDIA2_SMMU_EVENT_GSIV GIC_SPI(0x128)
#define MEDIA2_SMMU_SYNC_GSIV GIC_SPI(0x129)
#define MEDIA2_SMMU_GERR_GSIV GIC_SPI(0x12A)

#define NPU_SMMU_BASE_ADDRESS 0xE9400000ULL
#define NPU_SMMU_EVENT_GSIV GIC_SPI(0xCD)
#define NPU_SMMU_SYNC_GSIV GIC_SPI(0xCE)
#define NPU_SMMU_GERR_GSIV GIC_SPI(0xCF)

#define PCIE_SMMU_BASE_ADDRESS 0xCF200000ULL
#define PCIE_SMMU_EVENT_GSIV GIC_SPI(0x24E)
#define PCIE_SMMU_SYNC_GSIV GIC_SPI(0x24F)
#define PCIE_SMMU_GERR_GSIV GIC_SPI(0x250)

#define PCIE0_SEGMENT_NUMBER 0
#define PCIE1_SEGMENT_NUMBER 1
#define PCI_REQUESTER_ID_BASE 0x0000
#define PCI_REQUESTER_ID_COUNT 0x10000

#define IORT_DECLARE_NODE_HEADER(node_type, node_length, node_revision,        \
                                 node_identifier, mapping_count,              \
                                 mapping_offset)                              \
  .NodeHeader = {                                                             \
      .Type = node_type,                                                       \
      .Length = node_length,                                                   \
      .Revision = node_revision,                                               \
      .Identifier = node_identifier,                                           \
      .NumOfIDMappings = mapping_count,                                        \
      .ReferenceToIdArray = mapping_offset,                                    \
  }

#define IORT_DECLARE_ID_MAPPING(input_base, num_of_ids, output_base,           \
                                output_reference, flags)                      \
  {                                                                           \
      .InputBase = input_base,                                                 \
      .NumOfIds = num_of_ids,                                                  \
      .OutputBase = output_base,                                               \
      .OutputReference = output_reference,                                     \
      .Flags = flags,                                                          \
  }

#define IORT_DECLARE_SMMU_TO_ITS_MAPPING(input_base, num_of_ids)              \
  IORT_DECLARE_ID_MAPPING(                                                     \
      input_base, num_of_ids, input_base,                                      \
      offsetof(ACPI_IORT_TABLE_STRUCTURE_NAME, ItsGroupNode), 0)

#define IORT_DECLARE_PCI_TO_SMMU_MAPPING(smmu_node)                           \
  IORT_DECLARE_ID_MAPPING(                                                     \
      PCI_REQUESTER_ID_BASE, PCI_REQUESTER_ID_COUNT, PCI_REQUESTER_ID_BASE,    \
      offsetof(ACPI_IORT_TABLE_STRUCTURE_NAME, smmu_node), 0)

#define IORT_DECLARE_ITS_GROUP_NODE(node_identifier, its_identifier)           \
  {                                                                           \
      IORT_DECLARE_NODE_HEADER(IORT_NODE_TYPE_ITS_GROUP,                       \
                               sizeof(XRING_IORT_ITS_GROUP_NODE), 1,          \
                               node_identifier, 0, 0),                        \
      .NumOfITS = IORT_ITS_COUNT,                                              \
      .GicITSIdentifiers = {its_identifier},                                   \
  }

#define IORT_DECLARE_SMMU_V3_NODE(node_struct, node_identifier, mapping_count, \
                  base_address, event_gsiv, gerr_gsiv,        \
                  sync_gsiv)                                  \
  {                                                                           \
      IORT_DECLARE_NODE_HEADER(IORT_NODE_TYPE_SMMU_V3,                         \
                               sizeof(node_struct), 4, node_identifier,        \
                 mapping_count, offsetof(node_struct,            \
                             IdMappings)),           \
      .BaseAddress = base_address,                                             \
      .Flags = 0,                                                              \
      .Reserved = 0,                                                           \
      .VATOSAddress = 0,                                                       \
      .Model = IORT_SMMU_V3_MODEL_GENERIC_SMMU_V3,                             \
      .Event = event_gsiv,                                                     \
      .PRI = 0,                                                                \
      .GERR = gerr_gsiv,                                                       \
      .Sync = sync_gsiv,                                                       \
      .ProximityDomain = 0,                                                    \
      .DeviceIDMappingIndex = 0,                                               \
  }

#define IORT_DECLARE_PCI_ROOT_COMPLEX_NODE(node_struct, node_identifier,       \
                       mapping_count, segment_number,      \
                       smmu_node)                          \
  {                                                                           \
      .PciRootComplexNode = {                                                  \
          IORT_DECLARE_NODE_HEADER(IORT_NODE_TYPE_ROOT_COMPLEX,                \
                                   sizeof(node_struct), 4, node_identifier,    \
                   mapping_count,                              \
                   offsetof(node_struct, IdMappings)),         \
          .MemAccessProps = {                                                  \
              .CCA = 1,                                                        \
              .AH = 0,                                                         \
              .Reserved = 0,                                                   \
              .MAF = IORT_MEMORY_ACCESS_FLAG_CPM,                              \
          },                                                                   \
          .ATSAttribute = 0,                                                   \
          .PCISegmentNumber = segment_number,                                  \
          .MemoryAddressSizeLimit = 64,                                        \
          .PASIDCapabilities = IORT_PCI_ROOT_COMPLEX_PASID_NOT_SUPPORTED,      \
          .Reserved = 0,                                                       \
          .Flags = 0,                                                          \
      },                                                                       \
      .IdMappings = {IORT_DECLARE_PCI_TO_SMMU_MAPPING(smmu_node)},             \
  }

typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  UINT32 NumOfITS;
  UINT32 GicITSIdentifiers[IORT_ITS_COUNT];
} __attribute__((packed)) XRING_IORT_ITS_GROUP_NODE;
_Static_assert(sizeof(XRING_IORT_ITS_GROUP_NODE) == 24,
               "XRING_IORT_ITS_GROUP_NODE size incorrect");

#define XRING_IORT_DECLARE_SMMU_V3_NODE_TYPE(type_name, mapping_count)         \
  typedef struct {                                                             \
    IORT_SMMU_V3_NODE SmmuV3Node;                                              \
    IORT_ID_MAPPING_FORMAT IdMappings[mapping_count];                          \
  } __attribute__((packed)) type_name;                                         \
  _Static_assert(sizeof(type_name) ==                                           \
                     sizeof(IORT_SMMU_V3_NODE) +                               \
                         sizeof(IORT_ID_MAPPING_FORMAT) * mapping_count,       \
                 #type_name " size incorrect")

XRING_IORT_DECLARE_SMMU_V3_NODE_TYPE(XRING_IORT_MEDIA1_SMMU_NODE,
                                     MEDIA1_SMMU_MAPPING_COUNT);
XRING_IORT_DECLARE_SMMU_V3_NODE_TYPE(XRING_IORT_MEDIA2_SMMU_NODE,
                                     MEDIA2_SMMU_MAPPING_COUNT);
XRING_IORT_DECLARE_SMMU_V3_NODE_TYPE(XRING_IORT_NPU_SMMU_NODE,
                                     NPU_SMMU_MAPPING_COUNT);
XRING_IORT_DECLARE_SMMU_V3_NODE_TYPE(XRING_IORT_PCIE_SMMU_NODE,
                                     PCIE_SMMU_MAPPING_COUNT);

#define XRING_IORT_DECLARE_PCI_ROOT_COMPLEX_NODE_TYPE(type_name)               \
  typedef struct {                                                             \
    IORT_PCI_ROOT_COMPLEX_NODE PciRootComplexNode;                             \
    IORT_ID_MAPPING_FORMAT IdMappings[PCI_ROOT_COMPLEX_MAPPING_COUNT];         \
  } __attribute__((packed)) type_name;                                         \
  _Static_assert(sizeof(type_name) ==                                           \
                     sizeof(IORT_PCI_ROOT_COMPLEX_NODE) +                      \
                         sizeof(IORT_ID_MAPPING_FORMAT) *                      \
                             PCI_ROOT_COMPLEX_MAPPING_COUNT,                   \
                 #type_name " size incorrect")

XRING_IORT_DECLARE_PCI_ROOT_COMPLEX_NODE_TYPE(XRING_IORT_PCI_ROOT_COMPLEX_NODE);

typedef struct {
  ACPI_TABLE_HEADER Header;
  IORT_HEADER_EXTRA_DATA IortHeaderExtraData;
  XRING_IORT_ITS_GROUP_NODE ItsGroupNode;
  XRING_IORT_MEDIA1_SMMU_NODE Media1SmmuNode;
  XRING_IORT_MEDIA2_SMMU_NODE Media2SmmuNode;
  XRING_IORT_NPU_SMMU_NODE NpuSmmuNode;
  XRING_IORT_PCIE_SMMU_NODE PcieSmmuNode;
  XRING_IORT_PCI_ROOT_COMPLEX_NODE Pcie0RootComplexNode;
  XRING_IORT_PCI_ROOT_COMPLEX_NODE Pcie1RootComplexNode;
} __attribute__((packed)) ACPI_IORT_TABLE_STRUCTURE_NAME;

IORT_DEFINE_WITH_MAGIC;

IORT_START{
    ACPI_DECLARE_TABLE_HEADER(ACPI_IORT_SIGNATURE,
                              ACPI_IORT_TABLE_STRUCTURE_NAME,
                              ACPI_IORT_REVISION),
    .IortHeaderExtraData = {
        .NumOfNodes = IORT_NODE_COUNT,
        .OffsetToNodeArray = sizeof(ACPI_TABLE_HEADER) +
                             sizeof(IORT_HEADER_EXTRA_DATA),
        .Reserved = 0,
    },
    .ItsGroupNode = IORT_DECLARE_ITS_GROUP_NODE(0, IORT_ITS_IDENTIFIER),
    .Media1SmmuNode = {
      .SmmuV3Node = IORT_DECLARE_SMMU_V3_NODE(
            XRING_IORT_MEDIA1_SMMU_NODE, 1, MEDIA1_SMMU_MAPPING_COUNT,
            MEDIA1_SMMU_BASE_ADDRESS,
        MEDIA1_SMMU_EVENT_GSIV, MEDIA1_SMMU_GERR_GSIV,
        MEDIA1_SMMU_SYNC_GSIV),
      .IdMappings = {
        IORT_DECLARE_SMMU_TO_ITS_MAPPING(0x02, 1),
        IORT_DECLARE_SMMU_TO_ITS_MAPPING(0x20, 1),
      },
    },
    .Media2SmmuNode = {
      .SmmuV3Node = IORT_DECLARE_SMMU_V3_NODE(
            XRING_IORT_MEDIA2_SMMU_NODE, 2, MEDIA2_SMMU_MAPPING_COUNT,
            MEDIA2_SMMU_BASE_ADDRESS,
        MEDIA2_SMMU_EVENT_GSIV, MEDIA2_SMMU_GERR_GSIV,
        MEDIA2_SMMU_SYNC_GSIV),
      .IdMappings = {
        IORT_DECLARE_SMMU_TO_ITS_MAPPING(0x03, 1),
        IORT_DECLARE_SMMU_TO_ITS_MAPPING(0x21, 1),
      },
    },
    .NpuSmmuNode = {
      .SmmuV3Node = IORT_DECLARE_SMMU_V3_NODE(
            XRING_IORT_NPU_SMMU_NODE, 3, NPU_SMMU_MAPPING_COUNT,
            NPU_SMMU_BASE_ADDRESS,
        NPU_SMMU_EVENT_GSIV, NPU_SMMU_GERR_GSIV, NPU_SMMU_SYNC_GSIV),
      .IdMappings = {IORT_DECLARE_SMMU_TO_ITS_MAPPING(0x05, 9)},
    },
    .PcieSmmuNode = {
      .SmmuV3Node = IORT_DECLARE_SMMU_V3_NODE(
            XRING_IORT_PCIE_SMMU_NODE, 4, PCIE_SMMU_MAPPING_COUNT,
            PCIE_SMMU_BASE_ADDRESS,
        PCIE_SMMU_EVENT_GSIV, PCIE_SMMU_GERR_GSIV, PCIE_SMMU_SYNC_GSIV),
      .IdMappings = {IORT_DECLARE_SMMU_TO_ITS_MAPPING(0x22, 1)},
    },
    .Pcie0RootComplexNode = IORT_DECLARE_PCI_ROOT_COMPLEX_NODE(
      XRING_IORT_PCI_ROOT_COMPLEX_NODE, 5, PCI_ROOT_COMPLEX_MAPPING_COUNT,
      PCIE0_SEGMENT_NUMBER,
      PcieSmmuNode),
    .Pcie1RootComplexNode = IORT_DECLARE_PCI_ROOT_COMPLEX_NODE(
      XRING_IORT_PCI_ROOT_COMPLEX_NODE, 6, PCI_ROOT_COMPLEX_MAPPING_COUNT,
      PCIE1_SEGMENT_NUMBER,
      PcieSmmuNode),
} IORT_END;