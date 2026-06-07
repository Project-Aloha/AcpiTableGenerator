#pragma once
#include "table_header.h"
#include <common/spcr.h>

/* typedef */
SPCR_DEFINE_TABLE;
SPCR_DEFINE_WITH_MAGIC;

/* Initialize struct */
SPCR_START{
    SPCR_DECLARE_HEADER,
    .SPCRHeaderExtraData =
        {
            .InterfaceType =
                DBG2_DEBUG_PORT_SUBTYPE_SERIAL_16550_COMPATIBLE_WITH_PARAS_DEF_IN_GAS,
            .BaseAddress =
                {
                    .AddressSpaceID = 0x00,   // System Memory Mapped
                    .RegisterBitWidth = 0x20, // 32 bits
                    .RegisterBitOffset = 0x00,
                    .AccessSize = 0x02, // DWORD access
                    .Address = UARD_BASE_ADDRESS,
                },
            .InterruptType = SPCR_INTERRUPT_TYPE_ARMH_GIC,
            .GlobalSystemInterrupt = UARD_GIC_SPI_INTERRUPT_NUMBER,
            .ConfiguredBaudRate = SPCR_CONFIGURED_BAUD_RATE_RELIES_ON_DRIVER,
            .Parity = SPCR_PARITY_NO_PARITY,
            .StopBits = SPCR_STOP_BITS_ONE_STOP_BIT,
            .FlowControl = SPCR_FLOW_CONTROL_NONE,
            .TerminalType = SPCR_TERMINAL_TYPE_VT_ANSI,
            .PciDeviceId = 0xFFFF,
            .PciVendorId = 0xFFFF,
        },
} SPCR_END;
