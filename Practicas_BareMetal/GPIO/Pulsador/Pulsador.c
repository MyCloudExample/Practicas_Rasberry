#include "pico/stdlib.h"
#include <stdio.h>

#define BUTTON_PIN 7

int main() {
    // Inicializar la biblioteca estándar
    stdio_init_all();
    
    // Inicializar el GPIO del pulsador con pull-down
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);
    
    uint32_t counter = 0;
    bool last_button_state = false;
    sleep_ms(5000);
    printf("Programa iniciado. Presione el pulsador en GPIO %d\n", BUTTON_PIN);
    
    while (true) {
        bool current_button_state = gpio_get(BUTTON_PIN);
        
        // Detectar flanco ascendente (cuando se presiona el botón)
        if (current_button_state && !last_button_state) {
            counter++;
            printf("Pulsador presionado! Contador: %lu\n", counter);
            
            // Pequeño retardo para evitar rebotes
            sleep_ms(20);
        }
        
        last_button_state = current_button_state;
        
        // Pequeño retardo para reducir el consumo de CPU
        sleep_ms(10);
    }
    
    return 0;
}