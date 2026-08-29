/**
 * @brief Este coidgo se utiliza para verifcar cada funcion de la libreria.
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "OneWire.h"   // Tu librería

#define ONE_WIRE_PIN 15

// ===================================================================================
// 1- Funciones de bajo nivel para la Raspberry Pi Pico 2
// ===================================================================================

void pin_mode_output(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
}

void pin_mode_input(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_IN);
}

void pin_write_high(void) {
    gpio_put(ONE_WIRE_PIN, 1);
}

void pin_write_low(void) {
    gpio_put(ONE_WIRE_PIN, 0);
}

uint8_t pin_read(void) {
    return gpio_get(ONE_WIRE_PIN);
}

void delay_us(uint32_t us) {
    busy_wait_us(us);
}

// ===================================================================================
// 2- Funciones de depuración
// ===================================================================================

void test_reset(OneWireBus_t *bus) {
    printf("--- Test: OneWire_Reset ---\n");
    bool result = OneWire_Reset(bus);
    printf("Resultado: %s\n", result ? "Éxito (presencia detectada)" : "Fallo (no hay presencia)");
}

void test_write_bit(OneWireBus_t *bus, uint8_t bit) {
    printf("--- Test: OneWire_Write_Bit (bit=%d) ---\n", bit);
    OneWire_Write_Bit(bus, bit);
    printf("Bit escrito.\n");
}

void test_read_bit(OneWireBus_t *bus) {
    printf("--- Test: OneWire_Read_Bit ---\n");
    uint8_t bit = OneWire_Read_Bit(bus);
    printf("Bit leído: %d\n", bit);
}

void test_write_byte(OneWireBus_t *bus, uint8_t data) {
    printf("--- Test: OneWire_Write_Byte (0x%02X) ---\n", data);
    bool result = OneWire_Write_Byte(bus, data);
    printf("Resultado: %s\n", result ? "Éxito" : "Fallo");
}

void test_read_byte(OneWireBus_t *bus) {
    printf("--- Test: OneWire_Read_Byte ---\n");
    uint8_t data = OneWire_Read_Byte(bus);
    printf("Byte leído: 0x%02X\n", data);
}

// ===================================================================================
// 3- Función principal
// ===================================================================================

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("\n=== Pruebas de OneWire_Reset ===\n");

    // Configurar el bus 1-Wire
    OneWireBus_t bus = {
        .pin_mode_output = pin_mode_output,
        .pin_mode_input = pin_mode_input,
        .pin_write_high = pin_write_high,
        .pin_write_low = pin_write_low,
        .pin_read = pin_read,
        .delay_us = delay_us,
        .mode_rate = ONE_WIRE_MODE_STANDARD,
        .mode = NULL
    };

    // Inicializar el bus (esto llama a OneWire_Reset)
    printf("\n1. Inicializando bus...\n");
    if (!OneWire_Init(&bus)) {
        printf("ERROR: No se detectó ningún dispositivo en el bus.\n");
        // Continuamos igual para probar las otras funciones
    } else {
        printf("Dispositivo detectado en el bus.\n");
    }

    // --- PRUEBAS UNITARIAS ---

    // 1. Probar solo el RESET
    printf("\n--- SECCIÓN: Pruebas de RESET ---\n");
    test_reset(&bus);
    sleep_ms(1000);

    // 2. Probar escritura de un bit (sin reset)
    printf("\n--- SECCIÓN: Pruebas de escritura de bits ---\n");
    test_write_bit(&bus, 1);
    sleep_ms(1000);
    test_write_bit(&bus, 0);
    sleep_ms(1000);

    // 3. Probar lectura de un bit (sin reset)
    printf("\n--- SECCIÓN: Pruebas de lectura de bits ---\n");
    test_read_bit(&bus);
    sleep_ms(1000);

    // 4. Probar escritura de un byte (sin reset)
    printf("\n--- SECCIÓN: Pruebas de escritura de bytes ---\n");
    test_write_byte(&bus, 0x33);
    sleep_ms(1000);

    // 5. Probar lectura de un byte (sin reset)
    printf("\n--- SECCIÓN: Pruebas de lectura de bytes ---\n");
    test_read_byte(&bus);
    sleep_ms(1000);

    // 6. Probar el comando READ ROM completo
    printf("\n--- SECCIÓN: Prueba de READ ROM ---\n");
    uint8_t id[8];
    if (OneWire_Command_ROM_READ(&bus, id)) {
        printf("ID leído correctamente:\n    ");
        for (int i = 0; i < 8; i++) {
            printf("%02X ", id[i]);
        }
        printf("\n");
    } else {
        printf("ERROR: Falló la lectura del ID.\n");
    }

    while (1) {
        sleep_ms(1000);
    }
}