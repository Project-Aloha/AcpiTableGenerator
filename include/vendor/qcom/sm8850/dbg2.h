#pragma once
#include "table_header.h"
#include <common/dbg2.h>

// DEBUG UART
#define UARD_NAMESPACE_STRING "\\_SB.UARD"
// USB
#define URS0_NAMESPACE_STRING "\\_SB.URS0"

#define USB_OEM_DATA_SIZE 0x94

#define UARD_NUM_GAS 1
#define URS0_NUM_GAS 2

/* typedef */
DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(UARD, UARD_NUM_GAS,
                                        sizeof(UARD_NAMESPACE_STRING), 0);
DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(URS0HS, URS0_NUM_GAS,
                                        sizeof(URS0_NAMESPACE_STRING),
                                        USB_OEM_DATA_SIZE);
DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(URS0SS, URS0_NUM_GAS,
                                        sizeof(URS0_NAMESPACE_STRING),
                                        USB_OEM_DATA_SIZE);

DBG2_DEFINE_TABLE(DBG2_DEFINE_DEVICE_INFO_IN_TABLE(UARD);
                  DBG2_DEFINE_DEVICE_INFO_IN_TABLE(URS0HS);
                  DBG2_DEFINE_DEVICE_INFO_IN_TABLE(URS0SS););
DBG2_DEFINE_WITH_MAGIC;

/* Initialize struct */
DBG2_START{
    DBG2_DECLARE_HEADER,
    DBG2_DECLARE_HEADER_EXTRA_DATA(3), // Info count
    /* Debug UART */
    DBG2_DECLARE_QCOM_SDM845_UARD(UARD, UARD_NAMESPACE_STRING,
                                  UARD_BASE_ADDRESS),
    /* Primary Core USB HS (Synopsys)*/
    .URS0HS =
        {
            .Revision = 1,
            .Length = sizeof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS)),
            .NumOfGenericAddrRegs = URS0_NUM_GAS,
            .NamespaceString = URS0_NAMESPACE_STRING,
            .NamespaceStringLen = sizeof(URS0_NAMESPACE_STRING),
            .NamespaceStringOffset =
                offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS),
                         NamespaceString),
            .OemDataLen = USB_OEM_DATA_SIZE,
            .OemDataOffset = offsetof(
                DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS), OemData),
            .PortType = DBG2_DEBUG_PORT_TYPE_NETWORK,
            .PortSubtype = DBG2_DEBUG_PORT_SUBTYPE_USB_SYNOPSYS,
            .BaseAddrRegOffset =
                offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS),
                         BaseAddrRegister),
            .AddrSizeOffset = offsetof(
                DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS), AddressSize),
            .BaseAddrRegister =
                {
                    {
                        .AddressSpaceID = 00,
                        .RegisterBitWidth = 0x20,
                        .RegisterBitOffset = 0,
                        .AccessSize = 0x20,
                        .Address = 0xA600000,
                    },
                    {
                        .AddressSpaceID = 00,
                        .RegisterBitWidth = 0x20,
                        .RegisterBitOffset = 0,
                        .AccessSize = 0x20,
                        .Address = 0xA600000,
                    },
                },
            .AddressSize = {0xFFFFF, 0x1000},
            .OemData = {0x05, 0x00, 0x00, 0x00, 0x32, 0x58, 0x49, 0x46, 0x03,
                        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xc7,
                        0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x02, 0x00, 0x00, 0x10, 0x88, 0x0f, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00,
                        0x02, 0x00, 0x00, 0xb4, 0x88, 0x0f, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0xeb, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x43, 0x42, 0x53, 0x55},
        },
    /* Primary Core USB SS (Synopsys)*/
    .URS0SS =
        {
            .Revision = 1,
            .Length = sizeof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS)),
            .NumOfGenericAddrRegs = URS0_NUM_GAS,
            .NamespaceString = URS0_NAMESPACE_STRING,
            .NamespaceStringLen = sizeof(URS0_NAMESPACE_STRING),
            .NamespaceStringOffset =
                offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS),
                         NamespaceString),
            .OemDataLen = USB_OEM_DATA_SIZE,
            .OemDataOffset = offsetof(
                DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS), OemData),
            .PortType = DBG2_DEBUG_PORT_TYPE_NETWORK,
            .PortSubtype = DBG2_DEBUG_PORT_SUBTYPE_USB_SYNOPSYS,
            .BaseAddrRegOffset =
                offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS),
                         BaseAddrRegister),
            .AddrSizeOffset = offsetof(
                DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS), AddressSize),
            .BaseAddrRegister =
                {
                    {
                        .AddressSpaceID = 00,
                        .RegisterBitWidth = 0x20,
                        .RegisterBitOffset = 0,
                        .AccessSize = 0x20,
                        .Address = 0xA600000,
                    },
                    {
                        .AddressSpaceID = 00,
                        .RegisterBitWidth = 0x20,
                        .RegisterBitOffset = 0,
                        .AccessSize = 0x20,
                        .Address = 0xA600000,
                    },
                },
            .AddressSize = {0xFFFFF, 0x1000},
            .OemData = {0x05, 0x00, 0x00, 0x00, 0x32, 0x58, 0x49, 0x46, 0x03,
                        0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0xc7,
                        0x00, 0x00, 0xff, 0xe1, 0xff, 0xff, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x01, 0x00, 0x00, 0x10, 0x88, 0x0f, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00,
                        0x02, 0x00, 0x00, 0xb4, 0x88, 0x0f, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0xeb, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x43, 0x42, 0x53, 0x55},
        },

} DBG2_END