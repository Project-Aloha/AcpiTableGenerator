/** @file
 *
 *  Copyright (c) 2021-2026 The DuoWoa authors. All rights reserved.
 *  Copyright (c) 2025-2026 The Project Aloha authors. All rights reserved.
 *
 *  MIT License
 *
 */
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define EBADF 9   // Bad file descriptor
#define EINVAL 22 // Invalid argument
#define ENOENT 2  // No such file or directory
#define ENOMEM 12 // Out of memory
#define EIO 5     // I/O error

//
//  Config info read from config file and set to kernel.
//
typedef struct {
  uint64_t StackBase;
  uint64_t StackSize;
} Config, *pConfig;

//
// Store some file information and file buffer.
//
typedef struct {
  uint8_t *fileBuffer;
  size_t fileSize;
  const char *filePath;
} FileContent, *pFileContent;

size_t get_file_size(FileContent *fileContent);
uint8_t *read_file_content(FileContent *fileContent);
int write_file_content(pFileContent fileContent);
uint8_t checksum(uint8_t *buffer, size_t length);
bool is_directory(const char *path);

#define LOG_COLOR_RESET "\x1b[0m"
#define LOG_COLOR_INFO "\x1b[97m"         /* bright white */
#define LOG_COLOR_WARN "\x1b[38;5;208m"   /* orange (256-color) */
#define LOG_COLOR_ERROR "\x1b[31m"        /* red */

#define log_info(fmt, ...)                                                  \
  printf(LOG_COLOR_INFO "[INFO] " fmt LOG_COLOR_RESET "\n", ##__VA_ARGS__)
#define log_warn(fmt, ...)                                                  \
  printf(LOG_COLOR_WARN "[WARN] " fmt LOG_COLOR_RESET "\n", ##__VA_ARGS__)
#define log_err(fmt, ...)                                                  \
  printf(LOG_COLOR_ERROR "[ERROR] " fmt " (in %s:%d)" LOG_COLOR_RESET "\n", \
         ##__VA_ARGS__, __FILE__, __LINE__)

