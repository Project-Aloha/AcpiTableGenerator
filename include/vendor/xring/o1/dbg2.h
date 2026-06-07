#pragma once
#include "table_header.h"
#include <common/dbg2.h>
#include <common/dbg2_synopsys.h>

// DEBUG UART
#define UARD_NAMESPACE_STRING "\\_SB.UARD"
// USB
#define URS0_NAMESPACE_STRING "\\_SB.URS0"

#define USB_OEM_DATA_SIZE 0x94

#define UARD_NUM_GAS 1
// #define URS0_NUM_GAS 2

/* typedef */
DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(UARD, UARD_NUM_GAS,
                                        sizeof(UARD_NAMESPACE_STRING), 0,
                                        UINT8);
// DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(URS0HS, URS0_NUM_GAS,
//                                         sizeof(URS0_NAMESPACE_STRING), 1,
//                                         DBG2_DEVICE_OEM_DATA_SYNOPSYS);
// DBG2_DEFINE_DEBUG_DEVICE_INFO_STRUCTURE(URS0SS, URS0_NUM_GAS,
//                                         sizeof(URS0_NAMESPACE_STRING), 1,
//                                         DBG2_DEVICE_OEM_DATA_SYNOPSYS);

DBG2_DEFINE_TABLE(DBG2_DEFINE_DEVICE_INFO_IN_TABLE(UARD);
                //   DBG2_DEFINE_DEVICE_INFO_IN_TABLE(URS0HS);
                //   DBG2_DEFINE_DEVICE_INFO_IN_TABLE(URS0SS);
                );
DBG2_DEFINE_WITH_MAGIC;

/* Initialize struct */
DBG2_START{
    DBG2_DECLARE_HEADER,
    DBG2_DECLARE_HEADER_EXTRA_DATA(1), // Info count
    /* Debug UART */
    DBG2_DECLARE_XRTK_DW_UARD(UARD, UARD_NAMESPACE_STRING,
                                  UARD_BASE_ADDRESS),
    // /* Primary Core USB HS (Synopsys)*/
    // .URS0HS =
    //     {
    //         .Revision = 1,
    //         .Length = sizeof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS)),
    //         .NumOfGenericAddrRegs = URS0_NUM_GAS,
    //         .NamespaceString = URS0_NAMESPACE_STRING,
    //         .NamespaceStringLen = sizeof(URS0_NAMESPACE_STRING),
    //         .NamespaceStringOffset =
    //             offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS),
    //                      NamespaceString),
    //         .OemDataLen = USB_OEM_DATA_SIZE,
    //         .OemDataOffset = offsetof(
    //             DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS), OemData),
    //         .PortType = DBG2_DEBUG_PORT_TYPE_NETWORK,
    //         .PortSubtype = DBG2_DEBUG_PORT_SUBTYPE_USB_SYNOPSYS,
    //         .BaseAddrRegOffset =
    //             offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS),
    //                      BaseAddrRegister),
    //         .AddrSizeOffset = offsetof(
    //             DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0HS), AddressSize),
    //         .BaseAddrRegister =
    //             {
    //                 {
    //                     .AddressSpaceID = 00,
    //                     .RegisterBitWidth = 0x20,
    //                     .RegisterBitOffset = 0,
    //                     .AccessSize = 0x20,
    //                     .Address = 0xE1AC0000,
    //                 },
    //                 {
    //                     .AddressSpaceID = 00,
    //                     .RegisterBitWidth = 0x20,
    //                     .RegisterBitOffset = 0,
    //                     .AccessSize = 0x20,
    //                     .Address = 0xE1AC0000,
    //                 },
    //             },
    //         .AddressSize = {0xFFFFF, 0x1000},
    //         .OemData = {{
    //             .PortType = DBG2_QCOM_DEBUG_PORT_SUBTYPE_SYNOPSYS_USBFNB,
    //             .Magic = {DBG2_DEVICE_OEM_DATA_SYNOPSYS_MAGIC},
    //             .NumOfEntries = 3,
    //             .WriteEntries =
    //                 {
    //                     /* Phase, Offset, Val2And, Value2Or*/
    //                     DBG2_DEVICE_OEM_DATA_SYNOPSYS_ENTRY(
    //                         0x02, 0x0000c700, 0xfffffff8, 0x00000000),
    //                     DBG2_DEVICE_OEM_DATA_SYNOPSYS_ENTRY(
    //                         0x02, 0x000f8810, 0x00000000, 0x10100000),
    //                     DBG2_DEVICE_OEM_DATA_SYNOPSYS_ENTRY(
    //                         0x02, 0x000f88b4, 0x00000000, 0x00000deb),
    //                 },
    //         }},
    //     },
    // /* Primary Core USB SS (Synopsys)*/
    // .URS0SS =
    //     {
    //         .Revision = 1,
    //         .Length = sizeof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS)),
    //         .NumOfGenericAddrRegs = URS0_NUM_GAS,
    //         .NamespaceString = URS0_NAMESPACE_STRING,
    //         .NamespaceStringLen = sizeof(URS0_NAMESPACE_STRING),
    //         .NamespaceStringOffset =
    //             offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS),
    //                      NamespaceString),
    //         .OemDataLen = USB_OEM_DATA_SIZE,
    //         .OemDataOffset = offsetof(
    //             DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS), OemData),
    //         .PortType = DBG2_DEBUG_PORT_TYPE_NETWORK,
    //         .PortSubtype = DBG2_DEBUG_PORT_SUBTYPE_USB_SYNOPSYS,
    //         .BaseAddrRegOffset =
    //             offsetof(DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS),
    //                      BaseAddrRegister),
    //         .AddrSizeOffset = offsetof(
    //             DBG2_GET_DEBUG_DEVICE_INFO_STRUCTURE_NAME(URS0SS), AddressSize),
    //         .BaseAddrRegister =
    //             {
    //                 {
    //                     .AddressSpaceID = 00,
    //                     .RegisterBitWidth = 0x20,
    //                     .RegisterBitOffset = 0,
    //                     .AccessSize = 0x20,
    //                     .Address = 0xE1AC0000,
    //                 },
    //                 {
    //                     .AddressSpaceID = 00,
    //                     .RegisterBitWidth = 0x20,
    //                     .RegisterBitOffset = 0,
    //                     .AccessSize = 0x20,
    //                     .Address = 0xE1AC0000,
    //                 },
    //             },
    //         .AddressSize = {0xFFFFF, 0x1000},
    //         .OemData = {{
    //             .PortType = DBG2_QCOM_DEBUG_PORT_SUBTYPE_SYNOPSYS_USBFNB,
    //             .Magic = {DBG2_DEVICE_OEM_DATA_SYNOPSYS_MAGIC},
    //             .NumOfEntries = 3,
    //             .WriteEntries =
    //                 {
    //                     /* Phase, Offset, Val2And, Value2Or*/
    //                     DBG2_DEVICE_OEM_DATA_SYNOPSYS_ENTRY(
    //                         0x02, 0x0000c704, 0xffffe1ff, 0x00000000),
    //                     DBG2_DEVICE_OEM_DATA_SYNOPSYS_ENTRY(
    //                         0x01, 0x000f8810, 0x00000000, 0x10100000),
    //                     DBG2_DEVICE_OEM_DATA_SYNOPSYS_ENTRY(
    //                         0x02, 0x000f88b4, 0x00000000, 0x00000deb),
    //                 },
    //         }},
    //     },

} DBG2_END