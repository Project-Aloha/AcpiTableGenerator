#pragma once
#include <acpi.h>
#include <common.h>

/* Table signature */
#define ACPI_MADT_SIGNATURE 'A', 'P', 'I', 'C'
#define ACPI_MADT_REVISION 5

#define ACPI_MADT_TABLE_STRUCTURE_NAME MULTI_APIC_DESCRIPTION_TABLE

// #define GICC_HAS_TRBE_INTERRUPT

typedef struct {
  UINT32 LocalInterruptControllerAddress;
  UINT32 Flags; // Multiple MADT flags
} __attribute__((packed)) MADT_HEADER_EXTRA_DATA;

#define MADT_FLAG_PCAT_COMPAT BIT(0)
#define MADT_FLAG_RESERVED_MASK GEN_MSK(31, 1)

/* Body Structures */
// GICC Structure (Table 5.36)
typedef struct {
  UINT8 Type;                // Should be 0xB for GICC
  UINT8 Length;              // Should be 82
  UINT16 Reserved;           // Must be zero
  UINT32 CPUInterfaceNumber; // GIC's CPU Interface Number
  UINT32 ACPIProcessorUID;
  UINT32 Flags; // See GICC CPU Interface Flags
  UINT32 ParkingProtocolVersion;
  UINT32 PerformanceInterruptGSI;
  UINT64 ParkedAddress;
  UINT64 PhysicalBaseAddress;
  UINT64 GICV;
  UINT64 GICH;
  UINT32 VGICMaintenanceInterrupt;
  UINT64 GICRBaseAddress;
  UINT64 MPIDR;
  UINT8 ProcessorPowerEfficiencyClass;
  UINT8 Reserved2;
  UINT16 SpeOverflowInterrupt;
#ifdef GICC_HAS_TRBE_INTERRUPT
  // Introduced in ACPI 6.5
  // Windows seems do not support this arg currently
  UINT16 TRBEInterrupt;
#endif
} __attribute__((packed)) MADT_GICC_STRUCTURE;
#ifdef GICC_HAS_TRBE_INTERRUPT
_Static_assert(sizeof(MADT_GICC_STRUCTURE) == 82,
               "MADT_GICC_STRUCTURE size is incorrect");
#else
_Static_assert(sizeof(MADT_GICC_STRUCTURE) == 80,
               "MADT_GICC_STRUCTURE size is incorrect");
#endif

// GICC CPU Interface Flags
#define MADT_GICC_FLAG_ENABLED BIT(0)
#define MADT_GICC_FLAG_PERFORMANCE_INTERRUPT_MODE BIT(1)
#define MADT_GICC_FLAG_VIRTUAL_VGIC_MAINTENANCE_INTERRUPT_MODE BIT(2)
#define MADT_GICC_FLAG_ONLINE_CAPABLE BIT(3)
#define MADT_GICC_FLAG_GICR_NON_COHERENT BIT(4)
#define MADT_GICC_FLAG_GICR_RESERVED GEN_MSK(31, 5)

// GICD Structure (Table 5.38)
typedef struct {
  UINT8 Type;      // 0xC for GICD
  UINT8 Length;    // Should be 24
  UINT16 Reserved; // Must be zero
  UINT32 GICID;
  UINT64 PhysicalBaseAddress;
  UINT32 SystemVectorBase;
  UINT8 GICVersion;
  UINT8 Reserved2[3];
} __attribute__((packed)) MADT_GICD_STRUCTURE;
_Static_assert(sizeof(MADT_GICD_STRUCTURE) == 24,
               "MADT_GICD_STRUCTURE size is incorrect");

enum MADT_GICD_GIC_VERSION {
  GIC_V1 = 0x1,
  GIC_V2 = 0x2,
  GIC_V3 = 0x3,
  GIC_V4 = 0x4,
  GIC_INVALID = 0xFF,
};

// GIC MSI Frame Structure (Table 5.39)
typedef struct {
  UINT8 Type;      // 0xD for GIC MSI Frame structure
  UINT8 Length;    // Should be 24
  UINT16 Reserved; // Must be zero
  UINT32 GICMSRFrameID;
  UINT64 PhysicalBaseAddress;
  UINT32 Flags;
  UINT16 SPICount;
  UINT16 SPIBase;
} __attribute__((packed)) MADT_GIC_MSI_FRAME_STRUCTURE;
_Static_assert(sizeof(MADT_GIC_MSI_FRAME_STRUCTURE) == 24,
               "MADT_GIC_MSI_FRAME_STRUCTURE size is incorrect");

// GIC MSI Frame Flags (Table 5.40)
#define MADT_GIC_MSI_FRAME_FLAG_SPI_COUNT_BASE_SELECT BIT(0)
#define MADT_GIC_MSI_FRAME_FLAG_RESERVED GEN_MSK(31, 1)

// GICR Structure (Table 5.41)
typedef struct {
  UINT8 Type;   // 0xE for GICR Structure
  UINT8 Length; // Should be 16
  UINT8 Flags;
  UINT8 Reserved;
  UINT64 DiscoveryRangeBaseAddress;
  UINT32 DiscoveryRangeLength;
} __attribute__((packed)) MADT_GICR_STRUCTURE;
_Static_assert(sizeof(MADT_GICR_STRUCTURE) == 16,
               "MADT_GICR_STRUCTURE size is incorrect");

// GICR Flags (Table 5.42)
#define MADT_GICR_FLAG_NON_COHERENT BIT(0)
#define MADT_GICR_FLAG_RESERVED GEN_MSK(7, 1)

// GIC ITS Structure (Table 5.43)
typedef struct {
  UINT8 Type;   // 0xF for GIC ITS Structure
  UINT8 Length; // Should be 20
  UINT8 Flags;
  UINT8 Reserved;
  UINT32 GICITSID;
  UINT64 PhysicalBaseAddress;
  UINT32 Reserved2;
} __attribute__((packed)) MADT_GIC_ITS_STRUCTURE;
_Static_assert(sizeof(MADT_GIC_ITS_STRUCTURE) == 20,
               "MADT_GIC_ITS_STRUCTURE size is incorrect");

// GIC ITS Flags (Table 5.44)
#define MADT_GIC_ITS_FLAG_GIC_ITS_NON_COHERENT BIT(0)
#define MADT_GIC_ITS_FLAG_RESERVED GEN_MSK(7, 1)

// Helper macros fill madt define table
#ifndef NUM_CLUSTER_0_CORES
#define NUM_CLUSTER_0_CORES 0
#endif
#ifndef NUM_CLUSTER_1_CORES
#define NUM_CLUSTER_1_CORES 0
#endif
#ifndef NUM_CLUSTER_2_CORES
#define NUM_CLUSTER_2_CORES 0
#endif
#ifndef NUM_CLUSTER_3_CORES
#define NUM_CLUSTER_3_CORES 0
#endif

#define CPUID_TO_CLUSTER(cpu)                                                  \
  (((cpu) < NUM_CLUSTER_0_CORES)                                               \
       ? 0                                                                     \
       : (((cpu) < NUM_CLUSTER_0_CORES + NUM_CLUSTER_1_CORES)                  \
              ? 1                                                              \
              : (((cpu) < NUM_CLUSTER_0_CORES + NUM_CLUSTER_1_CORES +          \
                              NUM_CLUSTER_2_CORES)                             \
                     ? 2                                                       \
                     : (((cpu) < NUM_CLUSTER_0_CORES + NUM_CLUSTER_1_CORES +   \
                                     NUM_CLUSTER_2_CORES +                     \
                                     NUM_CLUSTER_3_CORES)                      \
                            ? 3                                                \
                            : 0))))

#define MADT_DECLARE_HEADER_EXTRA_DATA(local_intc_addr, flags)                 \
  .MadtHeaderExtraData = {                                                     \
      .LocalInterruptControllerAddress = local_intc_addr,                      \
      .Flags = flags,                                                          \
  }

#define MADT_DECLARE_GICD_STRUCTURE(base_addr, version)                        \
  .GicDStructure = {                                                           \
      .Type = 0xC,                                                             \
      .Length = sizeof(MADT_GICD_STRUCTURE),                                   \
      .Reserved = 0,                                                           \
      .GICID = 0,                                                              \
      .PhysicalBaseAddress = base_addr,                                        \
      .SystemVectorBase = 0,                                                   \
      .GICVersion = version,                                                   \
      .Reserved2 = {0},                                                        \
  }

#define MADT_DECLARE_GIC_ITS_STRUCTURE(index, base_addr, flags)                \
  .GicItsStructures[index] = {                                                 \
      .Type = 0xF,                                                             \
      .Length = sizeof(MADT_GIC_ITS_STRUCTURE),                                \
      .Flags = flags,                                                          \
      .Reserved = 0,                                                           \
      .GICITSID = 0,                                                           \
      .PhysicalBaseAddress = base_addr,                                        \
      .Reserved2 = 0,                                                          \
  }

#ifdef GICC_HAS_TRBE_INTERRUPT
#define MADT_DECLARE_GICC_STRUCTURE(index, cpu_id, mpidr)                      \
  .GiccStructures[index] = {                                                   \
      .Type = 0xB,                                                             \
      .Length = sizeof(MADT_GICC_STRUCTURE),                                   \
      .Reserved = 0,                                                           \
      .CPUInterfaceNumber = cpu_id,                                            \
      .ACPIProcessorUID = cpu_id,                                              \
      .Flags = MADT_GICC_FLAG_ENABLED,                                         \
      .ParkingProtocolVersion = 0,                                             \
      .PerformanceInterruptGSI = GICC_PERFORMANCE_INTERRUPT_GSI,               \
      .ParkedAddress = 0,                                                      \
      .PhysicalBaseAddress = 0,                                                \
      .GICV = 0,                                                               \
      .GICH = 0,                                                               \
      .VGICMaintenanceInterrupt = GICC_VGIC_MAINTENANCE_INTERRUPT,             \
      .GICRBaseAddress = GICR_BASE_ADDRESS + GICR_STRIDE * cpu_id,             \
      .MPIDR = mpidr,                                                          \
      .ProcessorPowerEfficiencyClass = CPUID_TO_CLUSTER(cpu_id),               \
      .Reserved2 = 0,                                                          \
      .SpeOverflowInterrupt = 0,                                               \
      .TRBEInterrupt = 0,                                                      \
  }
#else
#define MADT_DECLARE_GICC_STRUCTURE(index, cpu_id, mpidr)                      \
  .GiccStructures[index] = {                                                   \
      .Type = 0xB,                                                             \
      .Length = sizeof(MADT_GICC_STRUCTURE),                                   \
      .Reserved = 0,                                                           \
      .CPUInterfaceNumber = cpu_id,                                            \
      .ACPIProcessorUID = cpu_id,                                              \
      .Flags = MADT_GICC_FLAG_ENABLED,                                         \
      .ParkingProtocolVersion = 0,                                             \
      .PerformanceInterruptGSI = GICC_PERFORMANCE_INTERRUPT_GSI,               \
      .ParkedAddress = 0,                                                      \
      .PhysicalBaseAddress = 0,                                                \
      .GICV = 0,                                                               \
      .GICH = 0,                                                               \
      .VGICMaintenanceInterrupt = GICC_VGIC_MAINTENANCE_INTERRUPT,             \
      .GICRBaseAddress = GICR_BASE_ADDRESS + GICR_STRIDE * cpu_id,             \
      .MPIDR = mpidr,                                                          \
      .ProcessorPowerEfficiencyClass = CPUID_TO_CLUSTER(cpu_id),               \
      .Reserved2 = 0,                                                          \
      .SpeOverflowInterrupt = 0,                                               \
  }
#endif

#define MADT_DEFINE_TABLE(core_count, its_count, name)                         \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    MADT_HEADER_EXTRA_DATA MadtHeaderExtraData;                                \
    MADT_GICD_STRUCTURE GicDStructure;                                         \
    MADT_GIC_ITS_STRUCTURE GicItsStructures[its_count];                        \
    MADT_GICC_STRUCTURE GiccStructures[core_count];                            \
  } __attribute__((packed)) name;

#define MADT_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(                                                   \
      ACPI_MADT_SIGNATURE, ACPI_MADT_TABLE_STRUCTURE_NAME, ACPI_MADT_REVISION)

/* MADT Table with Magic */
#define MADT_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_MADT_TABLE_STRUCTURE_NAME)
#define MADT_START ACPI_TABLE_START(ACPI_MADT_TABLE_STRUCTURE_NAME)
#define MADT_END ACPI_TABLE_END(ACPI_MADT_TABLE_STRUCTURE_NAME)
