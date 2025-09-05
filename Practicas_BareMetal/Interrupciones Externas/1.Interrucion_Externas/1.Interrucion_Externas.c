#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define INPUT_PIN 7
#define DEBOUNCE_MS 20 // Tiempo de debounce

volatile uint32_t pulse = 0;
volatile bool new_pulse = false;
absolute_time_t last_interrupt_time = 0;

void gpio_callback(uint gpio, uint32_t events) {
    if(gpio == INPUT_PIN) {
        absolute_time_t now = get_absolute_time();
        int64_t diff = absolute_time_diff_us(last_interrupt_time, now) / 1000;
        
        if(diff > DEBOUNCE_MS) {
            pulse++;
            new_pulse = true;
            last_interrupt_time = now;
        }
    }
}

int main() {
    stdio_init_all();
    
    gpio_init(INPUT_PIN);
    gpio_set_dir(INPUT_PIN, GPIO_IN);
    //gpio_pull_down(INPUT_PIN);
    
    gpio_set_irq_enabled_with_callback(INPUT_PIN, 
                                     GPIO_IRQ_EDGE_RISE, 
                                     true, 
                                     &gpio_callback);

    printf("Esperando interrupciones...\n");
    
    while(true) {
        if(new_pulse) {
            printf("Pulso numero %lu\n", pulse);
            new_pulse = false;
        }
    }
}