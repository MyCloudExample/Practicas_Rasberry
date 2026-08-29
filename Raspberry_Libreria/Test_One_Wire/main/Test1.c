/**
 * @brief Este codigo fuerza la lectura del ID sin usar la libreria, es para garantizar que se peuda leer el dispositivo.
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define ONE_WIRE_PIN 15

// Funciones de bajo nivel muy simples
static inline void set_high(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
    gpio_put(ONE_WIRE_PIN, 1);
}

static inline void set_low(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
    gpio_put(ONE_WIRE_PIN, 0);
}

static inline uint8_t get_bit(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_IN);
    return gpio_get(ONE_WIRE_PIN);
}

// Generar un pulso de RESET y leer el pulso de presencia
uint8_t onewire_reset(void) {
    set_low();
    busy_wait_us(480);  // Pulso de RESET
    set_high();
    busy_wait_us(60);   // Esperar el tiempo de presencia
    uint8_t presence = get_bit();
    busy_wait_us(420);  // Timeout
    return presence;
}

// Escribir un bit (0 o 1)
void onewire_write_bit(uint8_t bit) {
    set_low();
    busy_wait_us(5);    // Pulso inicial
    if (bit) {
        set_high();
        busy_wait_us(55); // Tiempo para '1'
    } else {
        busy_wait_us(55); // Tiempo para '0'
        set_high();
    }
    busy_wait_us(5);    // Tiempo de recuperación
}

// Leer un bit
uint8_t onewire_read_bit(void) {
    uint8_t bit;
    set_low();
    busy_wait_us(5);    // Pulso inicial
    set_high();
    busy_wait_us(10);   // Esperar para muestrear
    bit = get_bit();
    busy_wait_us(45);   // Completar el slot
    return bit;
}

// Leer el ID completo (64 bits) de un solo dispositivo
void onewire_read_rom(uint8_t *id) {
    onewire_reset();
    // Enviar comando READ ROM (0x33)
    for (int i = 0; i < 8; i++) {
        onewire_write_bit((0x33 >> i) & 0x01);
    }
    // Leer los 8 bytes del ID
    for (int i = 0; i < 8; i++) {
        uint8_t byte = 0;
        for (int j = 0; j < 8; j++) {
            byte |= (onewire_read_bit() << j);
        }
        id[i] = byte;
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("\n=== Prueba de Fuerza Bruta 1-Wire en Pico 2 ===\n");

    uint8_t id[8];

    while (1) {
        printf("--- Nueva lectura ---\n");
        
        uint8_t presence = onewire_reset();
        printf("Pulso de presencia: %d\n", presence);

        if (presence == 0) {
            printf("Sensor detectado. Leyendo ID...\n");
            onewire_read_rom(id);
            printf("ID: ");
            for (int i = 0; i < 8; i++) {
                printf("%02X ", id[i]);
            }
            printf("\n");
        } else {
            printf("Sensor NO detectado.\n");
        }

        sleep_ms(3000);
    }
}