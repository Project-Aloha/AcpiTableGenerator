#pragma once
#include <common.h>
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
  CHAR8 Signature[4]; // something like "PPTT"
  uint32_t Length;    // Length of entire table in bytes
  uint8_t Revision;   // Revision (3)
  uint8_t Checksum;   // Checksum
  CHAR8 OemId[6];     // OEM ID
  CHAR8
  OemTableId[8]; // For the PPTT, the table ID is the manufacturer model ID.
  uint32_t
      OemRevision;    // OEM revision of the PPTT for the supplied OEM Table ID.
  CHAR8 CreatorId[4]; // Vendor ID of utility that created the table
  uint32_t CreatorRevision; // Revision of utility that created the table
} __attribute__((packed)) ACPI_TABLE_HEADER;
_Static_assert(sizeof(ACPI_TABLE_HEADER) == 36,
               "ACPI_TABLE_HEADER size is incorrect");

// Generic Address Structure (GAS)
typedef struct {
  UINT8 AddressSpaceId;
  UINT8 RegisterBitWidth;
  UINT8 RegisterBitOffset;
  UINT8 AccessSize;
  UINT64 Address;
} __attribute__((packed)) ACPI_GENERIC_ADDRESS_STRUCTURE;
_Static_assert(sizeof(ACPI_GENERIC_ADDRESS_STRUCTURE) == 12,
               "ACPI_GENERIC_ADDRESS_STRUCTURE size is incorrect");
/*
  0 Undefined (legacy reasons)
  1 Byte access : 8
  2 Word access : 16
  3 DWord access: 32
  4 QWord access: 64
*/
enum GAS_ACCESS_SIZE {
  ACPI_GAS_ACCESS_SIZE_UNDEFINED = 0,
  ACPI_GAS_ACCESS_SIZE_BYTE = 1,
  ACPI_GAS_ACCESS_SIZE_WORD = 2,
  ACPI_GAS_ACCESS_SIZE_DWORD = 3,
  ACPI_GAS_ACCESS_SIZE_QWORD = 4
};

enum GAS_ADDRESS_SPACE_ID {
  ACPI_GAS_ADDR_SPACE_ID_SYSTEM_MEMORY = 0,
  ACPI_GAS_ADDR_SPACE_ID_SYSTEM_IO = 1,
  ACPI_GAS_ADDR_SPACE_ID_PCI_CONFIGURATION = 2,
  ACPI_GAS_ADDR_SPACE_ID_EMBEDDED_CONTROLLER = 3,
  ACPI_GAS_ADDR_SPACE_ID_SMBUS = 4,
  ACPI_GAS_ADDR_SPACE_ID_SYSTEM_CMOS = 5,
  ACPI_GAS_ADDR_SPACE_ID_PCI_BAR_TARGET = 6,
  ACPI_GAS_ADDR_SPACE_ID_IPMI = 7,
  ACPI_GAS_ADDR_SPACE_ID_GPIO = 8,
  ACPI_GAS_ADDR_SPACE_ID_GENERIC_SERIAL_BUS = 9,
  ACPI_GAS_ADDR_SPACE_ID_PCC = 0xA, // Platform Communications Channel
  ACPI_GAS_ADDR_SPACE_ID_PRM = 0xB, // Platform Runtime Mechanism
  /* 0xC - 0x7E: Reserved */
  ACPI_GAS_ADDR_SPACE_ID_FUNCTIONAL_FIXED_HARDWARE = 0x7F,
  /* 0x80 - 0xFF: Oem defined */
  ACPI_GAS_ADDR_SPACE_ID_OEM_DEFINED = 0xFF
};

// Helper macros
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

#define _ACPI_TABLE_WITH_MAGIC(type)                                           \
  typedef struct {                                                             \
    ACPI_TABLE_DEFINE_START;                                                   \
    type ACPI_TABLE;                                                           \
    ACPI_TABLE_DEFINE_END;                                                     \
  } __attribute__((packed)) ACPI_TABLE_##type##_WITH_MAGIC;
#define ACPI_TABLE_WITH_MAGIC(type) _ACPI_TABLE_WITH_MAGIC(type)

#define _ACPI_TABLE_START(type)                                                \
  ACPI_TABLE_##type##_WITH_MAGIC table_with_magic = {ACPI_TABLE_DECLARE_START, \
                                                     .ACPI_TABLE =
#define ACPI_TABLE_START(type) _ACPI_TABLE_START(type)

#define ACPI_TABLE_END(type)                                                   \
  , ACPI_TABLE_DECLARE_END,                                                    \
  }                                                                            \
  ;
