/* Locate magic and extract table from compiled binaries */
#include "utils.h"
#include <acpi.h>
#include <common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  uint32_t table_size = 0;
  uint32_t table_start_offset = 0;
  uint32_t table_end_offset = 0;
  ACPI_TABLE_HEADER *table_header = NULL;
  char *output_file_path = NULL;
  FileContent output_table = {0};
  FileContent input_binary = {0};

  int ret = 0;

  // Check args, one for input binary, one for output acpi table
  if (argc != 3 && argc != 2) {
    log_warn("Usage: %s <input_binary> <output_acpi_table>", argv[0]);
    return -EINVAL;
  }

  // Open input binary file
  input_binary.filePath = argv[1];

  // Read buffer from input binary
  if (!get_file_size(&input_binary)) {
    log_err("Failed to get file size for %s", input_binary.filePath);
    return -EINVAL;
  }
  input_binary.fileBuffer = malloc(input_binary.fileSize);
  read_file_content(&input_binary);

  // Locate magic in input binary
  char table_start_magic[] = {ACPI_TABLE_START_MAGIC};
  char table_end_magic[] = {ACPI_TABLE_END_MAGIC};

  // Get offset of table start
  for (size_t i = 0; i < input_binary.fileSize - sizeof(table_start_magic);
       i++) {
    if (memcmp(input_binary.fileBuffer + i, table_start_magic,
               sizeof(table_start_magic)) == 0) {
      table_start_offset = i + sizeof(table_start_magic);
    }
  }

  // Check if table found
  if (table_start_offset == 0) {
    free(input_binary.fileBuffer);
    log_err("Table start magic not found in %s", input_binary.filePath);
    return -ENOENT;
  }

  // Get offset of table end
  for (size_t i = table_start_offset;
       i < input_binary.fileSize - sizeof(table_end_magic); i++) {
    if (memcmp(input_binary.fileBuffer + i, table_end_magic,
               sizeof(table_end_magic)) == 0) {
      table_end_offset = i;
    }
  }

  // Check if table end found
  if (table_end_offset == 0) {
    free(input_binary.fileBuffer);
    log_err("Table end magic not found in %s", input_binary.filePath);
    return -ENOENT;
  }

  // Map header
  table_header =
      (ACPI_TABLE_HEADER *)(input_binary.fileBuffer + table_start_offset);

  // Calulate and validate table size
  table_size = table_header->Length;
  if (table_size != table_end_offset - table_start_offset) {
    printf(
        "[WARN] Table size mismatch: table size in header %u, actual size %u\n",
        table_size, table_end_offset - table_start_offset);
  }

  // Calculate and correct checksum if needed
  if (memcmp(table_header->Signature, "FACS", 4) != 0)
    table_header->Checksum =
        checksum(input_binary.fileBuffer + table_start_offset, table_size);

  // Check if output file string does not exist
  if (argc == 2) {
    // Write file to current directory with default file name from input binary
    char table_name[5];
    memcpy(table_name, table_header->Signature, 4);
    table_name[4] = '\0';

    size_t out_len = sizeof(table_name) + 4; // Signature.aml + '\0'
    output_file_path = malloc(out_len);
    if (!output_file_path) {
      free(input_binary.fileBuffer);
      log_err("Failed to allocate memory for output file path");
      return -ENOMEM;
    }
    // Construct output file path
    snprintf(output_file_path, out_len, "%s.%s", table_name, "aml");
    output_table.filePath = output_file_path;
  } else if (argc >= 3 && is_directory(argv[2])) {
    // Output to specified directory with default file name from input binary
    char table_name[5];
    memcpy(table_name, table_header->Signature, 4);
    table_name[4] = '\0';
    size_t out_len =
        strlen(argv[2]) + sizeof(table_name) + 5; // dir/Signature.aml + '\0'
    output_file_path = malloc(out_len);
    if (!output_file_path) {
      free(input_binary.fileBuffer);
      log_err("Failed to allocate memory for output file path");
      return -ENOMEM;
    }
    // Remove the last '/' if exists
    if (argv[2][strlen(argv[2]) - 1] == '/')
      argv[2][strlen(argv[2]) - 1] = '\0';
    // Construct output file path
    snprintf(output_file_path, out_len, "%s/%s.%s", argv[2], table_name, "aml");
    output_table.filePath = output_file_path;
  } else {
    output_table.filePath = argv[2];
  }

  // Write table to output file
  output_table.fileSize = table_size;
  output_table.fileBuffer = malloc(output_table.fileSize);
  memcpy(output_table.fileBuffer, input_binary.fileBuffer + table_start_offset,
         output_table.fileSize);
  ret = write_file_content(&output_table);
  if (ret < 0) {
    log_err("Failed to write ACPI table to %s", output_table.filePath);
    free(input_binary.fileBuffer);
    free(output_table.fileBuffer);
    if (output_file_path)
      free(output_file_path);
    return ret;
  }

  // Success
  log_info("Table %c%c%c%c extracted to :\t%s", table_header->Signature[0],
           table_header->Signature[1], table_header->Signature[2],
           table_header->Signature[3], output_table.filePath);

  // Clean up
  free(input_binary.fileBuffer);
  free(output_table.fileBuffer);
  if (output_file_path)
    free(output_file_path);

  return ret;
}