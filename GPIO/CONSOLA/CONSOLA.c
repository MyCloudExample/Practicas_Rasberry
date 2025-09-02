#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    // Inicializar UART (USB por defecto en GPIO0 TX y GPIO1 RX)
    stdio_init_all();

    while (true) {
        printf("Hola desde Raspberry Pi Pico!\n");  // Mensaje a imprimir
        sleep_ms(1000);  // Espera 1 segundo
    }
    return 0;
}