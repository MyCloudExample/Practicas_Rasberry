#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "one_wire.h"

#define ONE_WIRE_PIN 16  // Asegúrate que coincide con tu conexión física

float read_ds18b20(one_wire_t *ow) {
    uint8_t data[9];
    float temp = -1000.0f; // Valor de error

    // 1. Reset del bus
    if (!one_wire_reset(ow)) {
        printf("Sensor no respondió al reset\n");
        return temp;
    }

    // 2. Skip ROM (0xCC) - Para un solo dispositivo
    one_wire_write(ow, 0xCC);
    
    // 3. Convert T (0x44) - Inicia conversión
    one_wire_write(ow, 0x44);
    sleep_ms(750);  // Espera conversión (750ms para 12-bit)

    // 4. Reset nuevamente
    if (!one_wire_reset(ow)) {
        printf("Sensor no respondió después de conversión\n");
        return temp;
    }

    // 5. Skip ROM (0xCC) otra vez
    one_wire_write(ow, 0xCC);
    
    // 6. Read Scratchpad (0xBE)
    one_wire_write(ow, 0xBE);
    
    // 7. Leer 9 bytes
    for (int i = 0; i < 9; i++) {
        data[i] = one_wire_read(ow);
    }

    // 8. Verificar CRC
    if (one_wire_crc8(data, 8) != data[8]) {
        printf("Error de CRC\n");
        return temp;
    }

    // 9. Calcular temperatura
    int16_t raw = (data[1] << 8) | data[0];
    return (float)raw / 16.0f;
}

int main() {
    stdio_init_all();
    printf("\nIniciando sensor DS18B20...\n");

    one_wire_t ow;
    one_wire_init(&ow, ONE_WIRE_PIN);

    while (true) {
        float temp = read_ds18b20(&ow);
        
        if (temp > -1000.0f) {
            printf("Temperatura: %.2f °C\n", temp);
        } else {
            printf("Error en lectura\n");
        }

        sleep_ms(2000);  // Lectura cada 2 segundos
    }
}