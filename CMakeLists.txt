cmake_minimum_required(VERSION 3.20)
project(GameCard LANGUAGES C CXX)

set(TARGET_NAME "nucleo_l053r8" CACHE STRING "Name of the target board")
set(TARGET_SRC_DIR "${CMAKE_SOURCE_DIR}/target_${TARGET_NAME}")
set(TARGET_BUILD_DIR "${CMAKE_BINARY_DIR}/${TARGET_NAME}")

set(UPLOAD_TOOL "openocd" CACHE STRING "Upload tool (openocd|st-flash)")

set(ARDUINO_CLI "arduino-cli" CACHE STRING "arduino-cli executable")
set(ARDUINO_FQBN "STMicroelectronics:stm32:Nucleo_64" CACHE STRING "FQBN of the board")
set(ARDUINO_BOARD_OPTS "pnum=NUCLEO_L053R8" CACHE STRING "Board options")
set(ARDUINO_CFG_DIR "${TARGET_SRC_DIR}/.config")
set(ARDUINO_BUILD_PATH "${TARGET_BUILD_DIR}")
set(ARDUINO_OUTPUT_DIR "${TARGET_BUILD_DIR}")

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")

# Join include flags into a single string
set(PROJECT_INCLUDE_FLAGS
  -I${SRC_DIR}
  -I${SRC_DIR}/graphics
)
string (JOIN " " PROJECT_INCLUDE_FLAGS_JOINED ${PROJECT_INCLUDE_FLAGS})

# 1) Setup step -> produce a stamp file
add_custom_command(
  OUTPUT "${ARDUINO_CFG_DIR}/.inited"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${ARDUINO_CFG_DIR}"
  COMMAND ${CMAKE_COMMAND} -E env PYTHONUNBUFFERED=1
          python "${CMAKE_SOURCE_DIR}/setup_target_dir.py"
  COMMAND ${CMAKE_COMMAND} -E touch "${ARDUINO_CFG_DIR}/.inited"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Setting up target folder and deps"
  VERBATIM
)

# 2) Compile step -> produce a stamp file
add_custom_command(
  OUTPUT "${ARDUINO_BUILD_PATH}/.built"
  DEPENDS "${ARDUINO_CFG_DIR}/.inited"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${ARDUINO_BUILD_PATH}"
  COMMAND ${ARDUINO_CLI}
          compile
          --config-dir "${ARDUINO_CFG_DIR}"
          --fqbn "${ARDUINO_FQBN}:${ARDUINO_BOARD_OPTS}"
          --build-path "${ARDUINO_BUILD_PATH}"
          --output-dir "${ARDUINO_OUTPUT_DIR}"
          --build-property "compiler.cpp.extra_flags=${PROJECT_INCLUDE_FLAGS_JOINED}"
          "${TARGET_SRC_DIR}"
  COMMAND ${CMAKE_COMMAND} -E touch "${ARDUINO_BUILD_PATH}/.built"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMENT "Compiling firmware with arduino-cli"
  VERBATIM
  COMMAND_EXPAND_LISTS
)

# 3) Phony build target that depends on the compile stamp
add_custom_target(build-fw ALL
  DEPENDS "${ARDUINO_BUILD_PATH}/.built"
)

# 4) Flash using openocd or st-link
if(UPLOAD_TOOL STREQUAL "openocd")
  add_custom_target(flash
    DEPENDS "${ARDUINO_BUILD_PATH}/.built"
    COMMAND /bin/bash -c "set -euo pipefail; ELF=\$(ls '${ARDUINO_OUTPUT_DIR}'/*.elf | head -n1); \
      echo '[FLASH] Using ELF:' \"\$ELF\"; \
      openocd -f interface/stlink.cfg -f target/stm32l0.cfg -c \"program \$ELF verify reset exit\""
    COMMENT "Flashing via OpenOCD (program + verify + reset)"
    VERBATIM
  )
elseif(UPLOAD_TOOL STREQUAL "st-flash")
  add_custom_target(flash
    DEPENDS "${ARDUINO_BUILD_PATH}/.built"
    COMMAND /bin/bash -c "set -euo pipefail; HEX=\$(ls '${ARDUINO_OUTPUT_DIR}'/*.hex | head -n1); \
      echo '[FLASH] Using HEX:' \"\$HEX\"; \
      st-flash --format ihex write \"\$HEX\" && st-flash reset"
    COMMENT "Flashing via st-flash (write + reset)"
    VERBATIM
  )
else()
  message(FATAL_ERROR "Unsupported UPLOAD_TOOL='${UPLOAD_TOOL}'. Use 'openocd' or 'st-flash'.")
endif()
