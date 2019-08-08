#include "port.h"

#include <errno.h> // Error integer and strerror() function
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions

// this is the interface of the bootloader
// re-implement these for your platform (ex with QSerialPort)

typedef struct {
  const char *fileName;
  FILE *_handle;
} port_t;

port_t portInfo = {NULL, NULL};

int port_open(const char *fileName) {
  portInfo.fileName = fileName;
  portInfo._handle  = fopen(portInfo.fileName, "a+");
}

int port_read(char *data, int nBytes) {
  int result = fread(data, 1, nBytes, portInfo._handle);

  if (result == 0) {
    fclose(portInfo._handle);
    portInfo._handle = fopen(portInfo.fileName, "a+");
  }
  return result;
}

int port_close() {
  if (portInfo._handle) {
    fclose(portInfo._handle);
  }
  portInfo.fileName = NULL;
  portInfo._handle  = NULL;
}

bool port_set_baudrate(int baudrate) {
  int result;

  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  // Read in existing settings, and handle any error
  if (tcgetattr(fileno(portInfo._handle), &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
  }

  tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag |= CS8;            // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty.c_lflag = 0;

  tty.c_iflag = 0;
  tty.c_iflag &= (IGNBRK | IGNPAR | INPCK);

  tty.c_cc[VTIME] = 1; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN]  = 0;

  // Set in/out baud rate
  cfsetispeed(&tty, baudrate);
  cfsetospeed(&tty, baudrate);

  // Save tty settings, also checking for error
  if (tcsetattr(fileno(portInfo._handle), TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return false;
  }

  return true;
}

int port_write(const uint8_t *buffer, int nBytes) {
  int bytes = fwrite(buffer, 1, nBytes, portInfo._handle);
  fflush(portInfo._handle);
  return bytes;
}
