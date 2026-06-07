#pragma once

#define ACPI_TABLE_HEADER_OEM_ID 'X', 'R', 'T', 'K', ' ', ' ' // "XRTK  "
#define ACPI_TABLE_HEADER_OEM_TABLE_ID                                         \
  'X', 'R', 'T', 'K', 'E', 'D', 'K', '2'  // "XRTKEDK2"
#define ACPI_CSRT_VENDOR_ID 0x4B545258ULL // 'K','T','R','X'
#define ACPI_CSRT_SUB_VENDOR_ID 0x0ULL

#define ACPI_FACP_HYP_VENDOR_ID 0x4B545258ULL // 'K','T','R','X'

enum ACPI_CSRT_DEVICE_ID {
  DEVICE_ID_TIMER = 0x100B,
  DEVICE_ID_MISC = 0x100C,
};

#define DBG2_DECLARE_XRTK_DW_UARD(name, namepath, address)                     \
  .UARD = {                                                                    \
      .Revision = 1,                                                           \
      .Length = sizeof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name)),       \
      .NumOfGenericAddrRegs = 1,                                               \
      .NamespaceStringLen = sizeof(namepath),                                  \
      .NamespaceStringOffset = offsetof(                                       \
          DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name), NamespaceString),   \
      .OemDataLen = 0,                                                         \
      .OemDataOffset = 0,                                                      \
      .PortType = DBG2_DEBUG_PORT_TYPE_SERIAL,                                 \
      .PortSubtype = DBG2_DEBUG_PORT_SUBTYPE_SERIAL_16550_COMPATIBLE_WITH_PARAS_DEF_IN_GAS,      \
      .BaseAddrRegOffset = offsetof(                                           \
          DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name), BaseAddrRegister),  \
      .AddrSizeOffset = offsetof(                                              \
          DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name), AddressSize),       \
      .BaseAddrRegister =                                                      \
          {                                                                    \
              {                                                                \
                  .AddressSpaceID = 0x0,                                     \
                  .RegisterBitWidth = 0x20,                                    \
                  .RegisterBitOffset = 0,                                      \
                  .AccessSize = 0x03,                                          \
                  .Address = address,                                          \
              },                                                               \
          },                                                                   \
      .AddressSize = {0x1000},                                                 \
      .NamespaceString = namepath,                                             \
  }

/** PortType in dbg2 device info for usb controllers.
  Reference to:
  \Windows Kits\10\Debuggers\ddk\samples\kdnet\usb\qualcomm\kdqcom\kdextension.c
*/
enum {
  DBG2_QCOM_DEBUG_PORT_SUBTYPE_CHIPIDEA_USBFN = 1,
  DBG2_QCOM_DEBUG_PORT_SUBTYPE_CHIPIDEA_AX88772 = 2,
  DBG2_QCOM_DEBUG_PORT_SUBTYPE_CHIPIDEA_USBFNB = 3,
  DBG2_QCOM_DEBUG_PORT_SUBTYPE_SYNOPSYS_USBFN = 4,
  DBG2_QCOM_DEBUG_PORT_SUBTYPE_SYNOPSYS_USBFNB = 5,
};