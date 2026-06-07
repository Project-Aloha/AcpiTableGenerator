#pragma once
#include "table_header.h"
#include <common/gtdt.h>
#include <stddef.h>

GTDT_DEFINE_TABLE();
GTDT_DEFINE_WITH_MAGIC;

GTDT_START{
    GTDT_DECLARE_HEADER,
    .GTDTHeaderExtraData =
        {
            .CntControlBasePhyAddress = 0xFFFFFFFFFFFFFFFFULL,
            .SecureEL1TimerGSI = 0x1D,
            .SecureEL1TimerFlags = GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_ALWATS_ON_CAP,
            .NSEL1TimerGSI = 0x1E,
            .NSEL1TimerFlags = GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_ALWATS_ON_CAP,
            .VirtualEL1TimerGSI = 0x1B,
            .VirtualEL1TimerFlags = GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_ALWATS_ON_CAP,
            .EL2TimerGSI = 0x1A,
            .EL2TimerFlags = GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_ALWATS_ON_CAP,
            .CntReadBasePhyAddress = 0xFFFFFFFFFFFFFFFFULL,
        },
} GTDT_END;