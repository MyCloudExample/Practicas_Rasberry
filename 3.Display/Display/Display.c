#include <stdio.h>
#include "pico/stdlib.h"
/*Defino los pines para cada LED CATODO COMUN*/
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define MASK_0 0x001F

void Display (uint8_t data);

uint8_t number[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
uint8_t indice=0;

int main()
{
    stdio_init_all();
    for(uint8_t i = 0; i < 7; i++)
    {
        gpio_init(i);
        gpio_set_dir(i,GPIO_OUT);
    }
    
    while (true) 
    {
       Display(number[indice]);
       sleep_ms(1000);
       indice++;
       if(indice == 10)
       {
            indice=0;
            printf("Restart Count \n");
       }
    }
}

void Display (uint8_t data)
{
    gpio_put(A,(data>>0)&0x01);
    gpio_put(B,(data>>1)&0x01);
    gpio_put(C,(data>>2)&0x01);
    gpio_put(D,(data>>3)&0x01);
    gpio_put(E,(data>>4)&0x01);
    gpio_put(F,(data>>5)&0x01);
    gpio_put(G,(data>>6)&0x01);
}