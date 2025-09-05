#include <stdio.h>
#include "pico/stdlib.h"
#include "pwm_lib.h"

int main() 
{
    stdio_init_all();
    printf("Iniciando control PWM\n");


    // Configuración inicial
    pwm_config_t mi_pwm = 
    {
        .pin = 15,      // GPIO15
        .wrap = 12499,  // Para ~1 kHz
        .clk_div = 10   // Divisor de clock
    };

    // Inicialización
    pwm_init_config(&mi_pwm);

    // Ejemplo: barrido de duty cycle
    while (true) 
    {
        // Fade in
        for (uint16_t i = 0; i <= mi_pwm.wrap; i += 100) 
        {
            pwm_set_level(&mi_pwm, i);
            sleep_ms(10);
        }

        // Fade out
        for (uint16_t i = mi_pwm.wrap; i > 0; i -= 100) 
        {
            pwm_set_level(&mi_pwm, i);
            sleep_ms(10);
        }
    }
}