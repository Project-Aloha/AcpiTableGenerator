#pragma once
#include <acpi.h>
#include <common.h>

/** Microsoft Debug Port Table (DBG2)
Reference:
    https://learn.microsoft.com/en-us/windows-hardware/drivers/bringup/acpi-debug-port-table
*/

/* Table signature */
#define ACPI_DBG2_SIGNATURE 'D', 'B', 'G', '2'
#define ACPI_DBG2_REVISION 1

#define ACPI_DBG2_TABLE_STRUCTURE_NAME DEBUG_PORT_TABLE_2

typedef struct {
  UINT8 AddressSpaceID;
  UINT8 RegisterBitWidth;
  UINT8 RegisterBitOffset;
  UINT8 AccessSize;
  UINT64 Address;
} __attribute__((packed)) ACPI_GAS;
_Static_assert(sizeof(ACPI_GAS) == 12, "ACPI_GAS size incorrect");

/* Header Extra Data */
typedef struct {
  UINT32 OffsetDbgDeviceInfo;
  UINT32 NumberOfDbgDevices;
  // followed by Debug device info
} __attribute__((packed)) DBG2_HEADER_EXTRA_DATA;
_Static_assert(sizeof(DBG2_HEADER_EXTRA_DATA) == 8,
               "DBG2_HEADER_EXTRA_DATA size incorrect");

/* Header Extra Data */
/* Body Structures */
enum DBG2_DEBUG_PORT_TYPE {
  DBG2_DEBUG_PORT_TYPE_RESERVED = 0,
  /* 0-0x7FFF are reserved */
  DBG2_DEBUG_PORT_TYPE_SERIAL = 0x8000,
  DBG2_DEBUG_PORT_TYPE_1394 = 0x8001,
  DBG2_DEBUG_PORT_TYPE_USB = 0x8002,
  DBG2_DEBUG_PORT_TYPE_NETWORK = 0x8003,
  /* 0x8004 - 0xFFFF are reserved */
  DBG2_DEBUG_PORT_TYPE_NET2 = 0x8004, /* Reserved */
  DBG2_DEBUG_PORT_TYPE_RESERVED2 = 0xFFFF,
};

enum DBG2_DEBUG_PORT_SUBTYPE_SERIAL {
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_FULLY_16550_COMPATIBLE = 0,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_16550_SUBSET_COMPATIBLE_WITH_DBGP = 1,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_NAX311XE_SPI_UART = 2,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_ARM_PL011 = 3,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_MSM_8x60 = 4,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_NVIDIA_16550 = 5,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_TI_OMAP = 6,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_RESERVED = 7,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_APM88XXXX = 8,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_MSM8974 = 9,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_SAM5250 = 0xA,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_INTEL_USIF = 0xB,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_IMX_6 = 0xC,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_DEPRECATED_ARM_SBSA_2_0_ONLY = 0xD,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_ARM_SBSA_GENERIC_UART = 0xE,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_ARM_DCC = 0xF,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_BCM2835 = 0x10,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_SDM845_1P8432_MHZ_CLK = 0x11,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_16550_COMPATIBLE_WITH_PARAS_DEF_IN_GAS = 0x12,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_SDM845_7P372_MHZ_CLK = 0x13,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_INTEL_LPSS = 0x14,
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_RISC_V_SBI_CONSOLE = 0x15,
  /* 0x16 - 0xFFFF are reserved for future use */
  DBG2_DEBUG_PORT_SUBTYPE_SERIAL_RESERVED2 = 0xFFFF,
};

enum DBG2_DEBUG_PORT_SUBTYPE_1394 {
  DBG2_DEBUG_PORT_SUBTYPE_1394_STD_HOST_CONTROLLER_INTERFACE = 0,
  /* 0x1 - 0xFFFF are reserved for future use */
  DBG2_DEBUG_PORT_SUBTYPE_1394_RESERVED = 0xFFFF,
};

enum DBG2_DEBUG_PORT_SUBTYPE_USB {
  DBG2_DEBUG_PORT_SUBTYPE_USB_XHCI_COMPLIANT_CONTROLLER = 0,
  DBG2_DEBUG_PORT_SUBTYPE_USB_EHCI_COMPLIANT_CONTROLLER = 1,
  /* 0x7 - 0xFFFF are reserved for future use */
  DBG2_DEBUG_PORT_SUBTYPE_USB_RESERVED = 0xFFFF,
};

enum DBG2_DEBUG_PORT_SUBTYPE_NET {
  DBG2_DEBUG_PORT_SUBTYPE_USB_SYNOPSYS = 0x5143,
  /* Other Vendor IDs can be used */
};

/* Helper macros */
#define DEBUG_DEVICE_INFO_STRUCTURE_SIZE_CHECK(name, num_gas, ns_len,          \
                                               oemd_len)                       \
  _Static_assert(sizeof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name)) ==    \
                     22 + sizeof(ACPI_GAS) * num_gas +                         \
                         sizeof(UINT32) * num_gas + ns_len + oemd_len,         \
                 "DBG2_DEBUG_DEVICE_INFO_STRUCTURE_" #name " size incorrect");

#define DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(                               \
    name, num_of_generic_addr_regs, namespacestr_len, oemdata_len)             \
  typedef struct {                                                             \
    UINT8 Revision;                                                            \
    UINT16 Length;                                                             \
    UINT8 NumOfGenericAddrRegs;                                                \
    UINT16 NamespaceStringLen;                                                 \
    UINT16 NamespaceStringOffset;                                              \
    UINT16 OemDataLen;                                                         \
    UINT16 OemDataOffset;                                                      \
    UINT16 PortType;                                                           \
    UINT16 PortSubtype;                                                        \
    UINT16 Reserved;                                                           \
    UINT16 BaseAddrRegOffset;                                                  \
    UINT16 AddrSizeOffset;                                                     \
    ACPI_GAS BaseAddrRegister[num_of_generic_addr_regs];                       \
    UINT32 AddressSize[num_of_generic_addr_regs];                              \
    CHAR8 NamespaceString[namespacestr_len];                                   \
    UINT8 OemData[oemdata_len];                                                \
  } __attribute__((packed)) DBG2_DEBUG_DEVICE_INFO_STRUCTURE_##name;           \
  DEBUG_DEVICE_INFO_STRUCTURE_SIZE_CHECK(name, num_of_generic_addr_regs,       \
                                         namespacestr_len, oemdata_len);

#define DBG2_DEFINE_TABLE(...)                                                 \
  typedef struct {                                                             \
    ACPI_TABLE_HEADER Header;                                                  \
    DBG2_HEADER_EXTRA_DATA Dbg2HeaderExtraData;                                \
    __VA_OPT__(__VA_ARGS__)                                                    \
  } __attribute__((packed)) ACPI_DBG2_TABLE_STRUCTURE_NAME;

#define DBG2_DECLARE_HEADER_EXTRA_DATA(info_count)                             \
  .Dbg2HeaderExtraData = {                                                     \
      .OffsetDbgDeviceInfo =                                                   \
          sizeof(ACPI_TABLE_HEADER) + sizeof(DBG2_HEADER_EXTRA_DATA),          \
      .NumberOfDbgDevices = info_count,                                        \
  }

#define DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name)                        \
  DBG2_DEBUG_DEVICE_INFO_STRUCTURE_##name

#define DBG2_DEFINE_DEVICE_INFO_IN_TABLE(name)                                 \
  DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name) name;

#define DBG2_DECLARE_HEADER                                                    \
  ACPI_DECLARE_TABLE_HEADER(                                                   \
      ACPI_DBG2_SIGNATURE, ACPI_DBG2_TABLE_STRUCTURE_NAME, ACPI_DBG2_REVISION)

/* DBG2 Table with Magic */
#define DBG2_DEFINE_WITH_MAGIC                                                 \
  ACPI_TABLE_WITH_MAGIC(ACPI_DBG2_TABLE_STRUCTURE_NAME)
#define DBG2_START ACPI_TABLE_START(ACPI_DBG2_TABLE_STRUCTURE_NAME)
#define DBG2_END ACPI_TABLE_END(ACPI_DBG2_TABLE_STRUCTURE_NAME)
