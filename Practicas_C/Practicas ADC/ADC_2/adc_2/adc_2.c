#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>

#define adc_pin 26  //Pin 31 de la placa
#define resolucion 3.3f/4095.0f

int main() 
{
    uint32_t raw=0;
    float valor=0.0;
    
    stdio_init_all();
    adc_init();
    adc_gpio_init(adc_pin);
    adc_select_input(0);

    adc_fifo_setup(true,false,0,false,false);
    adc_fifo_drain();
    adc_run(true);

    while (true) 
    {
        if(adc_fifo_get_level() > 0)
        {
            raw= adc_fifo_get();
            valor= raw * resolucion;
            printf("Raw: %u | Valor: %.2f V \n",raw, valor);
        }
        else
        {
            printf("No hay datod en la FIFO\n");
        }
        sleep_ms(500);
    }

    return 0;
}