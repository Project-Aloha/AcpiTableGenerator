#pragma once
#include <stdint.h>

/** References
 * 1. ACPI 6.6 Spec
 *   https://uefi.org/sites/default/files/resources/ACPI_Spec_6.6.pdf
 *
 */

// ACPI Revision & Creator
#define ACPI_REVISION 1
#define ACPI_CREATOR_ID 'A', 'L', 'H', 'A'
#define ACPI_CREATOR_REVISION 0x00000001

// Header
typedef struct {
  char Signature[4]; // something like "PPTT"
  uint32_t Length;   // Length of entire table in bytes
  uint8_t Revision;  // Revision (3)
  uint8_t Checksum;  // Checksum
  char OemId[6];     // OEM ID
  char
      OemTableId[8]; // For the PPTT, the table ID is the manufacturer model ID.
  uint32_t
      OemRevision;   // OEM revision of the PPTT for the supplied OEM Table ID.
  char CreatorId[4]; // Vendor ID of utility that created the table
  uint32_t CreatorRevision; // Revision of utility that created the table
} __attribute__((packed)) ACPI_TABLE_HEADER;
_Static_assert(sizeof(ACPI_TABLE_HEADER) == 36,
               "ACPI_TABLE_HEADER size is incorrect");

#define ACPI_DECLARE_TABLE_HEADER(signature, type, revision)                   \
  .Header = {                                                                  \
      .Signature = {signature},                                                \
      .Length = sizeof(type),                                                  \
      .Revision = revision,                                                    \
      .Checksum = 0,                                                           \
      .OemId = {ACPI_TABLE_HEADER_OEM_ID},                                     \
      .OemTableId = {ACPI_TABLE_HEADER_OEM_TABLE_ID},                          \
      .OemRevision = ACPI_OEM_REVISION,                                        \
      .CreatorId = {ACPI_CREATOR_ID},                                          \
      .CreatorRevision = ACPI_CREATOR_REVISION,                                \
  }

#define ACPI_TABLE_WITH_MAGIC(type)                                            \
  typedef struct {                                                             \
    ACPI_TABLE_DEFINE_START;                                                   \
    type ACPI_TABLE;                                                           \
    ACPI_TABLE_DEFINE_END;                                                     \
  } __attribute__((packed)) ACPI_TABLE_##type##_WITH_MAGIC;

#define ACPI_TABLE_START(type)                                                 \
  ACPI_TABLE_##type##_WITH_MAGIC table_with_magic = {ACPI_TABLE_DECLARE_START, \
                                                     .ACPI_TABLE =

#define ACPI_TABLE_END(type)                                                   \
  , ACPI_TABLE_DECLARE_END,                                                    \
  }                                                                            \
  ;