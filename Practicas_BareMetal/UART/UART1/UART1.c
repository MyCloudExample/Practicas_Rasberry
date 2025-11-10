#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
/*GPIO15(RX) AMARILLO ; GPIO14(TX) NARANJA PARA RASPBERRY PI 3*/
#define UART_ID uart1  // Cambiamos a uart1 para usar GPIO4 y GPIO5
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// GPIO4 para TX, GPIO5 para RX
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define LED_PIN 25

// Comandos esperados
const char *CMD_LED_ON = "LED_ON";
const char *CMD_LED_OFF = "LED_OFF";

void setup_uart() {
    // Inicializar UART1
    uart_init(UART_ID, BAUD_RATE);
    
    // Configurar pines GPIO4 y GPIO5 para UART1
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Configurar formato UART
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    
    // Habilitar FIFO
    uart_set_fifo_enabled(UART_ID, true);
    
    printf("UART1 configurado en GPIO4(TX) y GPIO5(RX)\n");
}

void process_command(char *cmd) {
    // Eliminar salto de línea si existe
    char *pos;
    if ((pos = strchr(cmd, '\n')) != NULL) {
        *pos = '\0';
    }
    if ((pos = strchr(cmd, '\r')) != NULL) {
        *pos = '\0';
    }
    
    printf("Comando recibido: %s\n", cmd);
    
    if (strcmp(cmd, CMD_LED_ON) == 0) {
        gpio_put(LED_PIN, 1);
        printf("LED ENCENDIDO\n");
    } else if (strcmp(cmd, CMD_LED_OFF) == 0) {
        gpio_put(LED_PIN, 0);
        printf("LED APAGADO\n");
    } else {
        printf("Comando desconocido: %s\n", cmd);
    }
}

int main() {
    stdio_init_all();
    
    // Configurar LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    setup_uart();
    
    printf("Pico listo para recibir comandos por UART1 (GPIO4-5)...\n");
    
    char buffer[256];
    int index = 0;
    
    while (true) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            
            if (c == '\n' || c == '\r') {
                if (index > 0) {
                    buffer[index] = '\0';
                    process_command(buffer);
                    index = 0;
                }
            } else if (index < sizeof(buffer) - 1) {
                buffer[index++] = c;
            }
        }
        
        sleep_ms(10);
    }
    
    return 0;
}