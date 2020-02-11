# ping360-bootloader

A command line tool for programming the Blue Robotics Ping360 sonar.

# building

```
git clone https://github.com/bluerobotics/ping360-bootloader
cd ping360-bootloader
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

# usage

Two applications are built: `ping360-bootloader` and `ping360-compare`

`ping360-bootloader` will program a device with new firmware, and `ping360-compare` will compare the contents of flash memory on a device to the contents of a firmware binary file.

An unprogrammed device will remain in the bootloader indefinitely when it is powered on. A programmed device will remain in the bootloader for 2 seconds after being powered on, and then it will switch to the main application firmware which uses a different communication protocol (ping-protocol) than the bootloader. The main application firmware may be commanded to run the bootloader application with the `--bootloader` option.

To use `ping360-bootloader` on an unprogrammed device:

`ping360-bootloader /dev/ttyUSB0 /path/to/firmware.hex`

To use `ping360-bootloader` on a device that is already running firmware:

`ping360-bootloader /dev/ttyUSB0 /path/to/firmware.hex --bootloader`

To use `ping360-compare`:

`ping360-compare /dev/ttyUSB0 /path/to/firmware.hex'
