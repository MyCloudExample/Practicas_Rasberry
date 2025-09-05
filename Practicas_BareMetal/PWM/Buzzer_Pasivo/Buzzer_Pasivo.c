#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define BUZZER_PIN 22  // GPIO15 (cambia si usas otro pin)

// Función para generar tonos PWM
void play_tone(uint gpio, uint frequency, uint duration_ms) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);

    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);  // Divisor = 125 (250MHz → 2MHz)
    pwm_config_set_wrap(&config, (2000000 / frequency) - 1);  // Ajusta frecuencia
    pwm_init(slice_num, &config, true);

    pwm_set_gpio_level(gpio, (2000000 / frequency) / 2);  // Ciclo de trabajo 50%
    sleep_ms(duration_ms);
    pwm_set_gpio_level(gpio, 0);  // Silencia el buzzer
}

int main() {
    stdio_init_all();  // Aunque no uses printf, se recomienda inicializar

    while (1) {
        // Secuencia de tonos: 1 kHz (500ms) → pausa → 2 kHz (300ms)
        play_tone(BUZZER_PIN, 1000, 500);
        sleep_ms(500);
        play_tone(BUZZER_PIN, 2000, 300);
        sleep_ms(1000);
    }
}