#include "pwm_lib.h"

void pwm_init_config(pwm_config_t *config) {
    // Verificar parámetros
    if (config == NULL) return;
    if (config->pin >= NUM_BANK0_GPIOS) return;

    // Configurar función PWM para el pin
    gpio_set_function(config->pin, GPIO_FUNC_PWM);

    // Obtener slice y canal asociados al pin
    config->slice = pwm_gpio_to_slice_num(config->pin);
    config->channel = pwm_gpio_to_channel(config->pin);

    // Configurar PWM
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, config->wrap);
    pwm_config_set_clkdiv(&cfg, config->clk_div);
    pwm_init(config->slice, &cfg, false);

    // Habilitar PWM
    pwm_set_enabled(config, true);
}

void pwm_set_level(pwm_config_t *config, uint16_t level) {
    if (config == NULL) return;
    if (level > config->wrap) level = config->wrap;

    pwm_set_chan_level(config->slice, config->channel, level);
}

uint pwm_get_wrap(pwm_config_t *config) {
    if (config == NULL) return 0;
    return config->wrap;
}

void pwm_set_enabled(pwm_config_t *config, bool enabled) {
    if (config == NULL) return;
    pwm_set_enabled(config->slice, enabled);
}
