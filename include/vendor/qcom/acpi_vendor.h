#pragma once

#define ACPI_TABLE_HEADER_OEM_ID 'Q', 'C', 'O', 'M', ' ', ' ' // "QCOM"
#define ACPI_TABLE_HEADER_OEM_TABLE_ID 'Q', 'C', 'O', 'M', 'E', 'D', 'K', '2' // "QCOMEDK2"
#define ACPI_CSRT_VENDOR_ID 0x4D4F4351ULL // 'M', 'O', 'C', 'Q'
#define ACPI_CSRT_SUB_VENDOR_ID 0x0ULL

#define ACPI_FACP_HYP_VENDOR_ID 0x4D4F4351ULL // 'M','O','C','Q'

enum ACPI_CSRT_DEVICE_ID {
  DEVICE_ID_TIMER = 0x100B,
  DEVICE_ID_MISC = 0x100C,
};

#define DBG2_DECLARE_QCOM_SDM845_UARD(name, namepath, address)                 \
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
      .PortSubtype = DBG2_DEBUG_PORT_SUBTYPE_SERIAL_SDM845_7P372_MHZ_CLK,      \
      .BaseAddrRegOffset = offsetof(                                           \
          DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name), BaseAddrRegister),  \
      .AddrSizeOffset = offsetof(                                              \
          DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(name), AddressSize),       \
      .BaseAddrRegister =                                                      \
          {                                                                    \
              {                                                                \
                  .AddressSpaceID = 00,                                        \
                  .RegisterBitWidth = 0x20,                                    \
                  .RegisterBitOffset = 0,                                      \
                  .AccessSize = 0x20,                                          \
                  .Address = address,                                          \
              },                                                               \
          },                                                                   \
      .AddressSize = {0x1000},                                                 \
      .NamespaceString = namepath,                                             \
  }
