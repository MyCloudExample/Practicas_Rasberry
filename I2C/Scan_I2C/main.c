#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

int main() {
    stdio_init_all();
    printf("Inicio del escaneo para I2C \n");
    sleep_ms(5000);
    i2c_init(I2C_PORT, 100 * 1000); // Inicializa I2C a 100 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    printf("\nEscaneando bus I2C...\n");

    for (int addr = 0x08; addr <= 0x77; ++addr) {
        uint8_t rxdata;
        int ret = i2c_read_blocking(I2C_PORT, addr, &rxdata, 1, false);
        if (ret >= 0) {
            printf("Dispositivo encontrado en dirección: 0x%02X\n", addr);
        }
    }

    printf("Escaneo completado.\n");
    while (1) tight_loop_contents();
}