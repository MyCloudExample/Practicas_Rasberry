#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t xQueue;

void vTaskEmisora(void *pvParameters) 
{
    int32_t value = 0;
    vTaskDelay(5000);
    while (true) 
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            if (xQueueSend(xQueue, &value, 100) == pdPASS) 
            {
                printf("Enviado: %ld\n", value);
                value++;
            }
        }
        printf("La tarea vTaksEmisora sera elminada dejando la cola completa \n");
        vTaskDelete(NULL);
    }
}


void vTaskReceptora(void *pvParameters) 
{
    int32_t receivedValue;
    vTaskDelay(5000);
    while (true)
    {
        if (xQueueReceive(xQueue, &receivedValue, 100) == pdPASS)
        {
            printf("Procesado: %ld\n", receivedValue);
        }
        else
        {
            printf("La tarea vTaksReceptora se bloqueo por cola vacia \n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

int main() 
{
    stdio_init_all(); 

    xQueue = xQueueCreate(5, sizeof(int32_t)); 
    if (xQueue != NULL) 
    {
        xTaskCreate(vTaskEmisora, "Emisor", 256, NULL, 2, NULL);
        xTaskCreate(vTaskReceptora, "Receptor", 256, NULL, 1, NULL);
        vTaskStartScheduler(); 
    }

    while (true); 
}