#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define MAX_RECURSOS 3  // Número máximo de recursos disponibles

SemaphoreHandle_t xSemaphoreContador;  // Semáforo contador

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

// Tarea que utiliza recursos
void vTaskUsuario(void *pvParameters) 
{
    int id = (int)pvParameters;
    
    while(true) 
    {
        printf("Intentando obtener recurso...\n");
        
        if(xSemaphoreTake(xSemaphoreContador, pdMS_TO_TICKS(1000))) 
        {
            // Si obtenemos el semáforo (recurso disponible)
            printf("Recurso obtenido. Recursos libres: %u\n", 
            uxSemaphoreGetCount(xSemaphoreContador));
        } 
        else 
        {
            printf("Recurso agotado.\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main() 
{
    stdio_init_all();
    printf("\nInicio del sistema - Semáforo contador (Max recursos: %d)\n", MAX_RECURSOS);

    // Creamos el semáforo contador con el máximo de recursos disponibles
    xSemaphoreContador = xSemaphoreCreateCounting(MAX_RECURSOS, MAX_RECURSOS);
    
    if(xSemaphoreContador == NULL)
    {
        printf("Error al crear el semáforo contador\n");
        return 1;
    }

    // Creamos varias tareas que compiten por los recursos
    xTaskCreate(task_trash,"Espera",configMINIMAL_STACK_SIZE,NULL,2,NULL);
    xTaskCreate(vTaskUsuario, "Unica", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

    while(true);
    return 0;
}