#!/usr/bin/env bash

find src -name *.c -o -name *.h -o -name *.cpp | xargs clang-format -i
