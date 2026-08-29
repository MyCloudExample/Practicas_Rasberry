# Crear la biblioteca estática "keypad" con los archivos fuente
add_library(keypad STATIC
    src/keypad.c
)

# Linkeo dependencias de la bibliotecas
target_link_libraries(keypad
    pico_stdlib
)

# Incluir las cabeceras de la biblioteca
target_include_directories(keypad PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
