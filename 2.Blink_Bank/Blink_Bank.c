#include <stdio.h>
#include "pico/stdlib.h"

#define BANK0 0x0F

int main()
{
    stdio_init_all();
    for (uint8_t GPIO = 0; GPIO < 4; GPIO++)
    {
        gpio_init(GPIO);
    }
    gpio_set_dir_masked(BANK0,0x0F);
    while (true) 
    {
        gpio_set_mask(BANK0);
        sleep_ms(1000);
        gpio_clr_mask(BANK0);
        sleep_ms(1000);
    }
}
