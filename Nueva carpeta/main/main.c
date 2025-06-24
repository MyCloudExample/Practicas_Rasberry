#include <stdio.h>
#include "pico/stdlib.h"
#include "led_control.h"

int main() 
{
    const uint LED_PIN = 25;  // GPIO 25 (LED integrado en Pico)
    led_init(LED_PIN);

    while (true) {
        led_on(LED_PIN);
        sleep_ms(500);
        led_off(LED_PIN);
        sleep_ms(500);
    }
}
