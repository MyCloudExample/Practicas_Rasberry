cmake_minimum_required(VERSION 3.12)
project(led_control)

# Crear la biblioteca estática "led_control" con los archivos fuente
add_library(led_control STATIC
    src/led_control.c
)

# Linkeo dependencias de la bibliotecas
target_link_libraries(led_control
    pico_stdlib
)

# Incluir las cabeceras de la biblioteca
target_include_directories(led_control PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
