#!/usr/bin/env bash

[ ! -d arm8 ] || exit 0
mkdir -p arm8
cd arm8
wget https://developer.arm.com/-/media/Files/downloads/gnu-a/8.3-2019.03/binrel/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz
tar -xf gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf.tar.xz
cd ..
export PATH=$(pwd)/arm8/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin:$PATH
