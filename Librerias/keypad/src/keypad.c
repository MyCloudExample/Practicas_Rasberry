#include "keypad.h"

// Mapeo físico del teclado matricial estándar
static const char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static const uint row_pins[4] = {ROW1_PIN, ROW2_PIN, ROW3_PIN, ROW4_PIN};
static const uint col_pins[4] = {COL1_PIN, COL2_PIN, COL3_PIN, COL4_PIN};

// Variable interna para el estado anterior (usada en el debounce)
static char last_key = '\0';

void keypad_init(void) {
    // 1. Configurar Pines de Filas como Salida (Escribiremos 1s para escanear)
    for (int i = 0; i < 4; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 0);
    }

    // 2. Configurar Pines de Columnas como Entrada con Pull-Down
    for (int i = 0; i < 4; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_down(col_pins[i]);
    }
}

char keypad_get_key(void) {
    char current_key = '\0';

    // Escaneo de la matriz (Fila por Fila)
    for (int r = 0; r < 4; r++) {
        gpio_put(row_pins[r], 1); // Activamos la fila actual
        
        // Un retraso muy pequeño en microsegundos para estabilizar la capacidad del pin
        sleep_us(10); 

        for (int c = 0; c < 4; c++) {
            if (gpio_get(col_pins[c])) {
                current_key = key_map[r][c];
                break;
            }
        }

        gpio_put(row_pins[r], 0); // Desactivamos la fila
        if (current_key != '\0') break; // Si encontramos una tecla, salimos del bucle
    }

    return current_key;
}

char keypad_get_pressed_key(void) {
    char current_key = keypad_get_key();
    char result = '\0';

    // Si el estado de la tecla cambió respecto a la última lectura
    if (current_key != last_key) {
        if (current_key != '\0') {
            result = current_key; // Se presionó una nueva tecla
        }
        last_key = current_key;
    }

    return result;
}