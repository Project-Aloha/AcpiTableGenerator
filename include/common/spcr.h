#pragma once
#include <acpi.h>
#include <common.h>
#include <common/dbg2.h>

/** Serial Port Console Redirection Table (SPCR)
  Reference:
    https://learn.microsoft.com/en-us/windows-hardware/drivers/bringup/serial-port-console-redirection-table
 */

/* Table signature */
#define ACPI_SPCR_SIGNATURE 'S', 'P', 'C', 'R'
// #define ACPI_SPCR_REVISION 4
#define ACPI_SPCR_REVISION 2

#define ACPI_SPCR_TABLE_STRUCTURE_NAME SERIAL_PORT_CONSOLE_REDIRECTION_TABLE

/* Header Extra Data */
typedef struct {
  UINT8 InterfaceType;
  UINT8 Reserved[3];
  ACPI_GAS BaseAddress;
  UINT8 InterruptType;
  UINT8 Irq;
  UINT32 GlobalSystemInterrupt;
  UINT8 ConfiguredBaudRate;
  UINT8 Parity;
  UINT8 StopBits;
  UINT8 FlowControl;
  UINT8 TerminalType;
  UINT8 Language;
  UINT16 PciDeviceId;
  UINT16 PciVendorId;
  UINT8 PciBusNumber;
  UINT8 PciDeviceNumber;
  UINT8 PciFunctionNumber;
  UINT32 PciFlags; // Must be 0 if it is not a PCI device
  UINT8 PciSegment;
#if ACPI_SPCR_REVISION == 4
  UINT32 UARTClockFrequency;
  UINT32 PreciseBaudRate;
  UINT16 NameSpaceStringLength;
  UINT16 NameSpaceStringOffset;
  // CHAR8 NameSpaceString[NameSpaceStringLength];
  // followed by Debug device info
#elif ACPI_SPCR_REVISION == 2
  UINT8 Reserved2[4];
#endif
} __attribute__((packed)) SPCR_HEADER_EXTRA_DATA;
#if ACPI_SPCR_REVISION == 4
_Static_assert(sizeof(SPCR_HEADER_EXTRA_DATA) == 88 - sizeof(ACPI_TABLE_HEADER),
               "SPCR_HEADER_EXTRA_DATA size incorrect");
#elif ACPI_SPCR_REVISION == 2
_Static_assert(sizeof(SPCR_HEADER_EXTRA_DATA) == 80 - sizeof(ACPI_TABLE_HEADER),
               "SPCR_HEADER_EXTRA_DATA size incorrect");
#endif

enum SPCR_INTERRUPT_TYPE {
  SPCR_INTERRUPT_TYPE_DUAL_8259_IRQ = BIT(0),
  SPCR_INTERRUPT_TYPE_IOAPIC_INTERRUPT = BIT(1),
  SPCR_INTERRUPT_TYPE_IOSAPIC_INTERRUPT = BIT(2),
  SPCR_INTERRUPT_TYPE_ARMH_GIC = BIT(3),
  SPCR_INTERRUPT_TYPE_RISCV_PLIC_APLIC = BIT(4),
  SPCR_INTERRUPT_TYPE_RESERVED2 = 0xFF,
};

enum SPCR_CONFIGURED_BAUD_RATE {
  SPCR_CONFIGURED_BAUD_RATE_RELIES_ON_DRIVER = 0,
  SPCR_CONFIGURED_BAUD_RATE_9600 = 3,
  SPCR_CONFIGURED_BAUD_RATE_19200 = 4,
  SPCR_CONFIGURED_BAUD_RATE_57600 = 6,
  SPCR_CONFIGURED_BAUD_RATE_115200 = 7,
  SPCR_CONFIGURED_BAUD_RATE_RESERVED = 0xFF,
};

enum SPCR_PARITY {
  SPCR_PARITY_NO_PARITY = 0,
  /* 1 - 255 reserved */
  SPCR_PARITY_RESERVED = 0xFF,
};

enum SPCR_STOP_BITS {
  SPCR_STOP_BITS_ONE_STOP_BIT = 1,
  SPCR_STOP_BITS_RESERVED = 0xFF
};

enum SPCR_FLOW_CONTROL {
  SPCR_FLOW_CONTROL_NONE = 0,
  SPCR_FLOW_CONTROL_DCD_REQUIRED_FOR_TRANSMIT = BIT(0),
  SPCR_FLOW_CONTROL_RTS_CTS_HARDWARE_FLOW_CONTROL = BIT(1),
  SPCR_FLOW_CONTROL_XON_XOFF_SOFTWARE_FLOW_CONTROL = BIT(2),
  /* BIT[3:7] reserved */
  SPCR_FLOW_CONTROL_RESERVED = 0xFF,
};

enum SPCR_TERMINAL_TYPE {
  SPCR_TERMINAL_TYPE_VT100 = 0,
  SPCR_TERMINAL_TYPE_EXT_VT100 = 1,
  SPCR_TERMINAL_TYPE_VT_UTF8 = 2,
  SPCR_TERMINAL_TYPE_VT_ANSI = 3,
  /* 4 - 255 reserved */
  SPCR_TERMINAL_TYPE_RESERVED = 0xFF,
};

/* Helper macros */
#if ACPI_SPCR_REVISION == 2
#define SPCR_DEFINE_TABLE                                                      \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    SPCR_HEADER_EXTRA_DATA SPCRHeaderExtraData;                                \
  } __attribute__((packed)) ACPI_SPCR_TABLE_STRUCTURE_NAME;
#elif ACPI_SPCR_REVISION == 4
#define SPCR_DEFINE_TABLE(namespace_str)                                       \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    SPCR_HEADER_EXTRA_DATA SPCRHeaderExtraData;                                \
    CHAR8 NamespaceString[sizeof(namespace_str)];                              \
  } __attribute__((packed)) ACPI_SPCR_TABLE_STRUCTURE_NAME;
#endif

#define SPCR_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(                                                   \
      ACPI_SPCR_SIGNATURE, ACPI_SPCR_TABLE_STRUCTURE_NAME, ACPI_SPCR_REVISION)

/* SPCR Table with Magic */
#define SPCR_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_SPCR_TABLE_STRUCTURE_NAME)
#define SPCR_START ACPI_TABLE_START(ACPI_SPCR_TABLE_STRUCTURE_NAME)
#define SPCR_END ACPI_TABLE_END(ACPI_SPCR_TABLE_STRUCTURE_NAME)
