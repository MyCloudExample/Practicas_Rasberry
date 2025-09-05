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

project(LCD_I2C_LIB C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

#Adicion de libreria LCD con I2C
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../lcd ${CMAKE_BINARY_DIR}/lcd)

# Add executable. Default name is the project name, version 0.1

add_executable(LCD_I2C_LIB LCD_I2C_LIB.c )

pico_set_program_name(LCD_I2C_LIB "LCD_I2C_LIB")
pico_set_program_version(LCD_I2C_LIB "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(LCD_I2C_LIB 0)
pico_enable_stdio_usb(LCD_I2C_LIB 0)

# Add the standard library to the build
target_link_libraries(LCD_I2C_LIB
        pico_stdlib
        lcd)

# Add the standard include files to the build
target_include_directories(LCD_I2C_LIB PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(LCD_I2C_LIB)

