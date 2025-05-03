#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t Tails;

void tareaEmisora(void *pvParameters) {
    int contador = 0;

    while (1) 
    {
        if (xQueueSend(Tails, &contador, portMAX_DELAY) == pdPASS) 
        {
            printf("Enviado: %d\n", contador);
            contador++;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  
    }
}

void tareaReceptora(void *pvParameters) 
{
    int recibido;

    while (1)
    {
        if (xQueueReceive(Tails, &recibido, portMAX_DELAY) == pdPASS) 
        {
            printf("Recibido: %d\n", recibido);
        }
    }
}

int main() 
{
    stdio_init_all();  
    Tails = xQueueCreate(10, sizeof(int));

    if (Tails == NULL) 
    {
        printf("Error al crear la cola\n");
        while (1);
    }

    xTaskCreate(tareaEmisora, "Emisora", 256, NULL, 1, NULL);
    xTaskCreate(tareaReceptora, "Receptora", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1); 
}
