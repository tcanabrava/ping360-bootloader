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

  printf("\nloading configuration from %s...", firmwareFileName);

  // todo fail if doesn't exist
  if (pic_hex_extract_configuration(firmwareFileName)) {
    printf("ok\n");
  } else {
    printf("\nerror extracting configuration from hex file: %s\n", firmwareFileName);
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

  uint8_t zeros[PACKET_ROW_LENGTH];
  memset(zeros, 0xff, sizeof(zeros));
  const uint32_t bootAddress = 0x1000;
  printf("\nwipe boot address 0x%08x...", bootAddress);
  if (bl_write_program_memory(zeros, bootAddress)) {
    printf("ok\n");
  } else {
    printf("error\n");
    return 1;
  }

  printf("\nwriting application...\n");
  for (int i = 0; i < 86; i++) {
    if (i >= 1 && i <= 3) {
      continue; // protected boot code
    }
    if (i == 4) {
      continue; // we write this page last, to prevent booting after failed programming
    }

    printf("write 0x%08x: ", i * 0x400);

    if (bl_write_program_memory(pic_hex_application_data + i * PACKET_ROW_LENGTH, i * 0x400)) {
      printf("ok\n");
    } else {
      printf("error\n");
      return 1;
    }
  }

  printf("\nwrite boot address 0x%08x...", bootAddress);
  if (bl_write_program_memory(pic_hex_application_data + 4*PACKET_ROW_LENGTH, bootAddress)) {
    printf("ok\n");
  } else {
    printf("error\n");
    return 1;
  }

  printf("\nverifying application...\n");
  uint8_t *verify;
  for (int i = 4; i < 86; i++) {
    if (i >= 1 && i <= 3) {
      continue; // protected boot code
    }
    uint32_t offset = i * 0x400;
    bool verify_ok  = true;
    printf("verify 0x%08x: ", offset);

    if (bl_read_program_memory(&verify, offset)) {
      for (int j = 0; j < PACKET_ROW_LENGTH; j++) {
        if (verify[j] != pic_hex_application_data[i * PACKET_ROW_LENGTH + j]) {
          printf("X\nerror: program data differs at 0x%08x: 0x%02x != 0x%02x\n", i * PACKET_ROW_LENGTH + j, verify[j],
                 pic_hex_application_data[i * PACKET_ROW_LENGTH + j]);
          return 1;
        }
      }
    }
    printf("ok\n");
  }

  printf("\nwriting configuration...");
  if (bl_write_configuration_memory(pic_hex_configuration_data)) {
    printf("ok\n");
  } else {
    printf("error\n");
    return 1;
  }

  printf("\nstarting application...");
  if (bl_reset()) {
    printf("ok\n");
  } else {
    printf("error\n");
    return 1;
  }

  usleep(500000);

  printf("\nfetching application protocol version...");
  application_protocol_version_t protocol_version;
  if (application_get_protocol_version(&protocol_version)) {
    printf("ok\n");
    printf(" > protocol version v%d.%d.%d <\n", protocol_version.major, protocol_version.minor, protocol_version.patch);
  } else {
    printf("error\n");
    return 1;
  }

  printf("\nfetching application device information...");
  application_device_information_t device_information;
  if (application_get_device_information(&device_information)) {
    printf("ok\n");
    printf(" > device type %d : hardware revision %c : v%d.%d.%d <\n", device_information.type,
           device_information.revision, device_information.major, device_information.minor, device_information.patch);
  } else {
    printf("error\n");
    return 1;
  }

  printf("\ncommanding head angle...");
  if (application_request_angle(0)) {
    printf("ok\n");
  } else {
    printf("error\n");
    return 1;
  }

  port_close();
  return 0;
}
