#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// Define el UART a usar (UART0)
#define UART_ID uart0
#define BAUD_RATE 115200

// ******************************************************
// ** CAMBIO SOLICITADO: PINES GPIO 4 (TX) y 5 (RX) **
// ******************************************************
#define UART_TX_PIN 4  // <--- Nuevo pin TX
#define UART_RX_PIN 5  // <--- Nuevo pin RX
// ******************************************************

// Buffer de recepción para la línea
#define RX_BUF_SIZE 64
char rx_buffer[RX_BUF_SIZE];
int rx_index = 0;

/**
 * @brief Manejador de interrupción del UART.
 * Lee bytes del UART y los almacena en el buffer hasta encontrar '\n'.
 */
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        char ch = uart_getc(UART_ID);

        // Si es un carácter de nueva línea o retorno de carro
        if (ch == '\n' || ch == '\r') {
            // Null-terminate el buffer. 
            if (rx_index < RX_BUF_SIZE) {
                rx_buffer[rx_index] = '\0';
                // Si recibimos "PING#", respondemos con "PONG\n"
                if (strstr(rx_buffer, "PING#") != NULL) {
                    uart_puts(UART_ID, "PONG\n");
                // También enviamos un ACK para las otras pruebas
                } else if (strstr(rx_buffer, "#") != NULL) {
                    uart_puts(UART_ID, "ACK#\n");
                }
            }
            // Reiniciar buffer para la próxima línea
            rx_index = 0;
        } else {
            // Almacenar carácter si hay espacio
            if (rx_index < RX_BUF_SIZE - 1) {
                rx_buffer[rx_index++] = ch;
            } else {
                // Buffer lleno, descartar el resto de la línea
                rx_index = 0;
            }
        }
    }
}

int main() {
    stdio_init_all();
    
    // Configurar UART0
    uart_init(UART_ID, BAUD_RATE);
    // Mapear el UART0 a los nuevos pines
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Configurar interrupción RX para recibir datos
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Habilitar interrupción de recepción (solo cuando se reciben datos)
    uart_set_irq_enables(UART_ID, true, false);

    printf("Pico Responder: Listo para recibir comandos en UART0 (115200 baudios).\n");

    while (true) {
        tight_loop_contents();
    }

    return 0;
}