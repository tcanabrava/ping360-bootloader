#pragma once

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
} application_protocol_version_t;

typedef struct {
  uint8_t type;
  uint8_t revision;
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
} application_device_information_t;

bool application_get_protocol_version(application_protocol_version_t *protocol_version);
bool application_get_device_information(application_device_information_t *device_information);
bool application_request_angle(uint16_t angle);
bool application_goto_bootloader();

#ifdef __cplusplus
}
#endif
