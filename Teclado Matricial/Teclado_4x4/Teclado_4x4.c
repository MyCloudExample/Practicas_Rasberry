#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define ROWS 4
#define COLS 4

// Configuración de pines (ajusta según tu conexión real)
const uint row_pins[ROWS] = {2, 3, 4, 5};  // Fila 1 a 4
const uint col_pins[COLS] = {6, 7, 8, 9};     // Columna 1 a 4

const char keymap[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void init_keyboard() {
    // Configurar filas como salidas inicialmente en alto
    for (int i = 0; i < ROWS; i++) {
        gpio_init(row_pins[i]);
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_put(row_pins[i], 1);
    }

    // Configurar columnas como entradas con pull-up
    for (int i = 0; i < COLS; i++) {
        gpio_init(col_pins[i]);
        gpio_set_dir(col_pins[i], GPIO_IN);
        gpio_pull_up(col_pins[i]);
    }
}

void test_column_readings() {
    printf("\nTest de columnas:\n");
    for (int col = 0; col < COLS; col++) {
        printf("Columna %d (GPIO%d) estado: %d\n", 
               col+1, col_pins[col], gpio_get(col_pins[col]));
    }
    printf("\n");
}

char scan_keyboard() {
    for (int row = 0; row < ROWS; row++) {
        gpio_put(row_pins[row], 0);  // Activar fila
        
        // Pequeña pausa para estabilización
        sleep_us(50);
        
        for (int col = 0; col < COLS; col++) {
            if (!gpio_get(col_pins[col])) {
                // Espera de debounce
                sleep_ms(20);
                if (!gpio_get(col_pins[col])) {
                    // Desactivar fila antes de retornar
                    gpio_put(row_pins[row], 1);
                    return keymap[row][col];
                }
            }
        }
        gpio_put(row_pins[row], 1);  // Desactivar fila
    }
    return '\0';
}

int main() {
    stdio_init_all();
    init_keyboard();

    // Espera para estabilización USB
    sleep_ms(1000);
    
    printf("Teclado 4x3 - Debug\n");
    printf("Fila 1-4: GPIO %d,%d,%d,%d\n", 
           row_pins[0], row_pins[1], row_pins[2], row_pins[3]);
    printf("Col 1-3: GPIO %d,%d,%d\n\n", 
           col_pins[0], col_pins[1], col_pins[2]);

    test_column_readings();

    while (true) {
        char key = scan_keyboard();
        if (key != '\0') {
            /*printf("Tecla: %c (Fila %d, Col %d)\n", key, 
                   (int)(&key - &keymap[0][0]) / COLS + 1,
                   (int)(&key - &keymap[0][0]) % COLS + 1);*/
            printf("Tecla %c \n",key);
            //test_column_readings();
            sleep_ms(300);  // Pausa anti-rebote extendida
        }
        sleep_ms(10);
    }
}