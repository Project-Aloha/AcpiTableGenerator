#pragma once
#include <acpi.h>
#include <common.h>

// Firmware ACPI Control Structure

/* Table signature */
#define ACPI_FACS_SIGNATURE 'F', 'A', 'C', 'S'
#define ACPI_FACS_REVISION 3
#define ACPI_FACS_TABLE_STRUCTURE_NAME FIRMWARE_ACPI_CONTROL_STRUCTURE

typedef struct {
  CHAR8 Signature[4];
  UINT32 Length;
  UINT32 HardwareSignature;
  UINT32 FirmwareWakingVector;
  UINT32 GlobalLock;
  UINT32 Flags;
  UINT64 X_FirmwareWakingVector;
  UINT8 Version;
  UINT8 Reserved[3];
  UINT32 OSPMFlags;
  UINT8 Reserved1[24];
} __attribute__((packed)) ACPI_FACS_TABLE;
_Static_assert(sizeof(ACPI_FACS_TABLE) == 64,
               "ACPI_FACS_TABLE size is incorrect");

// Firmware control structure feature flags
#define FACS_FLAG_S4_BIOS BIT(0)
#define FACS_FLAG_64BIT_WAKE_SUPPORTED BIT(1)
#define FACS_FLAG_RESERVED GEN_MSK(31, 2)

// OSPM Enabled firmware control structure feature flags
#define FACS_FLAG_OSPM_64BIT_WAKE BIT(0)
#define FACS_FLAG_OSPM_RESERVED GEN_MSK(31, 1)

/* Helper macros */
#define FACS_DEFINE_TABLE                                                      \
  typedef struct {                                                             \
    ACPI_FACS_TABLE FacsData;                                                  \
  } __attribute__((packed)) ACPI_FACS_TABLE_STRUCTURE_NAME;

#define FACS_FILL_DEFAULT_DATA                                                 \
  .FacsData = {                                                                \
      .Signature = {ACPI_FACS_SIGNATURE},                                      \
      .Length = sizeof(ACPI_FACS_TABLE_STRUCTURE_NAME),                        \
      .Version = ACPI_FACS_REVISION,                                           \
  }

/* FACS Table with Magic */
#define FACS_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_FACS_TABLE_STRUCTURE_NAME)
#define FACS_START ACPI_TABLE_START(ACPI_FACS_TABLE_STRUCTURE_NAME)
#define FACS_END ACPI_TABLE_END(ACPI_FACS_TABLE_STRUCTURE_NAME)