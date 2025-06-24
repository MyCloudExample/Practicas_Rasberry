#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pico/stdlib.h"  // Para stdio USB
#include <stdio.h>
#include <string.h>

// Estructura para mensajes de consola
typedef struct 
{
    char message[100];
    TickType_t delay_ticks;
} ConsoleMessage_t;

QueueHandle_t xConsoleQueue;  // Cola global

// Tarea Guardiana (único acceso a la salida USB)
void vConsoleGatekeeper(void *pvParameters) 
{
    ConsoleMessage_t msg;
    
    while (1) 
    {
        if (xQueueReceive(xConsoleQueue, &msg, portMAX_DELAY) == pdTRUE) 
        {
            printf("%s\n", msg.message);  // Envía por USB
            if (msg.delay_ticks > 0) 
            {
                vTaskDelay(msg.delay_ticks);
            }
        }
    }
}

// Tarea de ejemplo 1
void vTask1(void *pvParameters) 
{
    ConsoleMessage_t msg;
    strcpy(msg.message, "Hola desde Tarea 1 (USB)!");
    msg.delay_ticks = 0;

    while (1) 
    {
        xQueueSend(xConsoleQueue, &msg, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Espera 1 segundo
    }
}

// Tarea de ejemplo 2
void vTask2(void *pvParameters) 
{
    ConsoleMessage_t msg;
    strcpy(msg.message, "Tarea 2 activa (USB)!");
    msg.delay_ticks = pdMS_TO_TICKS(500);
    //msg.delay_ticks = 0;

    while (1) 
    {
        xQueueSend(xConsoleQueue, &msg, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(3000));  // Espera 2 segundos
    }
}

int main() {
    // 1. Inicializa USB para printf (sin UART)
    stdio_init_all();  // Habilita salida por USB
    
    // 2. Crea la cola para 5 mensajes
    xConsoleQueue = xQueueCreate(5, sizeof(ConsoleMessage_t));
    
    // 3. Crea tareas
    xTaskCreate(vConsoleGatekeeper, "Gatekeeper", 256, NULL, 3, NULL);
    xTaskCreate(vTask1, "Task1", 256, NULL, 2, NULL);
    xTaskCreate(vTask2, "Task2", 256, NULL, 2, NULL);
    
    // 4. Inicia el scheduler
    vTaskStartScheduler();

    while (1);  // Nunca debería llegar aquí
}