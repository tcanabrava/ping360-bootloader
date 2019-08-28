#include "application.h"
#include "bootloader.h"
#include "pic-hex.h"
#include "port.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

const uint16_t expectedDeviceId    = 0x062f;
const uint8_t expectedVersionMajor = 2;
const uint8_t expectedVersionMinor = 1;
const uint8_t expectedVersionPatch = 2;

int main(int argc, char *argv[]) {
  printf(" ~* ping360-bootloader *~\n");

  if (argc < 3 || argc > 3) {
    printf("usage: ping360-bootloader <path to port> <path to binary hex file>\n");
    printf("ex: ping360-bootloader /dev/ttyUSB0 UWU_production.hex\n");
    return 1;
  }

  const char *portFileName     = argv[1];
  const char *firmwareFileName = argv[2];

  if (!strstr(firmwareFileName, ".hex")) {
    printf("error, firmware hex file expected (.hex extension required): %s\n", firmwareFileName);
    return 1;
  }

  printf("\nloading application from %s...", firmwareFileName);

  if (pic_hex_extract_application(firmwareFileName)) {
    printf("ok\n");
  } else {
    printf("\nerror extracting application from hex file: %s\n", firmwareFileName);
    return 1;
  }

  printf("\nopen port %s...", portFileName);

  if (port_open(portFileName) && port_set_baudrate(B115200)) {
    printf("ok\n");
  } else {
    printf("\nerror opening device port: %s\n", portFileName);
    return 1;
  }

  printf("\nfetch device id...\n");
  uint16_t id;
  if (bl_read_device_id(&id)) {
    printf(" > device id: 0x%04x <\n", id);
  } else {
    printf("error fetching device id\n");
    return 1;
  }

  printf("\nfetch version...\n");
  packet_rsp_version_t version;
  if (bl_read_version(&version)) {
    if (version.message.version_major != expectedVersionMajor ||
        version.message.version_minor != expectedVersionMinor ||
        version.message.version_patch != expectedVersionPatch) {
      printf("error, bootloader version is v%d.%d.%d, expected v%d.%d.%d\n", version.message.version_major,
             version.message.version_minor, version.message.version_patch, expectedVersionMajor, expectedVersionMinor,
             expectedVersionPatch);
      return 1;
    }

    printf(" > device type 0x%02x : hardware revision %c : bootloader v%d.%d.%d <\n", version.message.device_type,
           version.message.device_revision, version.message.version_major, version.message.version_minor,
           version.message.version_patch);

  } else {
    printf("error fetching version\n");
    return 1;
  }

  printf("\ncomparing application...\n");
  uint8_t *verify;
  for (int i = 4; i < 86; i++) {
    if (i >= 1 && i <= 3) {
      continue; // protected boot code
    }
    uint32_t offset = i * 0x400;
    bool verify_ok  = true;
    printf("compare 0x%08x: ", offset);

    if (bl_read_program_memory(&verify, offset)) {
      for (int j = 0; j < PACKET_ROW_LENGTH; j++) {
        if (verify[j] != pic_hex_application_data[i * PACKET_ROW_LENGTH + j]) {
          printf("X\nerror: program data differs at 0x%08x: 0x%02x != 0x%02x\n", i * PACKET_ROW_LENGTH + j, verify[j],
                 pic_hex_application_data[i * PACKET_ROW_LENGTH + j]);
          return 1;
        }
      }
    }
    printf("match\n");
  }

  port_close();
  return 0;
}
