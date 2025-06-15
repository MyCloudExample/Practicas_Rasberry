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
        printf("[Tarea %d] Intentando obtener recurso...\n", id);
        
        if(xSemaphoreTake(xSemaphoreContador, pdMS_TO_TICKS(1000))) 
        {
            // Si obtenemos el semáforo (recurso disponible)
            printf("[Tarea %d] Recurso obtenido. Recursos libres: %u\n", 
                  id, uxSemaphoreGetCount(xSemaphoreContador));
            
            // Simulamos uso del recurso
            vTaskDelay(pdMS_TO_TICKS(500 + (id * 200)));  // Cada tarea tiene un tiempo diferente
            
            // Liberamos el recurso
            xSemaphoreGive(xSemaphoreContador);
            printf("[Tarea %d] Recurso liberado. Recursos libres: %u\n\n", 
                  id, uxSemaphoreGetCount(xSemaphoreContador));
        } 
        else 
        {
            printf("[Tarea %d] Tiempo de espera agotado (no hay recursos)\n", id);
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
    for(int i = 1; i <= 5; i++) 
    {
        xTaskCreate(vTaskUsuario, "Usuario", configMINIMAL_STACK_SIZE, (void *)i, 1, NULL);
    }

    vTaskStartScheduler();

    while(true);
    return 0;
}