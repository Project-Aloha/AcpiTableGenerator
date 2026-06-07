#pragma once
#include "table_header.h"
#include <common/facp.h>

#define FACP_RESET_REG_ADDRESS 0xE0000928ULL
FACP_DEFINE_TABLE;
FACP_DEFINE_WITH_MAGIC;

FACP_START{
    FACP_DECLARE_HEADER,
    /*
     * All parts not assigned are 0 by default. Only assign used parts here.
     */
    .FacpDataStructure =
        {
            .Preferred_PM_Profile = FACP_PM_PROFILE_TABLET,
            .Flags =
                FACP_FLAG_HW_REDUCED_ACPI | FACP_FLAG_LOW_POWER_S0_IDLE_CAPABLE,
            .ResetReg =
                {
                    .AddressSpaceId =
                        ACPI_GAS_ADDR_SPACE_ID_EMBEDDED_CONTROLLER, // Embeded controller
                    .RegisterBitWidth = 0,
                    .RegisterBitOffset = 0,
                    .AccessSize = ACPI_GAS_ACCESS_SIZE_DWORD,
                    .Address = FACP_RESET_REG_ADDRESS,
                },
            .ResetValue = 0x01,
            .ARM_BOOT_ARCH = FACP_ARM_BOOT_ARCH_PSCI_COMPLIANCE,
            .HypervisorVendorIdentity = 0,
        },
} FACP_END