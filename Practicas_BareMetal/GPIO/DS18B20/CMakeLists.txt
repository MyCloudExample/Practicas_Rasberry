# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(DS18B20 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Añadir la subcarpeta donde está la biblioteca BMP280
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../one_wire ${CMAKE_BINARY_DIR}/one_wire)

# Add executable. Default name is the project name, version 0.1

add_executable(DS18B20 DS18B20.c )

pico_set_program_name(DS18B20 "DS18B20")
pico_set_program_version(DS18B20 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(DS18B20 0)
pico_enable_stdio_usb(DS18B20 1)

# Add the standard library to the build
target_link_libraries(DS18B20
        pico_stdlib
        one_wire
        pico_time
        hardware_gpio)

# Add the standard include files to the build
target_include_directories(DS18B20 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(DS18B20)

