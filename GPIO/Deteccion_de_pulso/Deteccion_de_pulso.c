#include "pico/stdlib.h"
#include <stdio.h>

#define BUTTON_PIN 7

int main() {
    stdio_init_all();
    
    // Inicializar el pin del botón como entrada con pull-down
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);
    
    uint32_t counter = 0;
    bool boton;
    
    printf("Contador de pulsos en alto iniciado...\n");
    
    while (true) {
      
        // Solo contar cuando cambia de bajo a alto
        boton=gpio_get(BUTTON_PIN);
        if (boton) 
        {
            sleep_ms(30);
            counter++;
            printf("Pulso alto detectado! Contador: %lu\n", counter);
            boton=gpio_get(BUTTON_PIN);
            
            // Pequeña espera para evitar rebotes
            sleep_ms(20);
        }
    }
    
    return 0;
}