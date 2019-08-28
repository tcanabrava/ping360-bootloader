#!/usr/bin/env bash

cd lib/libcintelhex
autoreconf -i
./configure $@
cd ../..
