#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define input_pin 2

volatile uint16_t pulse=0;
// Función callback CORREGIDA (usando uint en lugar de uint8_t)
void gpio_callback(uint gpio, uint32_t events) 
{
    if(gpio == input_pin) 
    {
        pulse++;
        printf("Pulso numero %d\n", pulse);
    }
}

int main() 
{
    stdio_init_all(); // Inicializar USB serial para printf
    
    // Configuración de GPIO
    gpio_init(input_pin);
    gpio_set_dir(input_pin, GPIO_IN);
    gpio_pull_down(input_pin);
    
    // Configurar interrupción para flanco de bajada
    gpio_set_irq_enabled_with_callback(input_pin, 
                                     GPIO_IRQ_EDGE_RISE, 
                                     true, 
                                     &gpio_callback);

    printf("Esperando interrupciones...\n");
    
    while(true) 
    {
        // El procesamiento principal puede ir aquí
        tight_loop_contents(); // Reduce consumo de energía
    }
}