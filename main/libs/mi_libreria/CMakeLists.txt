# Librería como objeto estático
add_library(mi_libreria STATIC
    src/led.c
)

# Headers públicos
target_include_directories(mi_libreria PUBLIC include)

# Dependencias del SDK (necesarias para gpio_*)
target_link_libraries(mi_libreria pico_stdlib hardware_gpio)