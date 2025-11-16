#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5

int main() {
    stdio_init_all();
    
    // Inicializar UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    printf("=== PICO UART RECEIVER ===\n");
    printf("UART1: GPIO4(TX), GPIO5(RX)\n");
    printf("Baud rate: %d\n", BAUD_RATE);
    printf("Esperando datos...\n\n");
    
    while (true) {
        // Verificar si hay datos disponibles
        if (uart_is_readable(UART_ID)) {
            // Leer todos los caracteres disponibles
            while (uart_is_readable(UART_ID)) {
                char c = uart_getc(UART_ID);
                
                // Mostrar cada caracter con formato claro
                if (c >= 32 && c <= 126) {
                    // Caracter imprimible
                    printf("['%c'] ", c);
                } else {
                    // Caracter no imprimible
                    switch (c) {
                        case '\n': printf("[\\n] "); break;
                        case '\r': printf("[\\r] "); break;
                        case '\t': printf("[\\t] "); break;
                        default: printf("[0x%02X] ", c); break;
                    }
                }
                
                // Forzar flush para ver inmediatamente
                fflush(stdout);
            }
            printf("\n"); // Nueva línea después de cada grupo de caracteres
        }
        
        sleep_ms(10);
    }
    
    return 0;
}