# oled

Para agregar esta biblioteca en el proyecto, incluir en el `CMakeLists.txt` general lo siguiente:

```cmake
# Añadir la subcarpeta donde está la biblioteca BMP280
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../oled ${CMAKE_BINARY_DIR}/oled)
# Agrega dependencia al proyecto
target_link_libraries(firmware oled)
```

## Uso de la biblioteca

Una vez incluida la biblioteca con `#include "oled.h"` podemos hacer algo básico con el BMP280 usando:


> :warning: La inicializacion del I2C de la Raspberry Pi Pico y los GPIO deben hacerse previamente.