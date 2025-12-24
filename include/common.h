#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Size ops
#define SIZE_1B 1ULL
#define SIZE_1KB (1024ULL * SIZE_1B)
#define SIZE_1MB (1024ULL * SIZE_1KB)
#define SIZE_1GB (1024ULL * SIZE_1MB)
#define SIZE_B(x) ((x) * SIZE_1B)
#define SIZE_KB(x) ((x) * SIZE_1KB)
#define SIZE_MB(x) ((x) * SIZE_1MB)
#define SIZE_GB(x) ((x) * SIZE_1GB)

// Bit ops
#define BIT(x) (1ULL << (x))
#define GEN_MSK(high, low) (((BIT((high) + 1) - 1) & ~((BIT(low)) - 1)))

#define SET_BITS(mask, value)                                                  \
  (((unsigned long long)(value)                                                \
    << __builtin_ctzll((unsigned long long)(mask))) &                          \
   (unsigned long long)(mask))

// Standard type definitions
typedef char CHAR8;
_Static_assert(sizeof(CHAR8) == 1, "CHAR8 size is incorrect");
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef size_t UINTN;
typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;
#define TRUE true;
#define FALSE false;
typedef bool BOOLEAN;
typedef void VOID;
typedef char CHAR8;
_Static_assert(sizeof(CHAR8) == 1, "CHAR8 size incorrect");
#define STATIC static

/* For generator parsing acpi table in compiled binary */
#define ACPI_TABLE_START_MAGIC 'A', 'C', 'G', 'S'
#define ACPI_TABLE_END_MAGIC 'A', 'C', 'G', 'E'
#define ACPI_TABLE_DEFINE_START UINT8 StartMagic[4];
#define ACPI_TABLE_DEFINE_END UINT8 EndMagic[4];
#define ACPI_TABLE_DECLARE_START .StartMagic = {ACPI_TABLE_START_MAGIC}
#define ACPI_TABLE_DECLARE_END .EndMagic = {ACPI_TABLE_END_MAGIC}

// ARM GIC related conversion macros
// x means the irq number in device tree.
#define GIC_SGI(x) (x)
#define GIC_PPI(x) (16ULL + (x))
#define GIC_SPI(x) (32ULL + (x))
#define GIC_LPI(x) (8192ULL + (x))