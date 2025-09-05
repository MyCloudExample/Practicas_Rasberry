#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Definición de pines
#define LED1_PIN 16
#define LED2_PIN 17

void setup_gpios() {
    // Inicializar ambos pines como salidas
    gpio_init(LED1_PIN);
    gpio_init(LED2_PIN);
    
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    
    // Apagar ambos LEDs inicialmente
    gpio_put(LED1_PIN, 0);
    gpio_put(LED2_PIN, 0);
}

int main() {
    stdio_init_all(); // Inicializar comunicación serial (opcional para debug)
    setup_gpios();
    
    while (true) {
        // Encender LED1 y apagar LED2
        gpio_put(LED1_PIN, 1);
        gpio_put(LED2_PIN, 0);
        printf("LED1 encendido, LED2 apagado\n");
        sleep_ms(500);
        
        // Encender LED2 y apagar LED1
        gpio_put(LED1_PIN, 0);
        gpio_put(LED2_PIN, 1);
        printf("LED1 apagado, LED2 encendido\n");
        sleep_ms(500);
        
        // Opcional: Encender ambos LEDs
        gpio_put(LED1_PIN, 1);
        gpio_put(LED2_PIN, 1);
        printf("Ambos LEDs encendidos\n");
        sleep_ms(500);
        
        // Apagar ambos LEDs
        gpio_put(LED1_PIN, 0);
        gpio_put(LED2_PIN, 0);
        printf("Ambos LEDs apagados\n");
        sleep_ms(500);
    }
    
    return 0;
}