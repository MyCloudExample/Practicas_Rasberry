#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
//#include "tusb.h"

#define RELAY_PIN 25 // Pin GPIO conectado al módulo de relé

int main() {
    // === INICIALIZAR USB CDC - FALTABA ESTO ===
    stdio_init_all();
    
    // Esperar a que USB esté conectado (opcional pero recomendado)
    /*while (!tud_cdc_connected()) {
        sleep_ms(100);
    }*/
    
    printf("=== RELE INICIADO ===\n");
    printf("USB CDC funcionando correctamente\n");

    // Inicializar el pin del relé como salida
    gpio_init(RELAY_PIN);
    gpio_set_dir(RELAY_PIN, GPIO_OUT);

    while (true) {
        // Encender el relé (activar)
        gpio_put(RELAY_PIN, 1);
        sleep_ms(500); // Espera 0.5 segundos
        printf("RELE ENCENDIDO \n");

        // Apagar el relé (desactivar)
        gpio_put(RELAY_PIN, 0);
        sleep_ms(500); // Espera 0.5 segundos
        printf("RELE APAGADO \n");
    }

    return 0;
}