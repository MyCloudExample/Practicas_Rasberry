#include <stdio.h>
#include "pico/stdlib.h"

#define Comando_Inicial     0x03
#define Confiugracion_4BITS 0x02
#define DISPLAY_CURSOR      0x28
#define DISPLAY_CLEAR       0x08
#define ENTRY_MODE          0x06
#define DISPLAY_ON_OFF      0x0F

#define D7 5
#define D6 4
#define D5 3
#define D4 2
#define RS 1
#define E  0

void SendCmd(char cmd)
{char cmd_high, cmd_low;

    cmd_high=((cmd>>4)&0x0F);
    cmd_low=((cmd)& 0x0F);
    /*Envio la parte alta del comando*/
    gpio_put(RS,0);
    gpio_put(E,0);
    gpio_put(D7,(cmd_high >> 3) & 0x01);
    gpio_put(D6,(cmd_high >> 2) & 0x01);
    gpio_put(D5,(cmd_high >> 1) & 0x01);
    gpio_put(D4,(cmd_high >> 0) & 0x01);
    gpio_put(E,1);
    sleep_ms(1);
    gpio_put(E,0);
    /*Envio la parte baja del comando*/
    gpio_put(D7,(cmd_low >> 3) & 0x01);
    gpio_put(D6,(cmd_low >> 2) & 0x01);
    gpio_put(D5,(cmd_low >> 1) & 0x01);
    gpio_put(D4,(cmd_low >> 0) & 0x01);
    gpio_put(E,1);
    sleep_ms(1);
    gpio_put(E,0);

}
int main()
{
    stdio_init_all();
    for (uint8_t GPIO = 0; GPIO < 6; GPIO++)
    {
        gpio_init(GPIO);
        /*Inicializa pines del 0 al 5*/
    }
    gpio_set_dir(D7,GPIO_OUT);
    gpio_set_dir(D6,GPIO_OUT);
    gpio_set_dir(D5,GPIO_OUT);
    gpio_set_dir(D4,GPIO_OUT);
    gpio_set_dir(RS,GPIO_OUT);
    gpio_set_dir(E,GPIO_OUT);
    
    SendCmd(Comando_Inicial);
    SendCmd(Comando_Inicial);
    SendCmd(Comando_Inicial);
    SendCmd(Confiugracion_4BITS);
    SendCmd(DISPLAY_CURSOR);
    SendCmd(DISPLAY_CLEAR);
    SendCmd(ENTRY_MODE);
    SendCmd(DISPLAY_ON_OFF);

    while (true) 
    {
        
    }
}
