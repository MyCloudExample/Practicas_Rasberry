#include "led.h"
#include "hardware/gpio.h"

void led_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

void led_toggle(uint pin) {
    gpio_put(pin, !gpio_get(pin));
}