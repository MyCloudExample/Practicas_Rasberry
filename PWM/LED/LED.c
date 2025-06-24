#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PIN_PWM 15 //orresponde al pin 20 de la placa, slice 7, canal B
#define CLK_DIV 10 //divisor de clock
#define WRAP    12499 //Usado para genera una frecuencia
#define LEVEL   6500 //Clico de actividad del 50%
#define DELAY   50 //Retardo

int main()
{ pwm_config configuration_pwm;
  uint slice_num, channel;

    stdio_init_all();

    gpio_set_function(PIN_PWM,GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(PIN_PWM);
    channel = pwm_gpio_to_channel(PIN_PWM);
    configuration_pwm = pwm_get_default_config();
    pwm_set_wrap(slice_num,WRAP);
    pwm_set_clkdiv(slice_num,CLK_DIV);
    pwm_init(slice_num,&configuration_pwm,false);
    pwm_set_chan_level(slice_num,channel,LEVEL);
    pwm_set_enabled(slice_num,true);
    while (true) 
    {
        for (uint16_t i = 0; i < WRAP; i+=100)
        {
            pwm_set_chan_level(slice_num,channel,i);
            sleep_ms(DELAY);
        }  
        
    }
}
