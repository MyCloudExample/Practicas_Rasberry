#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "OneWire.h"

#define ONE_WIRE_PIN 15  // GPIO donde está conectado el DS18B20
#define LED_PIN 25       // LED integrado en la Pico (opcional, para indicar actividad)

// ===================================================================================
// 1- Funciones de bajo nivel para la Raspberry Pi Pico 2
// ===================================================================================

void pin_mode_output(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_OUT);
    //printf("pin a SALIDA\n");
}

void pin_mode_input(void) {
    gpio_init(ONE_WIRE_PIN);
    gpio_set_dir(ONE_WIRE_PIN, GPIO_IN);
    //printf("pin a ENTRADA\n");
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
// 2- Función para imprimir el ID
// ===================================================================================

void print_id(uint8_t *id) {
    printf("ID (64 bits): ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", id[i]);
    }
    printf("\n");
}

// ===================================================================================
// 3- Función principal (en bucle infinito)
// ===================================================================================

int main() {
    // Inicializar la UART para printf
    stdio_init_all();
    
    // Inicializar el LED (opcional, para saber que el programa está corriendo)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    // Esperar 2 segundos para que la UART esté lista antes de imprimir
    sleep_ms(2000);
    printf("\n=== Iniciando lector de ID de DS18B20 (en bucle) ===\n");
    
    // Configurar el bus 1-Wire (una sola vez, fuera del bucle)
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
    
    // Bucle principal infinito
    while (1) {
        // Parpadear el LED para indicar que el programa está activo
        gpio_put(LED_PIN, 1);
        
        // Array para almacenar el ID leído
        uint8_t id[8];
        bool lectura_ok = false;
        
        // 1. Inicializar el bus.
        if (OneWire_Init(&bus)) 
        {
            printf("[OK] Dispositivo detectado en el bus.\n");
            // 2. Reset para detectar presencia
            if(OneWire_Reset(&bus))
            {
                // 2. Leer el ID del dispositivo (debe ser el único en el bus)
                if (OneWire_Command_ROM_READ(&bus, id)) 
                {
                    printf("[OK] ID leído correctamente:\n    ");
                    print_id(id);
                    lectura_ok = true;
                } 
                else 
                {
                    printf("[ERROR] Falló la lectura del ID.\n");
                }
            }
            
        } 
        else 
        {
            printf("[ERROR] No se detectó ningún dispositivo en el bus.\n");
        }
        
        // Apagar el LED después del intento de lectura
        gpio_put(LED_PIN, 0);
        
        // Imprimir una línea separadora y esperar antes de la siguiente lectura
        printf("----------------------------------------\n");
        printf("Esperando 3 segundos para la siguiente lectura...\n\n");
        sleep_ms(3000);
    }
}