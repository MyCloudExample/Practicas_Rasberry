/**
 * @brief               Este programa verifica el funcionamiento de dos funciones de la libreria DS18B20:
 *                      - DS18B20_Init()                        
 *                      - DS18B20_Read_Temperature
 *                      - DS18B20_Read_Memory_Scratchpad()*
 *                      Tambien se verifica el fucionamiento de las funciones estaticas que no son accecibles para el programador:
 *                      - static bool DS18B20_Write_Scrathpad()
 *                      - static bool DS18B20_Select_Device()
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ds18b20.h"

// ============================================================
// 1. CONFIGURACIÓN DE PINES Y CONSTANTES
// ============================================================
#define ONEWIRE_PIN 15      // GPIO donde está conectado el DQ del DS18B20
#define LED_PIN 25          // LED interno del Pico (opcional, para indicar actividad)

// ============================================================
// 2. FUNCIONES DE BAJO NIVEL PARA 1-WIRE (Callbacks)
// ============================================================
static void pin_mode_output(void) {
    gpio_set_dir(ONEWIRE_PIN, GPIO_OUT);
}

static void pin_mode_input(void) {
    gpio_set_dir(ONEWIRE_PIN, GPIO_IN);
}

static void pin_write_high(void) {
    gpio_put(ONEWIRE_PIN, 1);
}

static void pin_write_low(void) {
    gpio_put(ONEWIRE_PIN, 0);
}

static uint8_t pin_read(void) {
    return gpio_get(ONEWIRE_PIN);
}

static void delay_us(uint32_t time_us) {
    sleep_us(time_us);
}

// ============================================================
// 3. VARIABLES GLOBALES
// ============================================================
static OneWireBus_t bus;

// ============================================================
// 4. PROGRAMA PRINCIPAL
// ============================================================
int main(void) {
    // --- 4.1 Inicializar sistema y comunicación ---
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    // Pequeña espera para que la UART se estabilice
    sleep_ms(2000);
    
    printf("\n\n");
    printf("========================================\n");
    printf("   PRUEBA DS18B20 en Raspberry Pi Pico 2\n");
    printf("========================================\n");
    printf("Pin DQ: GPIO %d\n", ONEWIRE_PIN);
    printf("Inicializando...\n");

    // --- 4.2 Configurar el pin GPIO ---
    gpio_init(ONEWIRE_PIN);
    gpio_set_dir(ONEWIRE_PIN, GPIO_OUT);
    gpio_put(ONEWIRE_PIN, 1);  // Dejar la línea en alto (reposo)

    // --- 4.3 Configurar el bus 1-Wire ---
    bus.pin_mode_output = pin_mode_output;
    bus.pin_mode_input = pin_mode_input;
    bus.pin_write_high = pin_write_high;
    bus.pin_write_low = pin_write_low;
    bus.pin_read = pin_read;
    bus.delay_us = delay_us;
    bus.mode_rate = ONE_WIRE_MODE_STANDARD;

    // --- 4.4 Inicializar el bus 1-Wire ---
    if (!OneWire_Init(&bus)) {
        printf("ERROR: No se pudo inicializar el bus 1-Wire\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Bus 1-Wire inicializado.\n");

    // --- 4.5 Verificar presencia de dispositivos en el bus ---
    printf("Buscando dispositivos en el bus...\n");
    if (!OneWire_Reset(&bus)) {
        printf("ERROR: No se detectó ningún dispositivo en el bus.\n");
        printf("   - Verifica las conexiones (DQ, GND, VDD).\n");
        printf("   - Asegura la resistencia de pull-up de 4.7kΩ.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Dispositivo detectado en el bus.\n");

    // --- 4.6 Configurar el DS18B20 ---
    printf("Configurando DS18B20...\n");
    DS18B20_Config_t config = {
        .th = 30,   // Alarma si T > 30°C
        .tl = 0,    // Alarma si T < 0°C
        .resolution = DS18B20_REG_CONFIGURATION_12
    };

    if (!DS18B20_Init(&config, &bus, NULL)) {
        printf("ERROR: No se pudo inicializar el DS18B20.\n");
        printf("   - Verifica que el dispositivo sea un DS18B20.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: DS18B20 inicializado correctamente.\n");
    printf("   - Alarma: T > %d°C o T < %d°C\n", config.th, config.tl);
    printf("   - Resolución: 12 bits (0.0625°C/paso)\n");
    
    // --- 4.7 Bucle principal de lectura ---
    printf("\nIniciando lecturas de temperatura...\n");
    printf("========================================\n\n");
    sleep_ms(5000);
    // Leer el scratchpad completo
    uint8_t scratchpad[9];
    if (DS18B20_Read_Memory_Scratchpad(&bus, scratchpad, NULL)) {
        printf("Scratchpad leído correctamente:\n");
        printf("Byte 0 (LSB Temp): 0x%02X (%d)\n", scratchpad[0], scratchpad[0]);
        printf("Byte 1 (MSB Temp): 0x%02X (%d)\n", scratchpad[1], scratchpad[1]);
        printf("Byte 2 (TH):       0x%02X (%d)\n", scratchpad[2], (int8_t)scratchpad[2]);
        printf("Byte 3 (TL):       0x%02X (%d)\n", scratchpad[3], (int8_t)scratchpad[3]);
        printf("Byte 4 (Config):   0x%02X\n", scratchpad[4]);
        printf("Byte 5 (Reserved): 0x%02X\n", scratchpad[5]);
        printf("Byte 6 (Reserved): 0x%02X\n", scratchpad[6]);
        printf("Byte 7 (Reserved): 0x%02X\n", scratchpad[7]);
        printf("Byte 8 (CRC):      0x%02X\n", scratchpad[8]);
    } else {
        printf("Error al leer el scratchpad\n");
    }
    
    uint32_t contador = 0;
    while (1) {
        // Parpadear el LED para indicar actividad
        gpio_put(LED_PIN, 1);
        
        // Leer temperatura
        float temp = DS18B20_Read_Temperature(&bus, NULL);
        if(temp == -999.0f)
        {
            printf("[ERROR] No se pudo leer el scratchpad\n");
        }
        
        // Apagar LED después de la lectura
        gpio_put(LED_PIN, 0);
        
        // Mostrar resultado
        if (temp < -100) {
            // Si el error es -999.0 o similar, mostramos el código
            printf("[%3lu] ERROR: Código %.1f (fallo de comunicación)\n", 
                   contador, temp);
        } else {
            printf("[%3lu] Temperatura: %6.2f °C\n", contador, temp);
        }
        
        contador++;
        
        // Esperar 1 segundo entre lecturas
        sleep_ms(1000);
    }
    
    return 0;
}