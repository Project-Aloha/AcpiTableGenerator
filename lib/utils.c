/** @file
 *
 *  Copyright (c) 2021-2026 The DuoWoa authors. All rights reserved.
 *  Copyright (c) 2025-2026 The Project Aloha authors. All rights reserved.
 *
 *  MIT License
 *
 */

#include "utils.h"
#include <string.h>
#include <sys/stat.h>

/**
 * Get file size based on given fileContent.
 *
 * @param fileContent provide filePath, will also set fileSize in it.
 * @retval 0    File does not exist.
 * @retval size_t   File size
 *
 */
size_t get_file_size(FileContent *fileContent) {
    FILE *pFile = fopen(fileContent->filePath, "r");
    if (pFile == NULL) {
        printf("Error: %s not found\n", fileContent->filePath);
        return 0;
    }
    fseek(pFile, 0, SEEK_END);
    size_t len = ftell(pFile);
    fclose(pFile);
    fileContent->fileSize = len;
    return len;
}

/**
 * Read File buffer based on given fileContent.
 *
 * @param fileContent   provide filePath, will also set fileBuffer in it.
 * @return  Buffer read from file.
 */
uint8_t *read_file_content(FileContent *fileContent) {
    if (fileContent->fileBuffer == NULL)
        return NULL;
    FILE *pFile = fopen(fileContent->filePath, "rb");
    if (pFile == NULL)
        return NULL;
    fread(fileContent->fileBuffer, fileContent->fileSize, 1, pFile);
    fclose(pFile);
    return fileContent->fileBuffer;
}

/**
 * Write buffer to filePath given by fileContent.
 *
 * @param fileContent   Contains file information.
 * @retval -EBADF   Failed to write file
 *
 */
int write_file_content(pFileContent fileContent) {
    FILE *pFile = fopen(fileContent->filePath, "wb");
    if (pFile == NULL)
        return -EBADF;
    fwrite(fileContent->fileBuffer, fileContent->fileSize, 1, pFile);
    fclose(pFile);
    return 0;
}

bool is_directory(const char *path) {
    if (path == NULL)
        return false;

    struct stat st;
    if (stat(path, &st) != 0)
        return false;

    return S_ISDIR(st.st_mode);
}

/**
 * Calculate checksum for given buffer.
 *
 * @param buffer    Buffer to calculate checksum.
 *
 */
uint8_t checksum(uint8_t *buffer, size_t length) {
    uint8_t chs = 0;
    for (size_t i = 0; i < length; i++) {
        chs += buffer[i];
    }
    return (uint8_t)(0 - chs);
}

