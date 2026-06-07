#pragma once
#include "table_header.h"
#include <common/iort.h>

// This one is ai generated currently and play as a placeholder.

#define IORT_ITS_COUNT 1
#define IORT_SMMU_V3_COUNT 3
#define IORT_NODE_COUNT (1 + IORT_SMMU_V3_COUNT)

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

#define IORT_DECLARE_NODE_HEADER(node_type, node_length, node_revision,        \
                                 node_identifier)                             \
  .NodeHeader = {                                                             \
      .Type = node_type,                                                       \
      .Length = node_length,                                                   \
      .Revision = node_revision,                                               \
      .Identifier = node_identifier,                                           \
      .NumOfIDMappings = 0,                                                    \
      .ReferenceToIdArray = 0,                                                 \
  }

#define IORT_DECLARE_ITS_GROUP_NODE(node_identifier, its_identifier)           \
  {                                                                           \
      IORT_DECLARE_NODE_HEADER(IORT_NODE_TYPE_ITS_GROUP,                       \
                               sizeof(XRING_IORT_ITS_GROUP_NODE), 1,          \
                               node_identifier),                              \
      .NumOfITS = IORT_ITS_COUNT,                                              \
      .GicITSIdentifiers = {its_identifier},                                   \
  }

#define IORT_DECLARE_SMMU_V3_NODE(node_identifier, base_address, event_gsiv,   \
                                  gerr_gsiv, sync_gsiv)                       \
  {                                                                           \
      IORT_DECLARE_NODE_HEADER(IORT_NODE_TYPE_SMMU_V3,                         \
                               sizeof(IORT_SMMU_V3_NODE), 4,                  \
                               node_identifier),                              \
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

typedef struct {
  IORT_NODE_FORMAT NodeHeader;
  UINT32 NumOfITS;
  UINT32 GicITSIdentifiers[IORT_ITS_COUNT];
} __attribute__((packed)) XRING_IORT_ITS_GROUP_NODE;
_Static_assert(sizeof(XRING_IORT_ITS_GROUP_NODE) == 24,
               "XRING_IORT_ITS_GROUP_NODE size incorrect");

typedef struct {
  ACPI_TABLE_HEADER Header;
  IORT_HEADER_EXTRA_DATA IortHeaderExtraData;
  XRING_IORT_ITS_GROUP_NODE ItsGroupNode;
  IORT_SMMU_V3_NODE SmmuV3Nodes[IORT_SMMU_V3_COUNT];
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
    .SmmuV3Nodes = {
        IORT_DECLARE_SMMU_V3_NODE(0, MEDIA1_SMMU_BASE_ADDRESS,
                                  MEDIA1_SMMU_EVENT_GSIV,
                                  MEDIA1_SMMU_GERR_GSIV,
                                  MEDIA1_SMMU_SYNC_GSIV),
        IORT_DECLARE_SMMU_V3_NODE(1, MEDIA2_SMMU_BASE_ADDRESS,
                                  MEDIA2_SMMU_EVENT_GSIV,
                                  MEDIA2_SMMU_GERR_GSIV,
                                  MEDIA2_SMMU_SYNC_GSIV),
        IORT_DECLARE_SMMU_V3_NODE(2, NPU_SMMU_BASE_ADDRESS,
                                  NPU_SMMU_EVENT_GSIV,
                                  NPU_SMMU_GERR_GSIV,
                                  NPU_SMMU_SYNC_GSIV),
    },
} IORT_END;