#pragma once

#include <inttypes.h>
#include <stdbool.h>

// Contains POSIX terminal control definitions
// needed for baudrate definitions ie B115200
#include <termios.h>

// this is the interface of the bootloader
// re-implement these for your platform (ex with QSerialPort)
int port_open(const char *fileName);
int port_close();

int port_read(char *data, int nBytes);
int port_write(const uint8_t *buffer, int nBytes);

bool port_set_baudrate(int baudrate);
