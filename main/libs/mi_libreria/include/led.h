// Declaración de funciones para controlar un LED
#ifndef LED_H
#define LED_H

#include "pico/stdlib.h"

void led_init(uint pin);      // Inicializa el LED
void led_toggle(uint pin);    // Cambia estado del LED

#endif