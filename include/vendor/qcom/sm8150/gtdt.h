#pragma once
#include "table_header.h"
#include <common/gtdt.h>
#include <stddef.h>

GTDT_DEFINE_TIMER_BLOCK_STRUCTURE_TYPE(GENERIC_TIMER, 1);

GTDT_DEFINE_TABLE(GTDT_DEFINE_TIMER_BLOCK_IN_TABLE(GENERIC_TIMER));
GTDT_DEFINE_WITH_MAGIC;

GTDT_START{
    GTDT_DECLARE_HEADER,
    .GTDTHeaderExtraData =
        {
            .CntControlBasePhyAddress = 0xFFFFFFFFFFFFFFFFULL,
            .SecureEL1TimerGSI = 0x11,
            .NSEL1TimerGSI = 0x12,
            .VirtualEL1TimerGSI = 0x13,
            .EL2TimerGSI = 0x10,
            .CntReadBasePhyAddress = 0xFFFFFFFFFFFFFFFFULL,
            .PlatformTimerCount = 1,
            .PlatformTimerOffset =
                sizeof(GTDT_HEADER_EXTRA_DATA) + sizeof(ACPI_TABLE_HEADER),
        },
    .GENERIC_TIMER =
        {.Type = 0, // Generic Timer Block
         .Length = sizeof(GTDT_GET_TIMER_BLOCK_STRUCTURE_NAME(GENERIC_TIMER)),
         .GTBlockPhysicalAddress = 0x17C20000,
         .GTBlockTimerCount = 1,
         .GTBlockTimerOffset =
             offsetof(GTDT_GET_TIMER_BLOCK_STRUCTURE_NAME(GENERIC_TIMER),
                      GTBlockTimerStructure),
         .GTBlockTimerStructure[0] =
             {
                 .GTFrameNumber = 0,
                 .CNTBaseX = 0x17C21000,
                 .CNTEL0BaseX = 0x17C22000,
                 .PhysicalTimerGSI = 0x28,
                 .VirtualTimerGSI = 0x26,
                 .CommonFlags = GTDT_BLOCK_COMMONT_FLAG_TIMER_ALWAYS_ON_CAP,
             }},
} GTDT_END;