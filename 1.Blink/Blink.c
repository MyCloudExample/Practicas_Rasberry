/*Ejemplo clasico de parpadear un led*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LED_1 10 /*Defino el pin 5 como LED*/
#define LED_2 11
#define LED_3 12

int main()
{
    stdio_init_all();
    gpio_init(LED_1); /*Inicializo el pin*/
    gpio_init(LED_2);
    gpio_init(LED_3);
    gpio_set_dir(LED_1,GPIO_OUT); /*Configuro el pin como salida*/
    gpio_set_dir(LED_2,GPIO_OUT);
    gpio_set_dir(LED_3,GPIO_OUT);

    while (true) 
    {
        gpio_put(LED_1,1);  /*Coloco un nivel alto en el pin 5*/
        gpio_put(LED_2,1);
        gpio_put(LED_3,1);
        sleep_ms(1000);   /*Retardo de tiempo*/
        gpio_put(LED_1,0);  /*Coloco un nivel bajo en el pin 5*/
        gpio_put(LED_2,0);
        gpio_put(LED_3,0);
        sleep_ms(1000);    
    }
}
