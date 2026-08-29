#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>

#define adc_pin 26  //Pin 31 de la placa
#define resolucion 3.3/4095

int main() 
{
    uint32_t raw=0;
    float valor=0.0;
    
    stdio_init_all();
    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(0);

    while (true) 
    {
        raw= adc_read();   
        valor= raw*resolucion;
        printf("Valor bruto: %u \n",raw);
        printf("Valor tomado: %.2f \n",valor);
        sleep_ms(200);
    }

    return 0;
}