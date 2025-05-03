#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t xQueue;

void vTaskProducer(void *pvParameters) 
{
    int32_t value = 0;
    vTaskDelay(5000);
    while (true) 
    {
        printf("Intentando enviar: %ld\n", value);
        
        if (xQueueSend(xQueue, &value, portMAX_DELAY) == pdPASS) 
        {
            printf("Enviado: %ld\n", value);
            value++;
        }
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}


void vTaskConsumer(void *pvParameters) 
{
    int32_t receivedValue;
    while (true)
    {
        if (xQueueReceive(xQueue, &receivedValue, portMAX_DELAY) == pdPASS)
        {
            printf("Procesado: %ld\n", receivedValue);
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Espera 2 segundos
    }
}

int main() 
{
    stdio_init_all(); 

    xQueue = xQueueCreate(3, sizeof(int32_t)); 
    if (xQueue != NULL) 
    {
        xTaskCreate(vTaskProducer, "Productor", 256, NULL, 1, NULL);
        xTaskCreate(vTaskConsumer, "Consumidor", 256, NULL, 1, NULL);
        vTaskStartScheduler(); 
    }

    while (true); 
}