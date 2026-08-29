/**
 * @file            bmp280.c
 * @brief           Ejemplo de uso de la librería BMP280 en Raspberry Pi Pico 2.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "bmp_280.h"

// Configuración I2C
#define I2C_PORT            i2c0
#define I2C_SDA_PIN         0
#define I2C_SCL_PIN         1
#define I2C_BAUDRATE        100000  // 100 kHz

int main(void)
{
    // 1. Inicializar el sistema
    stdio_init_all();
    sleep_ms(1000);
    
    printf("\n\n=== BMP280 con Raspberry Pi Pico 2 ===\n");
    
    // 2. Inicializar I2C
    printf("Inicializando I2C...\n");
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    printf("I2C inicializado: SDA=GP%d, SCL=GP%d, %d Hz\n", 
           I2C_SDA_PIN, I2C_SCL_PIN, I2C_BAUDRATE);
    
    // 3. Configurar la librería con el bus I2C
    BMP280_SetI2CConfig(I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN, I2C_BAUDRATE);
    printf("Librería configurada\n");
    
    // 4. Configurar el sensor
    BMP280_Config_t config = 
    {
        .osrs_t = BMP_280_CTRL_MEAS_OSR_T_X2,
        .osrs_p = BMP_280_CTRL_MEAS_OSR_P_X16,
        .mode = BMP_280_CTRL_MEAS_MODE_FORCED,
        .standby = BMP_280_CONFIG_T_BS_0_5,
        .filter = BMP_280_CONFIG_FILTER_C4,
        .spi3w_en = false,
        .i2c_addr = BMP280_I2C_ADDR_PRIM  // 0x76
    };
    
    // 5. Inicializar el sensor
    BMP280_CalibData_t calib;
    printf("\nInicializando sensor...\n");
    if (!BMP280_Init(&config, &calib)) 
    {
        printf("ERROR: No se pudo inicializar el sensor\n");
        printf("Verifica las conexiones y que el sensor esté alimentado\n");
        while (true) 
        {
            sleep_ms(1000);
            printf("Esperando...\n");
        }
    }
    printf("Sensor inicializado correctamente\n");
    
    // Mostrar datos de calibración (opcional)
    printf("\nDatos de calibración:\n");
    printf("  dig_T1: %u\n", calib.dig_T1);
    printf("  dig_T2: %d\n", calib.dig_T2);
    printf("  dig_T3: %d\n", calib.dig_T3);
    printf("  dig_P1: %u\n", calib.dig_P1);
    printf("  dig_P2: %d\n", calib.dig_P2);
    printf("  dig_P3: %d\n", calib.dig_P3);
    printf("  dig_P4: %d\n", calib.dig_P4);
    printf("  dig_P5: %d\n", calib.dig_P5);
    printf("  dig_P6: %d\n", calib.dig_P6);
    printf("  dig_P7: %d\n", calib.dig_P7);
    printf("  dig_P8: %d\n", calib.dig_P8);
    printf("  dig_P9: %d\n", calib.dig_P9);
    
    // 6. Bucle principal: leer y mostrar datos
    BMP280_Data_t data;
    uint32_t counter = 0;
    printf("\n--- Iniciando lecturas ---\n\n");
    
    while (true) 
    {
        if (BMP280_ReadMeasuredData(&data, &calib)) 
        {
            printf("[%lu] Temp: %6.2f °C, Pres: %8.2f hPa\n", 
                   counter, data.temperature, data.pressure);
            counter++;
        } 
        else 
        {
            printf("ERROR al leer datos\n");
        }
        sleep_ms(1000);
    }
    
    return 0;
}