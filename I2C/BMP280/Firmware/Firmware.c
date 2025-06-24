#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bmp280.h"
#include <math.h>

// Configuración de los pines I2C
#define I2C_PORT i2c0
#define I2C_SDA_PIN 8 //pin 11
#define I2C_SCL_PIN 9 //pin 12

void init_i2c();

int main() 
{
    stdio_init_all();

    init_i2c();

    // Inicializar BMP280
    bmp280_init(I2C_PORT);

    // Obtener parámetros de calibración
    struct bmp280_calib_param params;
    bmp280_get_calib_params(&params);

    // Variables para almacenar lecturas
    int32_t raw_temp, raw_pressure;
    float temp_c;
    int32_t pressure_pa;

    while (1) {
        // Leer valores crudos
        bmp280_read_raw(&raw_temp, &raw_pressure);

        // Convertir valores
        temp_c = bmp280_convert_temp(raw_temp, &params);
        pressure_pa = bmp280_convert_pressure(raw_pressure, raw_temp, &params);

        // Mostrar resultados
        printf("Temperatura: %.2f °C\n", temp_c);
        printf("Presión: %ld Pa\n", pressure_pa);
        printf("Altitud aproximada: %.2f m\n\n", 44330.0 * (1.0 - pow((pressure_pa / 100.0) / 1013.25, 0.1903)));

        sleep_ms(1000);
    }

    return 0;
}

void init_i2c()
{
    i2c_init(I2C_PORT, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
}