#!/usr/bin/env bash

export PATH=$(pwd)/arm8/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$PATH

mkdir -p build
cd build
cmake ..
make ping360-bootloader
cmake -DRASPBERRY_PI="" ..
make ping360-bootloader-rpi
cd ..
