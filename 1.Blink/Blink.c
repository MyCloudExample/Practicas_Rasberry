/*Ejemplo clasico de parpadear un led*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED 25 /*Defino el pin 5 como LED*/

int main()
{
    stdio_init_all();
    gpio_init(LED); /*Inicializo el pin*/
    gpio_set_dir(LED,GPIO_OUT); /*Configuro el pin como salida*/

    while (true) 
    {
        gpio_put(LED,1);  /*Coloco un nivel alto en el pin 5*/
        sleep_ms(1000);   /*Retardo de tiempo*/
        gpio_put(LED,0);  /*Coloco un nivel bajo en el pin 5*/
        sleep_ms(1000);    
    }
}
