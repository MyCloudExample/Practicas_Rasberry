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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Test_4 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# ============ AÑADIR LA LIBRERÍA ONE_WIRE y DS18B20 ============
# Agregar la librería One_Wire (ruta relativa a la carpeta one_wire)
add_subdirectory(../one_wire ${CMAKE_CURRENT_BINARY_DIR}/one_wire)
add_subdirectory(../ds18b20 ${CMAKE_CURRENT_BINARY_DIR}/ds18b20)
# ====================================================

# Add executable. Default name is the project name, version 0.1

add_executable(Test_4 Test_4.c )

pico_set_program_name(Test_4 "Test_4")
pico_set_program_version(Test_4 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Test_4 1)
pico_enable_stdio_usb(Test_4 1)

# Add the standard library to the build
target_link_libraries(Test_4
        pico_stdlib
        one_wire
        ds18b20)

# Add the standard include files to the build
target_include_directories(Test_4 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/one_wire/include
        ${CMAKE_CURRENT_LIST_DIR}/ds18b20/include
)

pico_add_extra_outputs(Test_4)

