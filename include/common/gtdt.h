#pragma once
#include <acpi.h>
#include <common.h>

/* Generic Timer Description Table */
#define ACPI_GTDT_SIGNATURE 'G', 'T', 'D', 'T'
#define ACPI_GTDT_REVISION 2
// #define ACPI_GTDT_REVISION 3

#define ACPI_GTDT_TABLE_STRUCTURE_NAME GENERIC_TIMER_DESCRIPTION_TABLE

/* Extra data in header */

typedef struct {
  UINT64 CntControlBasePhyAddress;
  UINT32 Reserved;
  UINT32 SecureEL1TimerGSI;
  UINT32 SecureEL1TimerFlags;
  UINT32 NSEL1TimerGSI;
  UINT32 NSEL1TimerFlags;
  UINT32 VirtualEL1TimerGSI;
  UINT32 VirtualEL1TimerFlags;
  UINT32 EL2TimerGSI;
  UINT32 EL2TimerFlags;
  UINT64 CntReadBasePhyAddress;
  UINT32 PlatformTimerCount;
  UINT32 PlatformTimerOffset;
#if ACPI_GTDT_REVISION >= 3
  UINT32 VirtualEL2TimerGSI;
  UINT32 VirtualEL2TimerFlags;
#endif
  // Platform Timer Structures
} __attribute__((packed)) GTDT_HEADER_EXTRA_DATA;
#if (ACPI_GTDT_REVISION >= 3)
_Static_assert(sizeof(GTDT_HEADER_EXTRA_DATA) ==
                   104 - sizeof(ACPI_TABLE_HEADER),
               "GTDT_HEADER_EXTRA_DATA size incorrect");
#else
_Static_assert(sizeof(GTDT_HEADER_EXTRA_DATA) == 96 - sizeof(ACPI_TABLE_HEADER),
               "GTDT_HEADER_EXTRA_DATA size incorrect");
#endif

/* Body Structures */
#define GTDT_DEFINE_TIMER_BLOCK_STRUCTURE_TYPE(name, block_timer_cnt)          \
  typedef struct {                                                             \
    UINT8 Type;                                                                \
    UINT16 Length;                                                             \
    UINT8 Reserved;                                                            \
    UINT64 GTBlockPhysicalAddress;                                             \
    UINT32 GTBlockTimerCount;                                                  \
    UINT32 GTBlockTimerOffset;                                                 \
    GTDT_BLOCK_TIMER_STRUCTURE GTBlockTimerStructure[block_timer_cnt];         \
  } __attribute__((packed)) GTDT_TIMER_BLOCK_STRUCTURE_##name;                 \
  _Static_assert(sizeof(GTDT_TIMER_BLOCK_STRUCTURE_##name) ==                  \
                     20 +                                                      \
                         block_timer_cnt * sizeof(GTDT_BLOCK_TIMER_STRUCTURE), \
                 "GTDT_TIMER_BLOCK_STRUCTURE size incorrect");

typedef struct {
  UINT8 GTFrameNumber;
  UINT8 Reserved[3];
  UINT64 CNTBaseX;
  UINT64 CNTEL0BaseX;
  UINT32 PhysicalTimerGSI;
  UINT32 PhysicalTimerFlags;
  UINT32 VirtualTimerGSI;
  UINT32 VirtualTimerFlags;
  UINT32 CommonFlags;
} __attribute__((packed)) GTDT_BLOCK_TIMER_STRUCTURE;
_Static_assert(sizeof(GTDT_BLOCK_TIMER_STRUCTURE) == 40,
               "GTDT_BLOCK_TIMER_STRUCTURE size incorrect");

#define GTDT_BLOCK_PVT_FLAG_TIMER_INTERRUPT_MODE BIT(0)
enum GTDT_BLOCK_PVT_TIMER_INTERRUPT_MODE {
  GTDT_BLOCK_PVT_TIMER_INTERRUPT_MODE_LVL_TRIGGERED = 0,
  GTDT_BLOCK_PVT_TIMER_INTERRUPT_MODE_EDGE_TRIGGERED = 1,
};
#define GTDT_BLOCK_PVT_FLAG_TIMER_INTERRUPT_POLARITY BIT(1)
enum GTDT_BLOCK_PVT_TIMER_INTERRUPT_POLARITY {
  GTDT_BLOCK_PVT_TIMER_INTERRUPT_POLARITY_ACTIVE_HIGH = 0,
  GTDT_BLOCK_PVT_TIMER_INTERRUPT_POLARITY_ACTIVE_LOW = 1,
};
#define GTDT_BLOCK_PVT_FLAG_TIMER_RESERVED GEN_MSK(31, 2)

#define GTDT_BLOCK_COMMONT_FLAG_TIMER_SECURE_TIMER BIT(0)
#define GTDT_BLOCK_COMMONT_FLAG_TIMER_ALWAYS_ON_CAP BIT(1)
#define GTDT_BLOCK_COMMONT_FLAG_TIMER_RESERVED GEN_MSK(31, 2)

#define GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_TIMER_INTERRUPT_MODE BIT(0)
enum GTDT_BLOCK_S_NS_ELX_TIMER_INTERRUPT_MODE {
  GTDT_BLOCK_S_NS_ELX_TIMER_INTERRUPT_MODE_LVL_TRIGGERED = 0,
  GTDT_BLOCK_S_NS_ELX_TIMER_INTERRUPT_MODE_EDGE_TRIGGERED = 1,
};
#define GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_TIMER_INTERRUPT_POLARITY BIT(1)
enum GTDT_BLOCK_S_NS_ELX_TIMER_INTERRUPT_POLARITY {
  GTDT_BLOCK_S_NS_ELX_TIMER_INTERRUPT_POLARITY_ACTIVE_HIGH = 0,
  GTDT_BLOCK_S_NS_ELX_TIMER_INTERRUPT_POLARITY_ACTIVE_LOW = 1,
};
#define GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_ALWATS_ON_CAP BIT(2)
enum GTDT_BLOCK_S_NS_ELX_TIMER_ALWATS_ON_CAP {
  GTDT_BLOCK_S_NS_ELX_TIMER_ALWATS_ON_CAP_NO = 0,
  GTDT_BLOCK_S_NS_ELX_TIMER_ALWATS_ON_CAP_YES = 1,
};
#define GTDT_BLOCK_S_NS_ELX_TIMER_FLAG_RESERVED GEN_MSK(31, 3)

typedef struct {
  UINT8 Type;    // 0x1 => WDT
  UINT16 Length; // 28
  UINT8 Reserved;
  UINT64 RefreshFramePhysicalAddress;
  UINT64 WatchdogControlFramePhysicalAddress;
  UINT32 WatchdogTimerGSI;
  UINT32 WatchdogTimerFlags;
} __attribute__((packed)) ACPI_GTDT_GENERIC_WDT_STRUCTURE;
_Static_assert(sizeof(ACPI_GTDT_GENERIC_WDT_STRUCTURE) == 28,
               "ACPI_GTDT_GENERIC_WDT_STRUCTURE size incorrect");

#define GTDT_WDT_FLAG_TIMER_TIMER_INTERRUPT_MODE BIT(0)
#define GTDT_WDT_FLAG_TIMER_TIMER_INTERRUPT_POLARITY BIT(1)
#define GTDT_WDT_FLAG_TIMER_SECURE_TIMER BIT(2)
#define GTDT_WDT_FLAG_TIMER_RESERVED GEN_MSK(31, 3)

/* Helper macros */
#define GTDT_DEFINE_TABLE(...)                                                 \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    GTDT_HEADER_EXTRA_DATA GTDTHeaderExtraData;                                \
    __VA_OPT__(__VA_ARGS__)                                                    \
  } __attribute__((packed)) ACPI_GTDT_TABLE_STRUCTURE_NAME;

#define GTDT_GET_TIMER_BLOCK_STRUCTURE_NAME(name)                              \
  GTDT_TIMER_BLOCK_STRUCTURE_##name

#define GTDT_DEFINE_TIMER_BLOCK_IN_TABLE(name)                                 \
  GTDT_GET_TIMER_BLOCK_STRUCTURE_NAME(name) name;

#define GTDT_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(                                                   \
      ACPI_GTDT_SIGNATURE, ACPI_GTDT_TABLE_STRUCTURE_NAME, ACPI_GTDT_REVISION)

/* GTDT Table with Magic */
#define GTDT_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_GTDT_TABLE_STRUCTURE_NAME)
#define GTDT_START ACPI_TABLE_START(ACPI_GTDT_TABLE_STRUCTURE_NAME)
#define GTDT_END ACPI_TABLE_END(ACPI_GTDT_TABLE_STRUCTURE_NAME)
