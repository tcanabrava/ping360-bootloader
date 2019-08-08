#!/usr/bin/env bash

mkdir -p build
cd build
cmake ..
make ping360-bootloader
cd ..
