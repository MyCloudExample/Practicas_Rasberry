#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Configuración UART - UART1
#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// Pines 4 y 5 para UART1
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// Buffer para recepción UART
#define RX_BUFFER_SIZE 256
#define COMMAND_BUFFER_SIZE 128

// Estructura para almacenar los parámetros
typedef struct
{
    uint32_t setpoint;
    float setpoint_min;
    float setpoint_max;
    float altura_medida;
    uint8_t linea; // Nos posicionamos en la linea 4 e indicamos la posicion en la linea 4
    uint8_t guardado; // Se utiliza para indicar altura superada
} estructura_setpoint;

// Colas y semáforos
QueueHandle_t xParameterQueue;
SemaphoreHandle_t xParameterMutex;

// Variable global de parámetros
estructura_setpoint current_params = {
    .setpoint = 50,
    .setpoint_min = 0.0,
    .setpoint_max = 100.0,
    .altura_medida = 0.0,
    .linea = 0,
    .guardado = 0
};

// Prototipos de funciones
void uart_task(void *pvParameters);
void process_command_task(void *pvParameters);
void parse_command(char *command);
void update_parameters(uint32_t sp, float sm_min, float sm_max);
void print_parameters(void);
void display_task(void *pvParameters);

int main() {
    stdio_init_all();
    
    // Inicializar UART1
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    
    printf("=== SISTEMA DE CONTROL DE SETPOINT ===\n");
    printf("UART1 configurado en pines TX=%d, RX=%d\n", UART_TX_PIN, UART_RX_PIN);
    printf("Formato de comando: SP:valor1,SM:valor2,Sm:valor3\n");
    printf("Donde:\n");
    printf("  SP:valor1  - Setpoint (entero mayor a 0)\n");
    printf("  SM:valor2  - Setpoint máximo (float mayor a 0)\n");
    printf("  Sm:valor3  - Setpoint mínimo (float mayor a 0)\n");
    printf("Ejemplos válidos:\n");
    printf("  SP:25,SM:100.5,Sm:0.5\n");
    printf("  SP:150,SM:300.0,Sm:10.0\n");
    printf("  SP:75,SM:200,Sm:25\n");
    printf("======================================\n");
    
    // Crear mutex para proteger los parámetros
    xParameterMutex = xSemaphoreCreateMutex();
    
    // Crear cola para comandos (puede almacenar hasta 5 comandos)
    xParameterQueue = xQueueCreate(5, sizeof(estructura_setpoint));
    
    if (xParameterQueue == NULL) {
        printf("Error creando la cola!\n");
        return 1;
    }
    
    // Crear tareas
    xTaskCreate(uart_task, "UART Task", 1024, NULL, 2, NULL);
    xTaskCreate(process_command_task, "Process Command", 1024, NULL, 2, NULL);
    xTaskCreate(display_task, "Display Task", 1024, NULL, 1, NULL);
    
    // Iniciar el scheduler de FreeRTOS
    vTaskStartScheduler();
    
    // Nunca debería llegar aquí
    while(1) {
        tight_loop_contents();
    }
    
    return 0;
}
/*===========================TAREA DE RECEPCION==================================================================================*/
// Tarea para recibir datos UART
void uart_task(void *pvParameters) 
{
    char rx_buffer[RX_BUFFER_SIZE];
    char command_buffer[COMMAND_BUFFER_SIZE];
    int buffer_index = 0;
    uint8_t received_char;
    
    printf("Tarea UART1 iniciada - Esperando comandos...\n");
    
    while(1) 
    {
        // Leer caracteres disponibles de UART1
        while(uart_is_readable(UART_ID)) 
        {
            received_char = uart_getc(UART_ID);
            
            // Echo del carácter recibido (opcional)
            uart_putc(UART_ID, received_char);
            
            // Si es fin de línea o retorno de carro, procesar comando
            if (received_char == '\n' || received_char == '\r') 
            {
                if (buffer_index > 0) 
                {
                    command_buffer[buffer_index] = '\0'; // Terminar string
                    
                    printf("\nComando recibido por UART1: %s\n", command_buffer);
                    
                    // Parsear el comando
                    parse_command(command_buffer);
                    
                    buffer_index = 0; // Resetear buffer
                }
            } 
            // Si el buffer no está lleno, almacenar el carácter
            else if (buffer_index < (COMMAND_BUFFER_SIZE - 1)) 
            {
                command_buffer[buffer_index++] = received_char;
            }
            // Si el buffer está lleno, resetear
            else 
            {
                printf("\nBuffer lleno, resetendo...\n");
                buffer_index = 0;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10)); // Pequeña pausa
    }
}
/*===========================Tarea para procesar comandos y actualizar parámetros================================================*/
void process_command_task(void *pvParameters) 
{
    estructura_setpoint new_params;
    
    printf("Tarea de procesamiento iniciada\n");
    
    while(1) 
    {
        // Esperar nuevos parámetros en la cola
        if (xQueueReceive(xParameterQueue, &new_params, portMAX_DELAY) == pdTRUE) 
        {
            // Actualizar parámetros protegidos por mutex
            if (xSemaphoreTake(xParameterMutex, portMAX_DELAY) == pdTRUE) 
            {
                // Mantener los valores que no se modifican por UART
                new_params.altura_medida = current_params.altura_medida;
                new_params.linea = current_params.linea;
                new_params.guardado = current_params.guardado;
                
                current_params = new_params;
                xSemaphoreGive(xParameterMutex);
                
                printf("Parámetros actualizados via UART1:\n");
                print_parameters();
            }
        }
    }
}
//===========================Función para parsear el comando recibido============================================================
void parse_command(char *command) 
{
    char *token;
    char *rest = command;
    uint32_t sp = 0;
    float sm_max = 0.0, sm_min = 0.0;
    int sp_found = 0, sm_max_found = 0, sm_min_found = 0;
    
    // Dividir por comas
    while ((token = strtok_r(rest, ",", &rest)))
    {
        // Buscar SP: (setpoint) - uint32_t
        if (strstr(token, "SP:")) 
        {
            uint32_t temp_sp;
            if (sscanf(token, "SP:%u", &temp_sp) == 1) 
            {
                sp = temp_sp;
                sp_found = 1;
            }
        }
        // Buscar SM: (setpoint máximo) - float
        else if (strstr(token, "SM:")) 
        {
            float temp_sm_max;
            if (sscanf(token, "SM:%f", &temp_sm_max) == 1) 
            {
                sm_max = temp_sm_max;
                sm_max_found = 1;
            }
        }
        // Buscar Sm: (setpoint mínimo) - float
        else if (strstr(token, "Sm:")) 
        {
            float temp_sm_min;
            if (sscanf(token, "Sm:%f", &temp_sm_min) == 1) 
            {
                sm_min = temp_sm_min;
                sm_min_found = 1;
            }
        }
    }
    
    // Si se encontraron todos los parámetros, actualizar
    if (sp_found && sm_max_found && sm_min_found) 
    {
        update_parameters(sp, sm_min, sm_max);
    } 
    else 
    {
        printf("Error: Formato incorrecto. Use: SP:valor,SM:valor,Sm:valor\n");
        printf("Faltaron: ");
        if (!sp_found) printf("SP ");
        if (!sm_max_found) printf("SM ");
        if (!sm_min_found) printf("Sm ");
        printf("\n");
    }
}

//===========================Función para actualizar parámetros==================================================================
void update_parameters(uint32_t sp, float sm_min, float sm_max) {
    estructura_setpoint new_params;
    
    // VALIDACIONES: Todos los valores deben ser mayores a 0
    if (sp <= 0) 
    {
        printf("Error: SP (%u) debe ser mayor a 0. Valor rechazado.\n", sp);
        return;
    }
    
    if (sm_min <= 0.0) 
    {
        printf("Error: Sm (%.2f) debe ser mayor a 0. Valor rechazado.\n", sm_min);
        return;
    }
    
    if (sm_max <= 0.0) 
    {
        printf("Error: SM (%.2f) debe ser mayor a 0. Valor rechazado.\n", sm_max);
        return;
    }
    
    // Validar que el setpoint esté entre mínimo y máximo
    if (sp < sm_min) 
    {
        printf("Advertencia: SP (%u) menor que Sm (%.2f). Ajustando SP a Sm...\n", sp, sm_min);
        sp = (uint32_t)sm_min;
    }
    
    if (sp > sm_max) 
    {
        printf("Advertencia: SP (%u) mayor que SM (%.2f). Ajustando SP a SM...\n", sp, sm_max);
        sp = (uint32_t)sm_max;
    }
    
    if (sm_min > sm_max) 
    {
        printf("Advertencia: Sm (%.2f) mayor que SM (%.2f). Intercambiando valores...\n", sm_min, sm_max);
        float temp = sm_min;
        sm_min = sm_max;
        sm_max = temp;
    }
    
    // Configurar nuevos parámetros
    new_params.setpoint = sp;
    new_params.setpoint_min = sm_min;
    new_params.setpoint_max = sm_max;
    // altura_medida, linea y guardado se mantienen de los valores actuales
    
    // Enviar a la cola
    if (xQueueSend(xParameterQueue, &new_params, pdMS_TO_TICKS(100)) != pdTRUE) 
    {
        printf("Error: Cola llena, no se pudo actualizar parámetros\n");
    } 
    else 
    {
        printf("Parámetros validados y enviados para actualización\n");
    }
}
// Función para imprimir parámetros actuales
void print_parameters(void) {
    estructura_setpoint params;
    
    // Leer parámetros protegidos por mutex
    if (xSemaphoreTake(xParameterMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        params = current_params;
        xSemaphoreGive(xParameterMutex);
        
        printf("=== PARAMETROS ACTUALES ===\n");
        printf("Setpoint (SP): %u\n", params.setpoint);
        printf("Setpoint Mínimo (Sm): %.2f\n", params.setpoint_min);
        printf("Setpoint Máximo (SM): %.2f\n", params.setpoint_max);
        printf("Altura Medida: %.2f\n", params.altura_medida);
        printf("Línea: %u\n", params.linea);
        printf("Guardado: %s\n", params.guardado ? "SI" : "NO");
        printf("===========================\n");
    }
}

// Tarea para mostrar el estado del sistema periódicamente
void display_task(void *pvParameters) {
    printf("Tarea de display iniciada\n");
    
    while(1) {
        // Mostrar parámetros cada 5 segundos
        print_parameters();
        
        // Simular cambios en altura_medida (para prueba)
        if (xSemaphoreTake(xParameterMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Simular una altura medida (esto sería reemplazado por lectura real de sensor)
            current_params.altura_medida += 0.1f;
            if (current_params.altura_medida > current_params.setpoint_max) {
                current_params.altura_medida = current_params.setpoint_min;
            }
            
            // Verificar si se superó el setpoint
            if (current_params.altura_medida >= current_params.setpoint) {
                current_params.guardado = 1;
            } else {
                current_params.guardado = 0;
            }
            
            xSemaphoreGive(xParameterMutex);
        }
        
        vTaskDelay(pdMS_TO_TICKS(5000)); // Esperar 5 segundos
    }
}