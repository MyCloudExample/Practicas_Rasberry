#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Definición del semáforo binario
SemaphoreHandle_t xBinarySemaphore;

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

// Tarea que toamra el semaforo mas de una vez
void vTaskLowPriority(void *pvParameters) 
{
    while (true) 
    {
        // Toma el semáforo para producir
        if(xSemaphoreTake(xBinarySemaphore, pdMS_TO_TICKS(100))) 
        {
            printf("La tarea ha tomado el semaforo binario.\n");
        } 
        else 
        {
            printf("La misma tarea trata de tomar de nuevo el mismo seamforo.\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main() 
{
    stdio_init_all();
    printf("\nIniciando ejemplo de semáforo binario con prioridades\n");
    
    // Crea el semáforo binario (inicialmente disponible)
    xBinarySemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xBinarySemaphore); // Lo liberamos inicialmente
    
    // Verifica creación del semáforo
    if (xBinarySemaphore == NULL) {
        printf("Error al crear el semáforo\n");
        return 1;
    }
    
    // Crea las tareas con diferentes prioridades
    xTaskCreate(task_trash,"Basura",configMINIMAL_STACK_SIZE,NULL,4,NULL);
    xTaskCreate(vTaskLowPriority, "BajaPrioridad", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL);
    // Inicia el planificador
    vTaskStartScheduler();
    
    // Nunca deberíamos llegar aquí
    while (true);
    return 0;
}