#ifndef COMMON_H
#define COMMON_H

// 通用头文件，用于包含所有公共定义

// 大小单位宏定义
#define SIZE_1B     1ULL
#define SIZE_1KB    (1024ULL * SIZE_1B)
#define SIZE_1MB    (1024ULL * SIZE_1KB)
#define SIZE_1GB    (1024ULL * SIZE_1MB)

// 参数化大小宏定义
#define SIZE_B(x)   ((x) * SIZE_1B)
#define SIZE_KB(x)  ((x) * SIZE_1KB)
#define SIZE_MB(x)  ((x) * SIZE_1MB)
#define SIZE_GB(x)  ((x) * SIZE_1GB)

#endif // COMMON_H
