/**
 * @brief               Este programa verifica el funcionamiento de las funciones de la libreria DS18B20:
 *                      - DS18B20_Init()                        
 *                      - DS18B20_Read_Temperature
 *                      - DS18B20_Read_Memory_Scratchpad()
 *                      - DS18B20_Save_Config()*
 *                      Tambien se verifica el fucionamiento de las funciones estaticas que no son accecibles para el programador:
 *                      - static bool DS18B20_Write_Scrathpad()
 *                      - static bool DS18B20_Select_Device()
 *                      NOTA: El asterisco * indica que es la nueva funcon que se adiciono a la libreria
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ds18b20.h"

// ============================================================
// 1. CONFIGURACIÓN DE PINES Y CONSTANTES
// ============================================================
#define ONEWIRE_PIN 15      // GPIO donde está conectado el DQ del DS18B20
#define LED_PIN 25          // LED interno del Pico

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
// 4. FUNCIÓN PARA MOSTRAR EL SCRATCHPAD
// ============================================================
void mostrar_scratchpad(OneWireBus_t* bus, uint8_t* rom_code) 
{
    uint8_t scratchpad[9];
    
    if (DS18B20_Read_Memory_Scratchpad(bus, scratchpad, rom_code)) {
        printf("\n--- SCRATCHPAD ---\n");
        printf("Byte 0 (LSB Temp): 0x%02X (%d)\n", scratchpad[0], scratchpad[0]);
        printf("Byte 1 (MSB Temp): 0x%02X (%d)\n", scratchpad[1], scratchpad[1]);
        printf("Byte 2 (TH):       0x%02X (%d)  <- Límite superior\n", scratchpad[2], (int8_t)scratchpad[2]);
        printf("Byte 3 (TL):       0x%02X (%d)  <- Límite inferior\n", scratchpad[3], (int8_t)scratchpad[3]);
        printf("Byte 4 (Config):   0x%02X       <- Resolución\n", scratchpad[4]);
        printf("Byte 5 (Reserved): 0x%02X\n", scratchpad[5]);
        printf("Byte 6 (Reserved): 0x%02X\n", scratchpad[6]);
        printf("Byte 7 (Reserved): 0x%02X\n", scratchpad[7]);
        printf("Byte 8 (CRC):      0x%02X\n", scratchpad[8]);
        
        // Calcular la temperatura a partir de los bytes 0 y 1
        int16_t raw_temp = (int16_t)((scratchpad[1] << 8) | scratchpad[0]);
        float temp = raw_temp * 0.0625f;
        printf("Temperatura calculada: %.2f °C\n", temp);
        printf("--------------------\n");
    } else {
        printf("Error al leer el scratchpad\n");
    }
}

// ============================================================
// 5. PROGRAMA PRINCIPAL
// ============================================================
int main(void) {
    // --- 5.1 Inicializar sistema ---
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    sleep_ms(5000);
    
    printf("\n\n");
    printf("========================================\n");
    printf("   PRUEBA DS18B20 - GUARDAR CONFIGURACIÓN\n");
    printf("========================================\n");
    printf("Pin DQ: GPIO %d\n", ONEWIRE_PIN);
    
    // --- 5.2 Configurar el pin GPIO ---
    gpio_init(ONEWIRE_PIN);
    gpio_set_dir(ONEWIRE_PIN, GPIO_OUT);
    gpio_put(ONEWIRE_PIN, 1);
    
    // --- 5.3 Configurar el bus 1-Wire ---
    bus.pin_mode_output = pin_mode_output;
    bus.pin_mode_input = pin_mode_input;
    bus.pin_write_high = pin_write_high;
    bus.pin_write_low = pin_write_low;
    bus.pin_read = pin_read;
    bus.delay_us = delay_us;
    bus.mode_rate = ONE_WIRE_MODE_STANDARD;
    
    // --- 5.4 Inicializar el bus ---
    if (!OneWire_Init(&bus)) {
        printf("ERROR: No se pudo inicializar el bus 1-Wire\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Bus 1-Wire inicializado.\n");
    
    // --- 5.5 Verificar presencia del sensor ---
    if (!OneWire_Reset(&bus)) {
        printf("ERROR: No se detectó ningún dispositivo en el bus.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Dispositivo detectado en el bus.\n");
    
    // ============================================================
    // 5.6 PRIMERA PARTE: CONFIGURAR Y GUARDAR
    // ============================================================
    printf("\n--- CONFIGURANDO SENSOR ---\n");
    
    // Configuración inicial (valores que queremos guardar)
    DS18B20_Config_t config = {
        .th = 35,   // Alarma si T > 35°C
        .tl = 5,    // Alarma si T < 5°C
        .resolution = DS18B20_REG_CONFIGURATION_11  // 11 bits (0.125°C/paso)
    };
    
    printf("Configuración deseada:\n");
    printf("  TH: %d°C\n", config.th);
    printf("  TL: %d°C\n", config.tl);
    printf("  Resolución: 11 bits (tiempo de conversión: 375 ms)\n");
    
    // Inicializar el sensor con la configuración
    if (!DS18B20_Init(&config, &bus, NULL)) {
        printf("ERROR: No se pudo inicializar el DS18B20.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Sensor inicializado con la configuración.\n");
    
    // Mostrar el scratchpad ANTES de guardar en EEPROM
    printf("\n--- SCRATCHPAD ANTES DE GUARDAR ---\n");
    mostrar_scratchpad(&bus, NULL);
    
    // Guardar la configuración en la EEPROM
    printf("\n--- GUARDANDO CONFIGURACIÓN EN EEPROM ---\n");
    if (DS18B20_Save_Config(&bus, NULL)) {
        printf("OK: Configuración guardada en EEPROM correctamente.\n");
    } else {
        printf("ERROR: No se pudo guardar la configuración en EEPROM.\n");
    }
    
    // Mostrar el scratchpad DESPUÉS de guardar (debería ser igual)
    printf("\n--- SCRATCHPAD DESPUÉS DE GUARDAR ---\n");
    mostrar_scratchpad(&bus, NULL);
    
    // ============================================================
    // 5.7 SEGUNDA PARTE: REINICIAR EL SENSOR Y VERIFICAR PERSISTENCIA
    // ============================================================
    printf("\n========================================\n");
    printf("   REINICIANDO SENSOR (simulado)\n");
    printf("========================================\n");
    printf("Desconecta y vuelve a conectar el sensor,\n");
    printf("o reinicia el Pico para verificar que la\n");
    printf("configuración persiste en la EEPROM.\n");
    printf("\nPresiona una tecla para continuar...\n");
    
    // Esperar entrada del usuario (por UART)
    getchar();
    
    printf("\n--- REINICIANDO COMUNICACIÓN ---\n");
    
    // Reinicializar el bus y el sensor
    if (!OneWire_Reset(&bus)) {
        printf("ERROR: No se detectó el sensor después del reinicio.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Sensor detectado después del reinicio.\n");
    
    // Leer el scratchpad para verificar que la configuración se mantiene
    printf("\n--- SCRATCHPAD DESPUÉS DEL REINICIO ---\n");
    mostrar_scratchpad(&bus, NULL);
    
    // Verificar que los valores coinciden con los guardados
    uint8_t scratchpad[9];
    if (DS18B20_Read_Memory_Scratchpad(&bus, scratchpad, NULL)) {
        if (scratchpad[2] == config.th && 
            scratchpad[3] == config.tl && 
            scratchpad[4] == config.resolution) {
            printf("\n✅ ÉXITO: La configuración se mantiene en la EEPROM.\n");
            printf("   TH: %d°C, TL: %d°C, Resolución: 0x%02X\n", 
                   scratchpad[2], (int8_t)scratchpad[3], scratchpad[4]);
        } else {
            printf("\n❌ ERROR: La configuración NO se mantiene.\n");
            printf("   Esperado: TH=%d, TL=%d, Config=0x%02X\n", 
                   config.th, config.tl, config.resolution);
            printf("   Leído:    TH=%d, TL=%d, Config=0x%02X\n", 
                   scratchpad[2], (int8_t)scratchpad[3], scratchpad[4]);
        }
    }
    
    // ============================================================
    // 5.8 BUCLE DE LECTURA DE TEMPERATURA (opcional)
    // ============================================================
    printf("\n--- INICIANDO LECTURAS DE TEMPERATURA ---\n");
    uint32_t contador = 0;
    while (1) {
        gpio_put(LED_PIN, 1);
        float temp = DS18B20_Read_Temperature(&bus, NULL);
        gpio_put(LED_PIN, 0);
        
        if (temp < -100) {
            printf("[%3lu] ERROR: Código %.1f\n", contador, temp);
        } else {
            printf("[%3lu] Temperatura: %6.2f °C\n", contador, temp);
        }
        contador++;
        sleep_ms(1000);
    }
    
    return 0;
}