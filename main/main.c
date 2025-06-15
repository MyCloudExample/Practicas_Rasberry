#include <stdio.h>
#include "pico/stdlib.h"
#include "led.h"  // Header de nuestra librería

#define LED_PIN 25  // LED integrado en la Pico

int main() {
    stdio_init_all();

    // Usa funciones de la librería
    led_init(LED_PIN);

    while (true) {
        led_toggle(LED_PIN);
        sleep_ms(500);
        printf("LED toggled!\n");
    }
}