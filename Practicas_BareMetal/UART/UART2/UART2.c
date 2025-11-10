/*Este codigo se utilizo para verificar una pposible tarea que se adicionara a la EGA. Consistene en recibir por UART una cadena
*de caracteres la cual contiene el seteo del Levitador Neumatico, la cadena se forma de la siguiente maenra:
*   Setpoint:20,SetpointM:21,Setpointm:19
*el algoritmo seberae separar cada parametro y asignarlo al parametro que se indica, por ejemplo deberar tomar el valor 20 que 
*es del setpoint y colocarlo en el parametro corrspondiente.*/
/*===========================ARCHIVOS DE CABECERA================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
/*===============================================================================================================================*/
/*===========================DEFINICIONES================================================================================*/
#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define BUFFER_SIZE 256
/*===============================================================================================================================*/
/*===========================CREACION DE ESTRUCUTRAS DE DATOS====================================================================*/
typedef struct
{
    uint32_t setpoint;
    float setpoint_min;
    float setpoint_max;
    float altura_medida;
    uint8_t linea;      // Nos posicionamos en la linea 4 e indicamos la posicion en la linea 4
    uint8_t guardado;   // Se utiliza para indicar altura superada
} estructura_setpoint;  // Estructura para almacenar los parámetros
/*===============================================================================================================================*/
/*===========================DECLARACION DE VARIABLES GLOBALES===================================================================*/
estructura_setpoint config = {
    .setpoint = 0,
    .setpoint_min = 0.0f,
    .setpoint_max = 0.0f,
    .altura_medida = 0.0f,
    .linea = 0,
    .guardado = 0
};                               // Variable global de la estructura

char uart_buffer[BUFFER_SIZE];  // Buffer para recibir datos UART
uint16_t buffer_index = 0;
/*===============================================================================================================================*/
/*===========================DECLARACION DE FUNCIONES============================================================================*/
//===========================Función para extraer números de una cadena==========================================================
float extract_number(const char *str) 
{
    char temp[32];
    int j = 0;
    int found_digit = 0;
    
    // Buscar dígitos, signo y punto decimal
    for (int i = 0; str[i] != '\0' && j < 31; i++) 
    {
        if ((str[i] >= '0' && str[i] <= '9') || 
            str[i] == '.' || 
            (str[i] == '-' && !found_digit)) 
            {
            temp[j++] = str[i];
            found_digit = 1;
        } 
        else if (found_digit) 
        {
            break; // Terminamos cuando encontramos un número completo
        }
    }
    temp[j] = '\0';
    
    return atof(temp);
}
//===========================Función para extraer números enteros (para setpoint)================================================
uint32_t extract_integer(const char *str) 
{
    char temp[32];
    int j = 0;
    int found_digit = 0;
    
    // Buscar solo dígitos para enteros
    for (int i = 0; str[i] != '\0' && j < 31; i++)
    {
        if (str[i] >= '0' && str[i] <= '9') 
        {
            temp[j++] = str[i];
            found_digit = 1;
        } 
        else if (found_digit) 
        {
            break; // Terminamos cuando encontramos un número completo
        }
    }
    temp[j] = '\0';
    
    return (uint32_t)atoi(temp);
}
//===========================Función para procesar el comando recibido===========================================================
void process_command(const char *command) 
{
    printf("Comando recibido: %s\n", command);
    
    char *token;
    char working_buffer[BUFFER_SIZE];
    
    strcpy(working_buffer, command);
    token = strtok(working_buffer, ",");
    
    while (token != NULL) {
        char *clean_token = token;
        while (*clean_token == ' ') clean_token++;
        
        // FORMATO EXTENDIDO
        if (strstr(clean_token, "Setpoint:") != NULL) {
            config.setpoint = extract_integer(clean_token);
            printf("Setpoint configurado: %lu\n", config.setpoint);
        }
        else if (strstr(clean_token, "Setpointm:") != NULL) {
            config.setpoint_min = extract_number(clean_token);
            printf("Setpoint_min configurado: %.2f\n", config.setpoint_min);
        }
        else if (strstr(clean_token, "SetpointM:") != NULL) {
            config.setpoint_max = extract_number(clean_token);
            printf("Setpoint_max configurado: %.2f\n", config.setpoint_max);
        }
        // FORMATO COMPACTO
        else if (strstr(clean_token, "SP:") != NULL) {
            config.setpoint = extract_integer(clean_token);
            printf("Setpoint configurado: %lu\n", config.setpoint);
        }
        else if (strstr(clean_token, "Sm:") != NULL) {
            config.setpoint_min = extract_number(clean_token);
            printf("Setpoint_min configurado: %.2f\n", config.setpoint_min);
        }
        else if (strstr(clean_token, "SM:") != NULL) {
            config.setpoint_max = extract_number(clean_token);
            printf("Setpoint_max configurado: %.2f\n", config.setpoint_max);
        }
        else if (strstr(clean_token, "Altura:") != NULL) {
            config.altura_medida = extract_number(clean_token);
            printf("Altura_medida configurada: %.2f\n", config.altura_medida);
        }
        
        token = strtok(NULL, ",");
    }
    
    printf("Valores actuales en estructura:\n");
    printf("  Setpoint: %lu\n", config.setpoint);
    printf("  Setpoint_min: %.2f\n", config.setpoint_min);
    printf("  Setpoint_max: %.2f\n", config.setpoint_max);
    printf("  Altura_medida: %.2f\n", config.altura_medida);
    printf("  Linea: %u\n", config.linea);
    printf("  Guardado: %u\n\n", config.guardado);
}
//===========================Función para mostrar el estado actual de la estructura==============================================
void mostrar_estado_actual() 
{
    printf("=== ESTADO ACTUAL DE CONFIGURACION ===\n");
    printf("Setpoint: %lu\n", config.setpoint);
    printf("Setpoint_min: %.2f\n", config.setpoint_min);
    printf("Setpoint_max: %.2f\n", config.setpoint_max);
    printf("Altura_medida: %.2f\n", config.altura_medida);
    printf("Linea: %u\n", config.linea);
    printf("Guardado: %u\n");
    printf("=======================================\n\n");
}
//===========================Función para inicializar UART=======================================================================
void uart_init_config() 
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    //uart_set_fifo_enabled(UART_ID, true);
    
    printf("Sistema inicializado. Esperando comandos...\n");
    printf("Formato: \"Setpoint: valor , SetpointM: valor , Setpointm: valor , Altura: valor\"\n");
    printf("Ejemplo: \"Setpoint: 100 , SetpointM: 25.5 , Setpointm: 10.2 , Altura: 15.7\"\n\n");
    // DEBUG CRÍTICO
    sleep_ms(5000);
    printf("=== UART1 CONFIGURADO ===\n");
    printf("UART: uart1\n");
    printf("GPIO TX: %d\n", UART_TX_PIN);
    printf("GPIO RX: %d\n", UART_RX_PIN); 
    printf("Baud rate: %d\n", BAUD_RATE);
    printf("=========================\n\n");
    
    printf("Esperando comandos...\n");
    mostrar_estado_actual();
}
/*===============================================================================================================================*/
/*===========================PROGRAMA PRINCIPAÑ==================================================================================*/
int main() 
{
    stdio_init_all();
    uart_init_config();
    
    // DEBUG: Inicializar LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0); // Inicialmente apagado

    // Limpiar buffer
    memset(uart_buffer, 0, BUFFER_SIZE);
    buffer_index = 0;
    absolute_time_t last_data_time = get_absolute_time();
    
    printf("=== SISTEMA INICIADO ===\n");
    
    while (true) {
        // Leer datos disponibles
        while (uart_is_readable(UART_ID) && buffer_index < BUFFER_SIZE - 1) {
            gpio_put(25, 1); // LED ON cuando recibe
            char c = uart_getc(UART_ID);
            uart_buffer[buffer_index++] = c;
            last_data_time = get_absolute_time();
            
            if (c == '\n' || c == '\r') {
                uart_buffer[buffer_index - 1] = '\0';
                printf("DEBUG: Comando por newline: '%s'\n", uart_buffer);
                process_command(uart_buffer);
                buffer_index = 0;
                memset(uart_buffer, 0, BUFFER_SIZE);
                gpio_put(25, 0); // LED OFF después de procesar
                break;
            }
        }
        
        // Timeout: si pasaron 50ms sin datos, procesar lo acumulado
        if (buffer_index > 0 && absolute_time_diff_us(last_data_time, get_absolute_time()) > 50000) {
            uart_buffer[buffer_index] = '\0';
            printf("DEBUG: Comando por timeout: '%s'\n", uart_buffer);
            process_command(uart_buffer);
            buffer_index = 0;
            memset(uart_buffer, 0, BUFFER_SIZE);
            gpio_put(25, 0); // LED OFF
        }
        
        // Mostrar estado cada 10 segundos
        static absolute_time_t last_display = 0;
        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_display, now) > 10000000) {
            printf("Debug: uart_is_readable() = %d, buffer_index = %d\n", uart_is_readable(UART_ID), buffer_index);
            mostrar_estado_actual();
            last_display = now;
        }
        
        sleep_ms(10);
    }
    
    return 0;
}