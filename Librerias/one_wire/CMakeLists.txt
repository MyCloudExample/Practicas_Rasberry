cmake_minimum_required(VERSION 3.12)
project(one_wire)

# Crear la biblioteca estática "one_wire" con los archivos fuente
add_library(one_wire STATIC
    src/one_wire.c
)

# Linkeo dependencias de la bibliotecas
target_link_libraries(one_wire
    pico_stdlib
	pico_time
hardware_gpio
)

# Incluir las cabeceras de la biblioteca
target_include_directories(one_wire PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
