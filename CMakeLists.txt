cmake_minimum_required(VERSION 3.12)
project(ping360-bootloader)

set (CMAKE_CXX_STANDARD 11)

include_directories(
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex/include
  ${PROJECT_SOURCE_DIR}/lib/ping-cpp/src/message
  ${PROJECT_SOURCE_DIR}/src/inc
)

file(GLOB LIBCIHEX_SRC
  ${PROJECT_SOURCE_DIR}/lib/libcintelhex/src/*.c
)

set(COMMON_SRC
  ${PROJECT_SOURCE_DIR}/src/application.cpp
  ${PROJECT_SOURCE_DIR}/src/bootloader.c
  ${PROJECT_SOURCE_DIR}/src/packet.c
  ${PROJECT_SOURCE_DIR}/src/pic-hex.c
  ${PROJECT_SOURCE_DIR}/src/port.c
  ${PROJECT_SOURCE_DIR}/src/time.c
)

add_executable(ping360-bootloader
  ${LIBCIHEX_SRC}
  ${COMMON_SRC}
  ${PROJECT_SOURCE_DIR}/src/main.c
)

add_executable(ping360-compare
  ${LIBCIHEX_SRC}
  ${COMMON_SRC}
  ${PROJECT_SOURCE_DIR}/src/compare.c
)

add_custom_target(setup-cintelhex
  cd ${PROJECT_SOURCE_DIR} && ./tools/setup-cintelhex.sh
)

add_dependencies(ping360-bootloader setup-cintelhex)
add_dependencies(ping360-compare setup-cintelhex)
