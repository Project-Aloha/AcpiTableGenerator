#ifndef COMMON_PPTT_H
#define COMMON_PPTT_H

#include <stdint.h>

// ACPI表头结构 (36字节)
typedef struct {
    char     Signature[4];          // "PPTT"
    uint32_t Length;                // 表长度
    uint8_t  Revision;              // ACPI版本
    uint8_t  Checksum;              // 校验和
    char     OemId[6];              // OEM ID
    char     OemTableId[8];         // OEM Table ID
    uint32_t OemRevision;           // OEM Revision
    uint32_t CreatorId;             // Creator ID
    uint32_t CreatorRevision;       // Creator Revision
} __attribute__((packed)) AcpiTableHeader;

// PPTT处理器节点 (24字节基础大小)
typedef struct {
    uint8_t  Type;                       // 0 = Processor
    uint8_t  Length;                     // 节点长度
    uint16_t Reserved;                   // 保留
    uint32_t Flags;                      // 标志位
    uint32_t Parent;                     // 父节点偏移
    uint32_t AcpiProcessorId;            // ACPI处理器ID
    uint32_t NumberOfPrivateResources;   // 私有资源数量
} __attribute__((packed)) PpttProcessorNode;

// PPTT缓存节点 (28字节，包含CacheId)
typedef struct {
    uint8_t  Type;              // 1 = Cache
    uint8_t  Length;            // 28
    uint16_t Reserved;          // 保留
    uint32_t Flags;             // 标志位
    uint32_t NextLevelOfCache;  // 下一级缓存偏移
    uint32_t Size;              // 缓存大小
    uint32_t NumberOfSets;      // 组数
    uint8_t  Associativity;     // 相联度
    uint8_t  Attributes;        // 属性
    uint16_t LineSize;          // 行大小
    uint32_t CacheId;           // 缓存ID (ACPI 6.4+)
} __attribute__((packed)) PpttCacheNode;

// PPTT标志位定义
#define PPTT_FLAG_PHYSICAL_PACKAGE              0x00000001
#define PPTT_FLAG_ACPI_PROC_ID_VALID            0x00000002
#define PPTT_FLAG_PROCESSOR_IS_THREAD           0x00000004
#define PPTT_FLAG_NODE_IS_LEAF                  0x00000008

// Cache标志位
#define CACHE_FLAG_SIZE_VALID                   0x00000001
#define CACHE_FLAG_NUM_SETS_VALID               0x00000002
#define CACHE_FLAG_ASSOCIATIVITY_VALID          0x00000004
#define CACHE_FLAG_ALLOCATION_TYPE_VALID        0x00000008
#define CACHE_FLAG_CACHE_TYPE_VALID             0x00000010
#define CACHE_FLAG_WRITE_POLICY_VALID           0x00000020
#define CACHE_FLAG_LINE_SIZE_VALID              0x00000040
#define CACHE_FLAG_CACHE_ID_VALID               0x00000080
#define CACHE_FLAG_ALL_VALID                    0x000000FF

// Cache属性定义 (ACPI 6.4 Table 5-140)
#define CACHE_ATTR_ALLOCATION_READ              0x00
#define CACHE_ATTR_ALLOCATION_WRITE             0x01
#define CACHE_ATTR_ALLOCATION_RW                0x02
#define CACHE_ATTR_ALLOCATION_RESERVED          0x03

#define CACHE_ATTR_TYPE_DATA                    0x00
#define CACHE_ATTR_TYPE_INSTRUCTION             0x04
#define CACHE_ATTR_TYPE_UNIFIED                 0x08
#define CACHE_ATTR_TYPE_RESERVED                0x0C

#define CACHE_ATTR_WRITE_BACK                   0x00
#define CACHE_ATTR_WRITE_THROUGH                0x10

// 组合常用属性
#define CACHE_ATTR_DATA_WB          (CACHE_ATTR_ALLOCATION_RW | CACHE_ATTR_TYPE_DATA | CACHE_ATTR_WRITE_BACK)
#define CACHE_ATTR_INSTRUCTION      (CACHE_ATTR_ALLOCATION_READ | CACHE_ATTR_TYPE_INSTRUCTION | CACHE_ATTR_WRITE_BACK)
#define CACHE_ATTR_UNIFIED_WB       (CACHE_ATTR_ALLOCATION_RW | CACHE_ATTR_TYPE_UNIFIED | CACHE_ATTR_WRITE_BACK)

// ACPI Revision & Creator  
#define ACPI_REVISION_3             3
#define ACPI_CREATOR_ID             0x41484C41
#define ACPI_CREATOR_REVISION       0x00000001

// 校验和计算函数声明
uint8_t calculate_checksum(uint8_t *data, uint32_t length);

#endif // COMMON_PPTT_H
