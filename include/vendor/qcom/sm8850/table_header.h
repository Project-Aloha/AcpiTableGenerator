#include <acpi_vendor.h>

#define ACPI_OEM_REVISION 0x8850

/* Platform specific configuration */
#define NUM_CORES 8
#define NUM_CLUSTERS 2
#define NUM_SYSTEM 1
#define NUM_CLUSTER_0_CORES 6
#define NUM_CLUSTER_1_CORES 2

#define L1_CACHES_COUNT 2
#define L2_CACHES_COUNT 1
#define L3_CACHES_COUNT 0

#define UARD_BASE_ADDRESS 0xA9C000ULL
#define UARD_GIC_SPI_INTERRUPT_NUMBER GIC_SPI(0x343) // GIC SPI 0x363
