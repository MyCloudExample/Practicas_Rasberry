#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#define ADC_PIN 27

int main() {
    stdio_init_all();
    adc_init();
    adc_gpio_init(ADC_PIN);      // Habilita GPIO26 como ADC
    adc_select_input(1);    // Selecciona canal 0

    while (1) 
    {
        uint16_t result = adc_read();  // Valor de 0 a 4095
        float voltage = result * 3.3f / 4095.0f;
        printf("Voltaje: %.2f V\n", voltage);
        printf("Valor= %d \n" ,result);
        sleep_ms(500);
    }
}
