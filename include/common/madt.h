#pragma once
#include <acpi.h>
#include <common.h>

/* Table signature */
#define ACPI_MADT_SIGNATURE 'A', 'P', 'I', 'C'

union {
    ACPI_TABLE_HEADER Header;
    struct {
        ACPI_TABLE_HEADER PlaceHolder;
        UINT64 LocalInterruptControllerAddress;
    };
};

#define MADT_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(ACPI_MADT_SIGNATURE, PROCESSOR_PROPERTIES_TOPOLOGY_TABLE)

/* MADT Table with Magic */
#define MADT_DEFINE_WITH_MAGIC ACPI_TABLE_WITH_MAGIC(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE)
#define MADT_START ACPI_TABLE_START(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE)
#define MADT_END ACPI_TABLE_END(PROCESSOR_PROPERTIES_TOPOLOGY_TABLE)