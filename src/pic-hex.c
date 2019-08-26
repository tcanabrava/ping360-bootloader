#include "pic-hex.h"

#include <cintelhex.h>
#include <stdio.h>

bool pic_hex_extract_application(const char *filename) {
  ihex_recordset_t *record_set = ihex_rs_from_file(filename);

  if (record_set == 0) {
    printf("failed to read recordset from file %s\n", filename);
    return false;
  }

  bool ret =
      pic_hex_mem_cpy(record_set, pic_hex_application_data, sizeof(pic_hex_application_data), PROGRAM_MEMORY_OFFSET);
  ihex_rs_free(record_set);
  return ret;
}

bool pic_hex_extract_configuration(const char *filename) {
  ihex_recordset_t *record_set = ihex_rs_from_file(filename);

  if (record_set == 0) {
    printf("failed to read recordset from file %s\n", filename);
    return false;
  }

  bool ret = pic_hex_mem_cpy(record_set, pic_hex_configuration_data, sizeof(pic_hex_configuration_data),
                             CONFIGURATION_MEMORY_OFFSET);
  ihex_rs_free(record_set);
  return ret;
}

/*
 * Address equations for your convenience:
 *
 * |   ihex   |    pic    |    data   |
 * | 0x000000 | 0x000000  | 0x000000  |
 * | 0x000800 | 0x000400  | 0x000600  |
 * | 0x001000 | 0x000800  | 0x000c00  |
 * | 0x001800 | 0x000c00  | 0x001200  |
 * | 0x002000 | 0x001000  | 0x001800  |
 */
bool pic_hex_mem_cpy(ihex_recordset_t *record_set, uint8_t *destination, uint32_t length, uint32_t offset) {
  uint_t i = 0; // current record #
  uint32_t record_offset, record_address = 0x00;

  // record iterator
  ihex_record_t *record;

  // zero-initialize destination data
  ihex_mem_zero(destination, length);

  do {
    int r = ihex_rs_iterate_data(record_set, &i, &record, &record_offset);

    // check error code
    if (r) {
      return false;
      // check record is valid
    } else if (record == 0) {
      // we reached the end of the list
      break;
    }

    ////////////////////////////
    // hex contains 16 bit words, addressed to 16 bit memory
    ////////////////////////////

    // get the record address
    record_address = (record_offset + record->ihr_address);

    ///////////////////////////////////////////
    // memory contains 24 bit words, addressed to 32 bit memory (address/2)
    ///////////////////////////////////////////

    record_address = record_address / 2;

    // record out of range low
    if (record_address < offset) {
      // printf("warn, record %d address %08x below offset %08x\r\n", i, record_address, offset);
      continue;
    }

    // record out of range high
    if (record_address >= offset + length) {
      // printf("warn, record %d address %08x beyond range %08x + %08x\r\n", i, record_address, offset, length);
      continue;
    }

    //////////////////////////////////////////////////
    // transmit data array contains 24 bit words *(3/2)
    //////////////////////////////////////////////////

    uint32_t destination_address = (3 * (record_address - offset)) / 2;

    // record out of range high
    if (destination_address >= length) {
      // printf("warn, destination address %08x beyond range %08x + %08x\r\n", destination_address, offset, length);
      continue;
    }

    for (int j = 0; j < record->ihr_length; j += 4) {
      uint32_t destination_offset = destination_address + j - (j / 4);

      // pointer to destination byte
      uint8_t *target = (uint8_t *)(destination + destination_offset);

      // copy a 24 bit word
      for (int l = 0; (l < 3); l++) {
        *(target++) = record->ihr_data[j + l];
      }
    }
  } while (i > 0);

  return true;
}
