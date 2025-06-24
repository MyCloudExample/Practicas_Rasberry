#ifndef PWM_LIB_H
#define PWM_LIB_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

typedef struct {
    uint pin;
    uint slice;
    uint channel;
    uint wrap;
    uint clk_div;
} pwm_config_t;

/**
 * @brief Inicializa el PWM con la configuración especificada
 * @param config Puntero a estructura de configuración PWM
 */
void pwm_init_config(pwm_config_t *config);

/**
 * @brief Establece el nivel de PWM (duty cycle)
 * @param config Puntero a estructura de configuración PWM
 * @param level Nivel de PWM (0 a config->wrap)
 */
void pwm_set_level(pwm_config_t *config, uint16_t level);

/**
 * @brief Obtiene el valor WRAP actual
 * @param config Puntero a estructura de configuración PWM
 * @return Valor WRAP configurado
 */
uint pwm_get_wrap(pwm_config_t *config);

/**
 * @brief Habilita o deshabilita el PWM
 * @param config Puntero a estructura de configuración PWM
 * @param enabled true para habilitar, false para deshabilitar
 */
void pwm_set_enabled(pwm_config_t *config, bool enabled);

#endif // PWM_LIB_H
