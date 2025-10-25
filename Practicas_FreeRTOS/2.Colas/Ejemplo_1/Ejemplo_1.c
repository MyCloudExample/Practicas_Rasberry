#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/*===============================================DEFINICIONES=======================================================================*/
#define TIME_DELAY_MS 500
/*==================================================================================================================================*/
/*===============================================DEFINICION DE VARIABLES============================================================*/
QueueHandle_t   cola;
TaskHandle_t    TaskEmisora;  //Usado para suspender a la tarea task_emisora
uint8_t         cantidad=5;          
/*==================================================================================================================================*/
/*====================================================DEFINICION DE TAREAS==========================================================*/
void task_emisora(void *params)
{
    uint8_t contador = 0;

    while(true)
    {
        if (xQueueSend(cola, &contador, portMAX_DELAY) == pdPASS)
        {
            printf("Tarea Emisora envió: %d\n", contador);
            contador++;
        }
    }
}
/*==================================================================================================================================*/
void task_receptora(void *params) 
{
    uint8_t dato_recibido = 0;
    UBaseType_t elementos_en_cola = 0;
    
    while(true) 
    {   
        vTaskSuspend(TaskEmisora);
        printf("\n--- Receptor activado: Leyendo cola ---\n");
        elementos_en_cola = uxQueueMessagesWaiting(cola);
        printf("Elementos en cola: %d\n", elementos_en_cola);
        
        while(xQueueReceive(cola, &dato_recibido, 0) == pdPASS)
        {
            printf("Tarea Receptora leyó: %d\n", dato_recibido);
        }
        
        printf("--- Cola vacía - Reactivando emisor ---\n\n");
        
        vTaskResume(TaskEmisora);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
/*==================================================================================================================================*/
int main(void) 
{
    stdio_init_all();
    sleep_ms(5000); // Wait for serial connection
    printf("Iniciando programa...\n");
    
    // Create queue with capacity for 5 uint8_t items
    cola = xQueueCreate(cantidad, sizeof(uint8_t));
    
    if (cola == NULL) {
        printf("Error: No se pudo crear la cola\n");
        while(1);
    }
    
    // Create tasks - emitter has higher priority (2) than receiver (1)
    xTaskCreate(task_emisora, "Emisor", 256, NULL, 2, &TaskEmisora);
    xTaskCreate(task_receptora, "Receptor", 256, NULL, 1, NULL);
    
    // Start the scheduler
    vTaskStartScheduler();
    
    // Should never reach here
    while(1);
}