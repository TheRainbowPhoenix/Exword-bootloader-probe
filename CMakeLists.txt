cmake_minimum_required(VERSION 3.15)
project(ExwordBootloaderProbes C ASM)
if(NOT FXSDK_PLATFORM_LONG STREQUAL "exword")
  message(FATAL_ERROR "Use fxsdk-exword EXWORD.cmake")
endif()
include(GenerateUPDADN2)
add_executable(key_tester
  apps/key-tester/main.c
  apps/key-tester/keys.c
  common/lcd.c
  common/start.S)
target_include_directories(key_tester PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/apps/key-tester"
  "${CMAKE_CURRENT_SOURCE_DIR}/common")
target_compile_options(key_tester PRIVATE
  -mb -m4a-nofpu -Os -ffreestanding -fno-builtin -fno-pic -fno-pie
  -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables
  -ffunction-sections -fdata-sections -Wall -Wextra)
target_link_options(key_tester PRIVATE
  -mb -m4a-nofpu -nostdlib -no-pie -Wl,--build-id=none
  -Wl,--no-warn-rwx-segments
  "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/common/link.ld"
  "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/key_tester.map")
set_property(TARGET key_tester APPEND PROPERTY
  LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/common/link.ld")
generate_updadn2(TARGET key_tester
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/UPDADN2.BIN")
