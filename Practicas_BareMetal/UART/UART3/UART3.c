/*Este codigo se utilizo para verificar una pposible tarea que se adicionara a la EGA. Consistene en recibir por UART una cadena
*de caracteres la cual contiene el seteo del Levitador Neumatico, la cadena se forma de la siguiente maenra:
*   SP:20,SM:21,Sm:19
*el algoritmo debera separar cada parametro y asignarlo al parametro que se indica, por ejemplo deberar tomar el valor 20 que 
*es del setpoint y colocarlo en el parametro corrspondiente. Esta es un version que utiliza el UART por interrupcion.*/
/*===========================ARCHIVOS DE CABECERA================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
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
volatile uint16_t buffer_index = 0;
volatile bool data_ready = false;  // Flag para indicar comando listo
/*===============================================================================================================================*/
/*===========================DECLARACION DE FUNCIONES============================================================================*/
//===========================Función de interrupción UART =======================================================================
void on_uart_rx() 
{
    while (uart_is_readable(UART_ID)) 
    {
        char c = uart_getc(UART_ID);
        
        if (c == '\n' || c == '\r') 
        {
            // Fin de comando
            if (buffer_index > 0) 
            {
                uart_buffer[buffer_index] = '\0';
                data_ready = true;  // Marcar que hay comando listo
                buffer_index = 0;
            }
        } 
        else if (buffer_index < BUFFER_SIZE - 1) 
        {
            // Almacenar caracter en buffer
            uart_buffer[buffer_index++] = c;
        } 
        else 
        {
            // Buffer lleno, procesar igual
            uart_buffer[BUFFER_SIZE - 1] = '\0';
            data_ready = true;
            buffer_index = 0;
        }
    }
}
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
    
    // Configurar interrupción por recepción de datos
    uart_set_irq_enables(UART_ID, true, false); // Habilitar solo interrupción RX
    irq_set_exclusive_handler(UART1_IRQ, on_uart_rx);
    irq_set_enabled(UART1_IRQ, true);
    
    printf("Sistema inicializado. Esperando comandos por interrupción...\n");
    printf("Formato: \"Setpoint: valor , SetpointM: valor , Setpointm: valor , Altura: valor\"\n");
    printf("Ejemplo: \"Setpoint: 100 , SetpointM: 25.5 , Setpointm: 10.2 , Altura: 15.7\"\n\n");
    
    printf("=== UART1 CONFIGURADO CON INTERRUPCIONES ===\n");
    printf("UART: uart1\n");
    printf("GPIO TX: %d\n", UART_TX_PIN);
    printf("GPIO RX: %d\n", UART_RX_PIN); 
    printf("Baud rate: %d\n", BAUD_RATE);
    printf("=========================\n\n");
    
    printf("Esperando comandos...\n");
    mostrar_estado_actual();
}
/*===============================================================================================================================*/
/*===========================PROGRAMA PRINCIPAL==================================================================================*/
int main() 
{
    stdio_init_all();
    uart_init_config();
    
    // Inicializar LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0); // Inicialmente apagado

    // Limpiar buffer
    memset(uart_buffer, 0, BUFFER_SIZE);
    buffer_index = 0;
    data_ready = false;
    
    printf("=== SISTEMA INICIADO CON INTERRUPCIONES ===\n");
    
    while (true) 
    {
        // El loop principal está LIBRE para hacer otras tareas
        // Solo verifica si hay un comando listo para procesar
        
        if (data_ready) 
        {
            gpio_put(25, 1); // LED ON cuando procesa
            process_command(uart_buffer);
            data_ready = false;
            gpio_put(25, 0); // LED OFF después de procesar
            
            // Mostrar estado después de procesar comando
            mostrar_estado_actual();
        }
        
        // Aquí puedes agregar otras tareas del sistema
        // Ej: control del levitador, lectura de sensores, etc.
        // sleep_ms(100); // O cualquier otra lógica
        
        // Mostrar estado periódicamente (cada 30 segundos en lugar de 10)
        static absolute_time_t last_display = 0;
        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_display, now) > 30000000) 
        { // 30 segundos
            printf("Sistema activo - Esperando comandos...\n");
            mostrar_estado_actual();
            last_display = now;
        }
        
        sleep_ms(10);
    }
    
    return 0;
}