#pragma once

#include <cintelhex.h>
#include <inttypes.h>
#include <stdbool.h>

// 86 pages * 512 words * 3 bytes
#define PROGRAM_MEMORY_SIZE 0x20400
#define PROGRAM_MEMORY_OFFSET 0x0

// 24 bytes configuration in separate memory region
#define CONFIGURATION_MEMORY_SIZE 24
#define CONFIGURATION_MEMORY_OFFSET 0x00F80000

uint8_t pic_hex_application_data[PROGRAM_MEMORY_SIZE];
uint8_t pic_hex_configuration_data[CONFIGURATION_MEMORY_SIZE];

bool pic_hex_mem_cpy(ihex_recordset_t *record_set, uint8_t *destination, uint32_t length, uint32_t offset);
bool pic_hex_extract_application(const char *filename);
bool pic_hex_extract_configuration(const char *filename);
