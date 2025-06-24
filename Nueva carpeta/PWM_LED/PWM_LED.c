#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main() {
    // 1. Configurar el GPIO como PWM
    const uint LED_PIN = 15; // Usamos GPIO15 (Slice 7, Canal B)
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM); // Asignar función PWM al pin

    // 2. Obtener el slice y canal asociados al GPIO
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
    uint chan = pwm_gpio_to_channel(LED_PIN);

    // 3. Configurar el PWM
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f); // Divisor de clock = 1 (125 MHz)
    pwm_config_set_wrap(&config, 12499);  // Frecuencia = 1 kHz (125,000,000 / 12500)

    // 4. Inicializar el slice de PWM
    pwm_init(slice_num, &config, true);

    // 5. Bucle para variar el brillo del LED (efecto "respiración")
    while (true) {
        // Aumentar brillo (0% a 100%)
        for (int i = 0; i <= 12500; i++) {
            pwm_set_chan_level(slice_num, chan, i);
            sleep_ms(1);
        }
        // Disminuir brillo (100% a 0%)
        for (int i = 12500; i >= 0; i--) {
            pwm_set_chan_level(slice_num, chan, i);
            sleep_ms(1);
        }
    }
}