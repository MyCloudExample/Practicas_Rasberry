#include "led_control.h"
#include "hardware/gpio.h"

void led_init(uint gpio_pin) {
    gpio_init(gpio_pin);          // Inicializa el pin GPIO
    gpio_set_dir(gpio_pin, true); // Configura el pin como salida
    gpio_put(gpio_pin, false);    // Apaga el LED inicialmente
}

void led_on(uint gpio_pin) {
    gpio_put(gpio_pin, true);     // Enciende el LED
}

void led_off(uint gpio_pin) {
    gpio_put(gpio_pin, false);    // Apaga el LED
}