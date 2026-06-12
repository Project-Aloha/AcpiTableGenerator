#pragma once
#include "table_header.h"
#include <common/iort.h>

/*
  This table is AI-generated.
  Currently only for testing purposes.
*/

#define IORT_ITS_COUNT 1
#define IORT_NODE_COUNT 1

#define IORT_ITS_IDENTIFIER 0

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

#define IORT_DECLARE_ITS_GROUP_NODE(node_identifier, its_identifier)           \
  {                                                                           \
      IORT_DECLARE_NODE_HEADER(IORT_NODE_TYPE_ITS_GROUP,                       \
                               sizeof(XRING_IORT_ITS_GROUP_NODE), 1,          \
                               node_identifier, 0, 0),                        \
      .NumOfITS = IORT_ITS_COUNT,                                              \
      .GicITSIdentifiers = {its_identifier},                                   \
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
} IORT_END;