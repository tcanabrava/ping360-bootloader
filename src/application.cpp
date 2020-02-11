#include <ping-message-common.h>
#include <ping-message-ping360.h>
#include <ping-message.h>
#include <ping-parser.h>

#include <inttypes.h>
#include <stdio.h>

extern "C" {
#include "application.h"
#include "port.h"
#include "time.h"
}

PingParser application_parser;

ping_message *application_wait_message(uint16_t id, uint32_t timeout_us) {
  application_parser.reset();

  uint32_t tstop = time_us() + timeout_us;

  while (time_us() < tstop) {
    char b;
    if (port_read(&b, 1) > 0) {
      PingParser::ParseState parseResult = application_parser.parseByte(b);
      if (parseResult == PingParser::NEW_MESSAGE) {
        if (application_parser.rxMessage.message_id() == id) {
          return &application_parser.rxMessage;
        } else {
          printf("application error: got unexpected id %d while waiting for %d\n",
                 application_parser.rxMessage.message_id(), id);
          return nullptr;
        }
      } else if (parseResult == PingParser::ERROR) {
        printf("application error: parse error while waiting for %d\n", id);
        return nullptr;
      }
    }
  }
  printf("application error: timed out waiting for %d\n", id);
  return nullptr;
}

extern "C" uint16_t ping_msg_get_length(ping_message *msg) { return msg->msgDataLength(); }

extern "C" bool application_get_protocol_version(application_protocol_version_t *protocol_version) {
  common_general_request msg;
  msg.set_requested_id(CommonId::PROTOCOL_VERSION);
  msg.updateChecksum();
  uint16_t length = ping_msg_get_length(&msg);
  port_write((uint8_t *)msg.msgData, length);
  common_protocol_version *response =
      (common_protocol_version *)application_wait_message(CommonId::PROTOCOL_VERSION, 500000);
  if (!response) {
    return false;
  }

  protocol_version->major = response->version_major();
  protocol_version->minor = response->version_minor();
  protocol_version->patch = response->version_patch();

  return true;
}

extern "C" bool application_get_device_information(application_device_information_t *device_information) {
  common_general_request msg;
  msg.set_requested_id(CommonId::DEVICE_INFORMATION);
  msg.updateChecksum();
  uint16_t length = ping_msg_get_length(&msg);
  port_write((uint8_t *)msg.msgData, length);
  common_device_information *response =
      (common_device_information *)application_wait_message(CommonId::DEVICE_INFORMATION, 500000);
  if (!response) {
    return false;
  }

  device_information->type     = response->device_type();
  device_information->revision = response->device_revision();
  device_information->major    = response->firmware_version_major();
  device_information->minor    = response->firmware_version_minor();
  device_information->patch    = response->firmware_version_patch();

  return true;
}

extern "C" bool application_request_angle(uint16_t angle) {
  ping360_transducer msg;
  msg.set_angle(angle);
  msg.set_gain_setting(0);
  msg.set_sample_period(80);
  msg.set_transmit_duration(32);
  msg.set_transmit_frequency(740);
  msg.set_mode(0);
  msg.set_number_of_samples(200);
  msg.set_transmit(0);
  msg.updateChecksum();
  uint16_t length = ping_msg_get_length(&msg);
  port_write((uint8_t *)msg.msgData, length);
  ping_message *response = application_wait_message(Ping360Id::DEVICE_DATA, 4000000);
  if (!response) {
    return false;
  }

  return true;
}

extern "C" bool application_goto_bootloader() {
  ping360_reset msg;
  msg.set_bootloader(1);
  msg.updateChecksum();
  uint16_t length = ping_msg_get_length(&msg);
  port_write((uint8_t *)msg.msgData, length);
  //ping_message *response = application_wait_message(Ping360Id::DEVICE_DATA, 4000000);
  //if (!response) {
  //  return false;
  //}

  return true;
}
