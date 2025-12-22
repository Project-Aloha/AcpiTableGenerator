#pragma once
#include <acpi.h>
#include <common.h>

/** Reference
    PCI Firmware Spec v3.3 from https://pcisig.com/
    This table is only required on systems with PCI Express devices.
*/

/* Table signature */
#define ACPI_MCFG_SIGNATURE 'M', 'C', 'F', 'G'
#define ACPI_MCFG_REVISION 1

#define ACPI_MCFG_TABLE_STRUCTURE_NAME MEMORY_MAPPED_CONFIGURATION_TABLE

typedef struct {
  UINT64 Reserved;
} __attribute__((packed)) MCFG_HEADER_EXTRA_DATA;

/* Body Structures */
// Memory Mapped Enhanced Configuration Space Base Address Allocation Structure
typedef struct {
  UINT64 BaseAddress;
  UINT16 PCISegmentGroupNumber;
  UINT8 StartBusNumber;
  UINT8 EndBusNumber;
  UINT32 Reserved;
} __attribute__((packed)) MCFG_MEM_MAP_EC_SPACE_STRUCTURE;
_Static_assert(sizeof(MCFG_MEM_MAP_EC_SPACE_STRUCTURE) == 16,
               "MCFG_MEM_MAP_EC_SPACE_STRUCTURE size incorrect");

/* Helper macros */
#define MCFG_DECLARE_EC_SPACE_STRUCTURE(index, seggrp_num, addr,               \
                                        start_bus_num, end_bus_num)            \
  .MemMapEcSpaceBaseAddrStructure[index] = {                                   \
      .BaseAddress = addr,                                                     \
      .PCISegmentGroupNumber = seggrp_num,                                     \
      .StartBusNumber = start_bus_num,                                         \
      .EndBusNumber = end_bus_num,                                             \
      .Reserved = 0,                                                           \
  }

#define MCFG_DEFINE_TABLE(pci_count)                                           \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    MCFG_HEADER_EXTRA_DATA MadtHeaderExtraData;                                \
    MCFG_MEM_MAP_EC_SPACE_STRUCTURE MemMapEcSpaceBaseAddrStructure[pci_count]; \
  } __attribute__((packed)) ACPI_MCFG_TABLE_STRUCTURE_NAME;

#define MCFG_DECLARE_HEADER_EXTRA_DATA                                         \
  .MadtHeaderExtraData = {                                                     \
      .Reserved = 0,                                                           \
  }

#define MCFG_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(                                                   \
      ACPI_MCFG_SIGNATURE, ACPI_MCFG_TABLE_STRUCTURE_NAME, ACPI_MCFG_REVISION)

/* MCFG Table with Magic */
#define MCFG_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_MCFG_TABLE_STRUCTURE_NAME)
#define MCFG_START ACPI_TABLE_START(ACPI_MCFG_TABLE_STRUCTURE_NAME)
#define MCFG_END ACPI_TABLE_END(ACPI_MCFG_TABLE_STRUCTURE_NAME)
