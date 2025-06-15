// led_control.h - Librería estática para control de LEDs
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "pico/stdlib.h"  // Dependencia del SDK de Pico

// Declaraciones
void led_init(uint gpio_pin);
void led_on(uint gpio_pin);
void led_off(uint gpio_pin);

#endif // LED_CONTROL_H
