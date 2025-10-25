#include <stdio.h>                  // Para printf
#include "pico/stdlib.h"            // Para inicializar el hardware (UART/USB)
#include "FreeRTOS.h"               // Core de FreeRTOS
#include "task.h"                   // Funciones de tareas (vTaskDelay, etc.)
#include "semphr.h"                 // Para semáforos y mutex

SemaphoreHandle_t xMutex;

void task_trash(void *pvParameters) 
{
    const uint32_t BUSY_WAIT_TIME_MS = 5000; // 5 segundos
    const uint32_t CPU_FREQ_MHZ = 125; // Frecuencia del RP2040 (125 MHz por defecto)
    const uint32_t CYCLES_PER_MS = CPU_FREQ_MHZ * 1000;
    const uint32_t TOTAL_CYCLES = BUSY_WAIT_TIME_MS * CYCLES_PER_MS;
    
    printf("[BUSY WAIT] Iniciando espera activa de 5 segundos (max prioridad)...\n");
    for(uint32_t i = 0; i < TOTAL_CYCLES; i++) 
    {
        __asm volatile ("nop");
    }
    printf("[BUSY WAIT] Espera completada. Elimando tarea trash\n");
    vTaskDelete(NULL);
}

void vTask1(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            printf("Tarea 1 usando recurso\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Simula uso del recurso
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vTask2(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(xMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            printf("Tarea 2 usando recurso\n");
            vTaskDelay(pdMS_TO_TICKS(300)); // Simula uso del recurso
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main() {
    stdio_init_all(); // Inicializa UART/USB para printf (necesario en Pico)
    xMutex = xSemaphoreCreateMutex();
    xTaskCreate(vTask1, "Task1", 256, NULL, 1, NULL);
    xTaskCreate(vTask2, "Task2", 256, NULL, 2, NULL);
    xTaskCreate(task_trash,"Retardo",256,NULL,3,NULL);
    vTaskStartScheduler();
    while (1); // Nunca debería llegar aquí
}