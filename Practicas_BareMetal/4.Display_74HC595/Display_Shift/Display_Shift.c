#include <stdio.h>
#include "pico/stdlib.h"

#define CLOCK      0
#define SERIAL     1
#define VIEW       2  

void Shift_74HC595(uint8_t data);
void Setup(void);

uint8_t number[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
uint8_t indice=0;

int main()
{
    stdio_init_all();
    Setup();

    while (true) 
    {
        Shift_74HC595(number[indice]);
        sleep_ms(1000);
        indice++;
        if(indice == 10)
        {
            indice=0;
        }
    }
}

void Shift_74HC595(uint8_t data)
{uint8_t D;

    for (uint8_t i = 0; i < 8; i++)
    {
        D=7-i;  
        gpio_put(SERIAL,(data>>D)&0x01);
        gpio_put(CLOCK,1);
        sleep_ms(20);
        gpio_put(CLOCK,0);
        
    }
    gpio_put(VIEW,1);
    sleep_ms(1);
    gpio_put(VIEW,0);
    
}

void Setup(void)
{
     for (uint8_t PIN = 0; PIN < 3; PIN++)
    {
        gpio_init(PIN);
        gpio_set_dir(PIN,GPIO_OUT);
    }
}