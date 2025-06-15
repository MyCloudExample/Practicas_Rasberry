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
// Tarea de ALTA prioridad (Consumidor)
void vTaskHighPriority(void *pvParameters) {
    while (true) {
        // Espera el semáforo (máxima prioridad, se ejecutará inmediatamente al recibirlo)
        printf("[ALTA PRIORIDAD] Esperando dato...\n");
        xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
        
        // Procesamiento crítico
        printf("[ALTA PRIORIDAD] Dato recibido, procesando...\n");
        for(int i = 0; i < 3; i++) {
            printf("[ALTA PRIORIDAD] Procesando (%d/3)...\n", i+1);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        
        // Libera el semáforo para permitir más producción
        xSemaphoreGive(xBinarySemaphore);
        printf("[ALTA PRIORIDAD] Procesamiento completado. Semáforo liberado.\n\n");
        
        vTaskDelay(pdMS_TO_TICKS(500)); // Pequeña pausa
    }
}

// Tarea de BAJA prioridad (Productor)
void vTaskLowPriority(void *pvParameters) {
    while (true) {
        // Toma el semáforo para producir (si está disponible)
        if(xSemaphoreTake(xBinarySemaphore, pdMS_TO_TICKS(100))) {
            printf("[baja prioridad] Produciendo dato...\n");
            vTaskDelay(pdMS_TO_TICKS(800)); // Simula producción lenta
            
            printf("[baja prioridad] Dato listo. Liberando semáforo.\n");
            xSemaphoreGive(xBinarySemaphore);
        } else {
            printf("[baja prioridad] Semáforo no disponible, esperando...\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

int main() {
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
    xTaskCreate(vTaskHighPriority, "AltaPrioridad", configMINIMAL_STACK_SIZE*2, NULL, 3, NULL);
    xTaskCreate(vTaskLowPriority, "BajaPrioridad", configMINIMAL_STACK_SIZE*2, NULL, 1, NULL);
    // Inicia el planificador
    vTaskStartScheduler();
    
    // Nunca deberíamos llegar aquí
    while (true);
    return 0;
}