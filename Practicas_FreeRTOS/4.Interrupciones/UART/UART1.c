/*Este codigo se utilizo para verificar una pposible tarea que se adicionara a la EGA. Consistene en recibir por UART una cadena
*de caracteres la cual contiene el seteo del Levitador Neumatico*/
/*===========================ARCHIVOS DE CABECERA================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
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
#define COMMAND_QUEUE_SIZE 1

// Prioridades de tareas FreeRTOS
#define TASK_PRIORITY_PROCESS_COMMAND    (tskIDLE_PRIORITY + 2)
#define TASK_PRIORITY_STATUS_DISPLAY     (tskIDLE_PRIORITY + 1)
/*===============================================================================================================================*/
/*===========================CREACION DE ESTRUCUTRAS DE DATOS====================================================================*/
typedef struct
{
    uint32_t setpoint;
    float setpoint_min;
    float setpoint_max;
    float altura_medida;
    uint8_t linea;
    uint8_t guardado;
} estructura_setpoint;

typedef struct {
    char command[BUFFER_SIZE];
} command_message_t;
/*===============================================================================================================================*/
/*===========================DECLARACION DE VARIABLES GLOBALES===================================================================*/
estructura_setpoint config = {
    .setpoint = 0,
    .setpoint_min = 0.0f,
    .setpoint_max = 0.0f,
    .altura_medida = 0.0f,
    .linea = 0,
    .guardado = 0
};

// Variables FreeRTOS
QueueHandle_t xCommandQueue;
SemaphoreHandle_t xConfigMutex;
TaskHandle_t xProcessCommandTaskHandle;
TaskHandle_t xStatusDisplayTaskHandle;

// Buffer circular para recepción UART
static char uart_rx_buffer[BUFFER_SIZE];
static volatile uint16_t uart_buffer_index = 0;
/*===============================================================================================================================*/
/*===========================DECLARACION DE FUNCIONES (PROTOTIPOS)===============================================================*/
// Funciones de procesamiento (se definen después)
void process_command(const char *command);
void mostrar_estado_actual(void);
float extract_number(const char *str);
uint32_t extract_integer(const char *str);

// Funciones FreeRTOS
void vProcessCommandTask(void *pvParameters);
void vStatusDisplayTask(void *pvParameters);
void init_freertos(void);
void uart_init_config(void);
/*===========================DECLARACION DE FUNCIONES============================================================================*/

//===========================INTERRUPCIÓN UART (MÍNIMA)===========================================================================
void __not_in_flash_func(on_uart_rx)() 
{
    // Solo leer el carácter y ponerlo en la cola
    while (uart_is_readable(UART_ID)) {
        char c = uart_getc(UART_ID);
        
        // Manejo básico del buffer en la interrupción
        if (c == '\n' || c == '\r') 
        {
            if (uart_buffer_index > 0) 
            {
                // Crear mensaje para la tarea
                command_message_t message;
                strncpy(message.command, uart_rx_buffer, uart_buffer_index);
                message.command[uart_buffer_index] = '\0';
                
                // Enviar a la cola (NO BLOQUEANTE en interrupción)
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xQueueSendFromISR(xCommandQueue, &message, &xHigherPriorityTaskWoken);
                
                // Resetear buffer
                uart_buffer_index = 0;
                
                // Yield si se despertó una tarea de mayor prioridad
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } 
        else if (uart_buffer_index < BUFFER_SIZE - 1) 
        {
            uart_rx_buffer[uart_buffer_index++] = c;
        } 
        else 
        {
            // Buffer lleno, resetear
            uart_buffer_index = 0;
        }
    }
}

//===========================TAREA: Procesamiento de Comandos====================================================================
void vProcessCommandTask(void *pvParameters) 
{
    command_message_t received_message;
    
    printf("Tarea de procesamiento de comandos iniciada\n");
    
    for (;;) 
    {
        // Esperar por comandos (BLOQUEANTE - eficiente)
        if (xQueueReceive(xCommandQueue, &received_message, portMAX_DELAY) == pdTRUE) 
        {
            // LED ON
            gpio_put(25, 1);
            
            printf("Tarea: Comando recibido: %s\n", received_message.command);
            
            // Tomar mutex para acceso seguro a la estructura config
            if (xSemaphoreTake(xConfigMutex, portMAX_DELAY) == pdTRUE) 
            {
                process_command(received_message.command);
                xSemaphoreGive(xConfigMutex);
            }
            
            // LED OFF
            gpio_put(25, 0);
        }
    }
}

//===========================TAREA: Mostrar Estado================================================================================
void vStatusDisplayTask(void *pvParameters) 
{
    const TickType_t xDelay = pdMS_TO_TICKS(30000); // 30 segundos
    
    printf("Tarea de visualización de estado iniciada\n");
    
    for (;;) 
    {
        vTaskDelay(xDelay);
        
        if (xSemaphoreTake(xConfigMutex, portMAX_DELAY) == pdTRUE) 
        {
            printf("=== ESTADO DEL SISTEMA FreeRTOS ===\n");
            printf("Tareas activas:\n");
            printf("Comandos en cola: %d\n", uxQueueMessagesWaiting(xCommandQueue));
            printf("Tiempo activo: %lu ticks\n", xTaskGetTickCount());
            mostrar_estado_actual();
            xSemaphoreGive(xConfigMutex);
        }
    }
}

//===========================Función para extraer números de una cadena==========================================================
float extract_number(const char *str) 
{
    char temp[32];
    int j = 0;
    int found_digit = 0;
    
    for (int i = 0; str[i] != '\0' && j < 31; i++) {
        if ((str[i] >= '0' && str[i] <= '9') || 
            str[i] == '.' || 
            (str[i] == '-' && !found_digit)) {
            temp[j++] = str[i];
            found_digit = 1;
        } else if (found_digit) {
            break;
        }
    }
    temp[j] = '\0';
    
    return atof(temp);
}

//===========================Función para extraer números enteros================================================
uint32_t extract_integer(const char *str) {
    char temp[32];
    int j = 0;
    int found_digit = 0;
    
    for (int i = 0; str[i] != '\0' && j < 31; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            temp[j++] = str[i];
            found_digit = 1;
        } else if (found_digit) {
            break;
        }
    }
    temp[j] = '\0';
    
    return (uint32_t)atoi(temp);
}

//===========================Función para procesar el comando recibido===========================================================
void process_command(const char *command) 
{
    printf("Procesando comando: %s\n", command);
    
    char *token;
    char working_buffer[BUFFER_SIZE];
    
    strcpy(working_buffer, command);
    token = strtok(working_buffer, ",");
    
    while (token != NULL) 
    {
        char *clean_token = token;
        while (*clean_token == ' ') clean_token++;
        
        if (strstr(clean_token, "Setpoint:") != NULL || strstr(clean_token, "SP:") != NULL) 
        {
            config.setpoint = extract_integer(clean_token);
            printf("Setpoint configurado: %lu\n", config.setpoint);
        }
        else if (strstr(clean_token, "Setpointm:") != NULL || strstr(clean_token, "Sm:") != NULL) 
        {
            config.setpoint_min = extract_number(clean_token);
            printf("Setpoint_min configurado: %.2f\n", config.setpoint_min);
        }
        else if (strstr(clean_token, "SetpointM:") != NULL || strstr(clean_token, "SM:") != NULL) 
        {
            config.setpoint_max = extract_number(clean_token);
            printf("Setpoint_max configurado: %.2f\n", config.setpoint_max);
        }
        else if (strstr(clean_token, "Altura:") != NULL) 
        {
            config.altura_medida = extract_number(clean_token);
            printf("Altura_medida configurada: %.2f\n", config.altura_medida);
        }
        
        token = strtok(NULL, ",");
    }
    
    printf("Valores actualizados en estructura\n");
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
    
    // Configurar interrupción UART
    uart_set_irq_enables(UART_ID, true, false);
    irq_set_exclusive_handler(UART1_IRQ, on_uart_rx);
    irq_set_enabled(UART1_IRQ, true);
    
    printf("UART1 configurado con interrupciones para FreeRTOS\n");
}

//===========================Inicialización FreeRTOS=============================================================================
void init_freertos() 
{
    // Crear cola para comandos
    xCommandQueue = xQueueCreate(COMMAND_QUEUE_SIZE, sizeof(command_message_t));
    
    // Crear mutex para acceso a configuración
    xConfigMutex = xSemaphoreCreateMutex();
    
    if (xCommandQueue == NULL || xConfigMutex == NULL) 
    {
        printf("Error creando objetos FreeRTOS\n");
        return;
    }
    
    // Crear tareas
    xTaskCreate(vProcessCommandTask, "ProcessCmd", 1024, NULL, TASK_PRIORITY_PROCESS_COMMAND, &xProcessCommandTaskHandle);
    xTaskCreate(vStatusDisplayTask, "StatusDisplay", 512, NULL, TASK_PRIORITY_STATUS_DISPLAY, &xStatusDisplayTaskHandle);
    
    printf("FreeRTOS inicializado - Tareas creadas\n");
}
/*===============================================================================================================================*/
/*===========================PROGRAMA PRINCIPAL==================================================================================*/
int main() 
{
    stdio_init_all();
    
    // Inicializar LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 0);
    
    // Inicializar UART
    uart_init_config();
    
    // Inicializar FreeRTOS
    init_freertos();
    
    printf("=== SISTEMA FreeRTOS INICIADO ===\n");
    printf("Esperando comandos por UART...\n");
    
    // Iniciar el scheduler de FreeRTOS (NO RETORNA)
    vTaskStartScheduler();
    
    // Nunca debería llegar aquí
    for (;;) {
        tight_loop_contents();
    }
    
    return 0;
}