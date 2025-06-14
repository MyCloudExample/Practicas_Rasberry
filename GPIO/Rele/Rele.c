#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define RELAY_PIN 15 // Pin GPIO conectado al módulo de relé

int main() {
    // Inicializar el pin del relé como salida
    gpio_init(RELAY_PIN);
    gpio_set_dir(RELAY_PIN, GPIO_OUT);

    while (true) {
        // Encender el relé (activar)
        gpio_put(RELAY_PIN, 1);
        sleep_ms(2000); // Espera 2 segundos

        // Apagar el relé (desactivar)
        gpio_put(RELAY_PIN, 0);
        sleep_ms(2000); // Espera 2 segundos
    }

    return 0;
}