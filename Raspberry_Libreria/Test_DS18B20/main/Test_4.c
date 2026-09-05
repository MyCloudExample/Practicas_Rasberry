/**
 * @brief               Este programa verifica el funcionamiento de las funciones de la libreria DS18B20:
 *                      - DS18B20_Init()                        
 *                      - DS18B20_Read_Temperature
 *                      - DS18B20_Read_Memory_Scratchpad()
 *                      - DS18B20_Save_Config()
 *                      - DS18B20_Set_Config()*
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
// 4. FUNCIÓN PARA MOSTRAR EL SCRATCHPAD (MEJORADA)
// ============================================================
void mostrar_scratchpad(OneWireBus_t* bus, uint8_t* rom_code, const char* titulo) {
    uint8_t scratchpad[9];
    
    if (DS18B20_Read_Memory_Scratchpad(bus, scratchpad, rom_code)) {
        printf("\n========== %s ==========\n", titulo);
        printf("Byte 0 (LSB Temp): 0x%02X (%3d)\n", scratchpad[0], scratchpad[0]);
        printf("Byte 1 (MSB Temp): 0x%02X (%3d)\n", scratchpad[1], scratchpad[1]);
        printf("Byte 2 (TH):       0x%02X (%3d)  <- Límite superior de alarma\n", scratchpad[2], (int8_t)scratchpad[2]);
        printf("Byte 3 (TL):       0x%02X (%3d)  <- Límite inferior de alarma\n", scratchpad[3], (int8_t)scratchpad[3]);
        
        // Mostrar la resolución de forma legible
        const char* resolucion_str;
        switch (scratchpad[4]) {
            case DS18B20_REG_CONFIGURATION_9:  resolucion_str = "9 bits (0.5°C)"; break;
            case DS18B20_REG_CONFIGURATION_10: resolucion_str = "10 bits (0.25°C)"; break;
            case DS18B20_REG_CONFIGURATION_11: resolucion_str = "11 bits (0.125°C)"; break;
            case DS18B20_REG_CONFIGURATION_12: resolucion_str = "12 bits (0.0625°C)"; break;
            default: resolucion_str = "DESCONOCIDA"; break;
        }
        printf("Byte 4 (Config):   0x%02X       <- Resolución: %s\n", scratchpad[4], resolucion_str);
        printf("Byte 5 (Reserved): 0x%02X\n", scratchpad[5]);
        printf("Byte 6 (Reserved): 0x%02X\n", scratchpad[6]);
        printf("Byte 7 (Reserved): 0x%02X\n", scratchpad[7]);
        printf("Byte 8 (CRC):      0x%02X\n", scratchpad[8]);
        
        // Calcular y mostrar la temperatura actual
        int16_t raw_temp = (int16_t)((scratchpad[1] << 8) | scratchpad[0]);
        float temp = raw_temp * 0.0625f;
        printf("Temperatura actual: %.2f °C\n", temp);
        printf("====================================\n");
    } else {
        printf("ERROR: No se pudo leer el scratchpad para '%s'\n", titulo);
    }
}

// ============================================================
// 5. FUNCIÓN PARA LEER Y MOSTRAR TEMPERATURA
// ============================================================
void leer_y_mostrar_temperatura(OneWireBus_t* bus, uint8_t* rom_code, uint32_t contador) {
    float temp = DS18B20_Read_Temperature(bus, rom_code);
    if (temp < -100) {
        printf("[%3lu] ERROR: Código %.1f\n", contador, temp);
    } else {
        // Determinar si la temperatura está fuera de los límites de alarma
        uint8_t scratchpad[9];
        if (DS18B20_Read_Memory_Scratchpad(bus, scratchpad, rom_code)) {
            int8_t th = (int8_t)scratchpad[2];
            int8_t tl = (int8_t)scratchpad[3];
            
            printf("[%3lu] Temperatura: %6.2f °C", contador, temp);
            
            if (temp >= th) {
                printf(" 🔥 ¡ALARMA! T >= %d°C", th);
            } else if (temp <= tl) {
                printf(" ❄️ ¡ALARMA! T <= %d°C", tl);
            } else {
                printf(" ✅ OK (T entre %d°C y %d°C)", tl, th);
            }
            printf("\n");
        } else {
            printf("[%3lu] Temperatura: %6.2f °C\n", contador, temp);
        }
    }
}

// ============================================================
// 6. PROGRAMA PRINCIPAL
// ============================================================
int main(void) {
    // --- 6.1 Inicializar sistema ---
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    sleep_ms(5000);
    
    printf("\n\n");
    printf("============================================================\n");
    printf("   PRUEBA DS18B20 - CAMBIO DE CONFIGURACIÓN EN TIEMPO REAL\n");
    printf("============================================================\n");
    printf("Pin DQ: GPIO %d\n", ONEWIRE_PIN);
    
    // --- 6.2 Configurar el pin GPIO ---
    gpio_init(ONEWIRE_PIN);
    gpio_set_dir(ONEWIRE_PIN, GPIO_OUT);
    gpio_put(ONEWIRE_PIN, 1);
    
    // --- 6.3 Configurar el bus 1-Wire ---
    bus.pin_mode_output = pin_mode_output;
    bus.pin_mode_input = pin_mode_input;
    bus.pin_write_high = pin_write_high;
    bus.pin_write_low = pin_write_low;
    bus.pin_read = pin_read;
    bus.delay_us = delay_us;
    bus.mode_rate = ONE_WIRE_MODE_STANDARD;
    
    // --- 6.4 Inicializar el bus ---
    if (!OneWire_Init(&bus)) {
        printf("ERROR: No se pudo inicializar el bus 1-Wire\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Bus 1-Wire inicializado.\n");
    
    // --- 6.5 Verificar presencia del sensor ---
    if (!OneWire_Reset(&bus)) {
        printf("ERROR: No se detectó ningún dispositivo en el bus.\n");
        while (1) { tight_loop_contents(); }
    }
    printf("OK: Dispositivo detectado en el bus.\n");
    
    // ============================================================
    // 6.6 PASO 1: MOSTRAR CONFIGURACIÓN ACTUAL (cargada desde EEPROM)
    // ============================================================
    printf("\n--- PASO 1: CONFIGURACIÓN ACTUAL (desde EEPROM) ---\n");
    mostrar_scratchpad(&bus, NULL, "CONFIGURACIÓN INICIAL");
    
    // Extraer TH y TL de la configuración actual
    uint8_t scratchpad_actual[9];
    if (DS18B20_Read_Memory_Scratchpad(&bus, scratchpad_actual, NULL)) {
        uint8_t th_actual = scratchpad_actual[2];
        uint8_t tl_actual = scratchpad_actual[3];
        uint8_t resol_actual = scratchpad_actual[4];
        
        printf("\nConfiguración cargada desde EEPROM:\n");
        printf("  TH: %d°C\n", (int8_t)th_actual);
        printf("  TL: %d°C\n", (int8_t)tl_actual);
        printf("  Resolución: 0x%02X\n", resol_actual);
    }
    
    // ============================================================
    // 6.7 PASO 2: MEDIR TEMPERATURA DURANTE 10 SEGUNDOS
    // ============================================================
    printf("\n--- PASO 2: Midiendo temperatura durante 10 segundos ---\n");
    printf("(Observa cómo se comporta la alarma según la configuración actual)\n\n");
    
    uint32_t contador = 0;
    for (int i = 0; i < 10; i++) {
        gpio_put(LED_PIN, 1);
        leer_y_mostrar_temperatura(&bus, NULL, contador);
        gpio_put(LED_PIN, 0);
        contador++;
        sleep_ms(1000);
    }
    
    // ============================================================
    // 6.8 PASO 3: CAMBIAR LA CONFIGURACIÓN
    // ============================================================
    printf("\n--- PASO 3: Cambiando configuración en tiempo real ---\n");
    
    // Nueva configuración (valores diferentes a los actuales)
    uint8_t nuevo_th = 35;   // Alarma si T > 35°C
    uint8_t nuevo_tl = 10;   // Alarma si T < 10°C
    uint8_t nueva_resolucion = DS18B20_REG_CONFIGURATION_11;  // 11 bits
    
    printf("Nueva configuración a aplicar:\n");
    printf("  TH: %d°C\n", nuevo_th);
    printf("  TL: %d°C\n", (int8_t)nuevo_tl);
    printf("  Resolución: 11 bits (tiempo de conversión: 375 ms)\n");
    
    // Aplicar la nueva configuración
    if (DS18B20_Set_Config(&bus, nuevo_th, nuevo_tl, nueva_resolucion, NULL)) {
        printf("\n✅ Configuración cambiada exitosamente.\n");
    } else {
        printf("\n❌ Error al cambiar la configuración.\n");
        while (1) { tight_loop_contents(); }
    }
    
    // ============================================================
    // 6.9 PASO 4: MOSTRAR EL SCRATCHPAD DESPUÉS DEL CAMBIO
    // ============================================================
    printf("\n--- PASO 4: Verificando el scratchpad después del cambio ---\n");
    mostrar_scratchpad(&bus, NULL, "CONFIGURACIÓN MODIFICADA");
    
    // ============================================================
    // 6.10 PASO 5: COMPARAR CONFIGURACIONES (ANTES VS DESPUÉS)
    // ============================================================
    printf("\n--- PASO 5: COMPARACIÓN DE CONFIGURACIONES ---\n");
    
    uint8_t scratchpad_nuevo[9];
    if (DS18B20_Read_Memory_Scratchpad(&bus, scratchpad_nuevo, NULL)) {
        printf("Configuración ANTERIOR (desde EEPROM):\n");
        printf("  TH: %d°C\n", (int8_t)scratchpad_actual[2]);
        printf("  TL: %d°C\n", (int8_t)scratchpad_actual[3]);
        printf("  Resolución: 0x%02X\n", scratchpad_actual[4]);
        
        printf("\nConfiguración NUEVA (scratchpad):\n");
        printf("  TH: %d°C\n", (int8_t)scratchpad_nuevo[2]);
        printf("  TL: %d°C\n", (int8_t)scratchpad_nuevo[3]);
        printf("  Resolución: 0x%02X\n", scratchpad_nuevo[4]);
        
        // Verificar si los valores coinciden con lo que esperábamos
        if (scratchpad_nuevo[2] == nuevo_th && 
            scratchpad_nuevo[3] == nuevo_tl && 
            scratchpad_nuevo[4] == nueva_resolucion) {
            printf("\n✅ La nueva configuración coincide con lo esperado.\n");
        } else {
            printf("\n❌ La nueva configuración NO coincide con lo esperado.\n");
        }
        
        // Verificar si la configuración anterior era la que estaba en EEPROM
        // (Nota: esto solo funciona si la EEPROM no ha cambiado desde el inicio)
        if (scratchpad_actual[2] == scratchpad_nuevo[2] &&
            scratchpad_actual[3] == scratchpad_nuevo[3] &&
            scratchpad_actual[4] == scratchpad_nuevo[4]) {
            printf("\n⚠️ La configuración NO cambió (los valores son iguales).\n");
        } else {
            printf("\n✅ La configuración ha cambiado correctamente.\n");
        }
    }
    
    // ============================================================
    // 6.11 PASO 6: MEDIR TEMPERATURA CON LA NUEVA CONFIGURACIÓN
    // ============================================================
    printf("\n--- PASO 6: Midiendo temperatura con la NUEVA configuración ---\n");
    printf("(Observa cómo cambian los límites de alarma)\n\n");
    
    for (int i = 0; i < 10; i++) {
        gpio_put(LED_PIN, 1);
        leer_y_mostrar_temperatura(&bus, NULL, contador);
        gpio_put(LED_PIN, 0);
        contador++;
        sleep_ms(1000);
    }
    
    // ============================================================
    // 6.12 PASO 7: OPCIÓN PARA GUARDAR LA NUEVA CONFIGURACIÓN
    // ============================================================
    printf("\n--- PASO 7: ¿Guardar la nueva configuración en EEPROM? ---\n");
    printf("La nueva configuración solo está en el scratchpad.\n");
    printf("Si quieres que persista después de apagar el sensor,\n");
    printf("debes llamar a DS18B20_Save_Config().\n");
    printf("\nPresiona 's' para guardar, o cualquier otra tecla para continuar sin guardar.\n");
    
    char c = getchar();
    if (c == 's' || c == 'S') {
        if (DS18B20_Save_Config(&bus, NULL)) {
            printf("\n✅ Configuración guardada en EEPROM correctamente.\n");
        } else {
            printf("\n❌ Error al guardar la configuración en EEPROM.\n");
        }
    } else {
        printf("\nContinuando sin guardar la configuración en EEPROM.\n");
    }
    
    // ============================================================
    // 6.13 BUCLE INFINITO DE LECTURA DE TEMPERATURA
    // ============================================================
    printf("\n--- BUCLE INFINITO DE LECTURA ---\n");
    while (1) {
        gpio_put(LED_PIN, 1);
        leer_y_mostrar_temperatura(&bus, NULL, contador);
        gpio_put(LED_PIN, 0);
        contador++;
        sleep_ms(1000);
    }
    
    return 0;
}