cmake_minimum_required(VERSION 3.12)
project(ping360-bootloader)

set (CMAKE_CXX_STANDARD 11)

set(PLATFORM_EXTENSION "")
if(DEFINED RASPBERRY_PI)
  set(PLATFORM_EXTENSION "-rpi")
  set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
  set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
  set(CINTELHEX_FLAGS "--host arm-linux-gnueabihf --build x86_64")
endif()

include_directories(
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex/include
  ${PROJECT_SOURCE_DIR}/lib/ping-cpp/src/message
  ${PROJECT_SOURCE_DIR}/src/inc
)

set(LIBCIHEX_SRC
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex/src/ihex_parse.c
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex/src/ihex_record.c
)

set(COMMON_SRC
  ${PROJECT_SOURCE_DIR}/src/application.cpp
  ${PROJECT_SOURCE_DIR}/src/bootloader.c
  ${PROJECT_SOURCE_DIR}/src/packet.c
  ${PROJECT_SOURCE_DIR}/src/pic-hex.c
  ${PROJECT_SOURCE_DIR}/src/port.c
  ${PROJECT_SOURCE_DIR}/src/time.c
)

add_executable(ping360-bootloader${PLATFORM_EXTENSION}
  ${LIBCIHEX_SRC}
  ${COMMON_SRC}
  ${PROJECT_SOURCE_DIR}/src/main.c
)

add_executable(ping360-compare${PLATFORM_EXTENSION}
  ${LIBCIHEX_SRC}
  ${COMMON_SRC}
  ${PROJECT_SOURCE_DIR}/src/compare.c
)

add_custom_target(setup-cintelhex
  cd ${PROJECT_SOURCE_DIR} && ./tools/setup-cintelhex.sh ${CINTELHEX_FLAGS}
)

add_dependencies(ping360-bootloader${PLATFORM_EXTENSION} setup-cintelhex)
add_dependencies(ping360-compare${PLATFORM_EXTENSION} setup-cintelhex)
