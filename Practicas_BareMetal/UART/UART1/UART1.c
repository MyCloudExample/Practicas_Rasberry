#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

// Configuración UART1
#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// Pines UART1 (GPIO4 y GPIO5)
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// LED de la Pico
#define LED_PIN 25

// Comandos
#define LED_ON_CMD "LED_ON"
#define LED_OFF_CMD "LED_OFF"

void setup_uart() {
    // Inicializar UART1
    uart_init(UART_ID, BAUD_RATE);
    
    // Configurar pines GPIO para UART1
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Configurar formato UART
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    
    // Habilitar FIFOs
    uart_set_fifo_enabled(UART_ID, true);
    
    printf("UART1 inicializado en GPIO%d (TX) y GPIO%d (RX)\n", UART_TX_PIN, UART_RX_PIN);
}

void setup_led() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // LED apagado inicialmente
    printf("LED configurado en GPIO%d\n", LED_PIN);
}

void process_command(char* command) {
    if (strcmp(command, LED_ON_CMD) == 0) {
        gpio_put(LED_PIN, 1);
        printf("Comando recibido: LED_ON - LED ENCENDIDO\n");
        uart_puts(UART_ID, "LED_ON_OK\n");
    } 
    else if (strcmp(command, LED_OFF_CMD) == 0) {
        gpio_put(LED_PIN, 0);
        printf("Comando recibido: LED_OFF - LED APAGADO\n");
        uart_puts(UART_ID, "LED_OFF_OK\n");
    }
    else {
        printf("Comando desconocido: '%s'\n", command);
        uart_puts(UART_ID, "ERROR: Comando desconocido\n");
    }
}

int main() {
    // Inicializar SDK - USB para printf
    stdio_init_all();
    
    // Configurar hardware
    setup_uart();
    setup_led();
    
    // Buffer para recibir datos
    char buffer[64];
    int index = 0;
    
    printf("=== Raspberry Pico UART1 Listener ===\n");
    printf("Esperando comandos por UART1...\n");
    printf("Comandos validos: LED_ON, LED_OFF\n");
    printf("UART1: GPIO%d (RX), GPIO%d (TX)\n", UART_RX_PIN, UART_TX_PIN);
    
    while (true) {
        // Verificar si hay datos disponibles en UART1
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            
            // Echo del carácter recibido (opcional)
            // uart_putc(UART_ID, c);
            
            if (c == '\n' || c == '\r') {
                // Fin de comando
                if (index > 0) {
                    buffer[index] = '\0';
                    process_command(buffer);
                    index = 0; // Resetear buffer
                }
            } 
            else if (index < sizeof(buffer) - 1) {
                // Agregar carácter al buffer (ignorar si está lleno)
                buffer[index++] = c;
            }
        }
        
        // Pequeña pausa para evitar uso excesivo de CPU
        sleep_ms(1);
    }
    
    return 0;
}