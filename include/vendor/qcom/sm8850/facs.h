#pragma once
#include "table_header.h"
#include <common/facs.h>

FACS_DEFINE_TABLE;
FACS_DEFINE_WITH_MAGIC;

FACS_START{
    /* FACS does not have any header ~ */
    // Use default data here (all zero except signature, length, version)
    FACS_FILL_DEFAULT_DATA,
} FACS_END
