#ifndef KEYPAD_H
#define KEYPAD_H

#include "pico/stdlib.h"

// Definición de Pines (Ajusta estos números según tus conexiones en la Pico 2)
#define ROW1_PIN 2
#define ROW2_PIN 3
#define ROW3_PIN 4
#define ROW4_PIN 5

#define COL1_PIN 6
#define COL2_PIN 7
#define COL3_PIN 8
#define COL4_PIN 9

/**
 * @brief Inicializa los pines GPIO necesarios para el teclado 4x4.
 */
void keypad_init(void);

/**
 * @brief Escanea la matriz y devuelve el carácter presionado en ese instante.
 * @return Carácter presionado ('1'-'D'), o '\0' si no hay ninguna tecla presionada.
 */
char keypad_get_key(void);

/**
 * @brief Escanea el teclado y solo devuelve la tecla en el flanco de subida
 *        (cuando se presiona por primera vez, evitando repeticiones repetidas).
 * @return Carácter presionado, o '\0' si no hay cambio de estado.
 */
char keypad_get_pressed_key(void);

#endif // KEYPAD4X4_H