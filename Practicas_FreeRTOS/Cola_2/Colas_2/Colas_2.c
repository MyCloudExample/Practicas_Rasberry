#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t miCola;

void tareaReceptora(void *pvParameters) 
{
    int valor;
    while (1) 
    {
        printf("Esperando dato...\n");
        if (xQueueReceive(miCola, &valor, portMAX_DELAY) == pdPASS) 
        {
            printf("Dato recibido: %d\n", valor);
        }
    }
}

void tareaEmisoraRetrasada(void *pvParameters) 
{
    int valor = 42;

    vTaskDelay(pdMS_TO_TICKS(5000));
    printf("Enviando dato: %d\n", valor);
    xQueueSend(miCola, &valor, portMAX_DELAY);
    vTaskDelete(NULL); 
}

int main() 
{
    stdio_init_all();
    sleep_ms(5000);
    sleep_ms(5000);
    printf("Inicio del sistemas \n");

    miCola = xQueueCreate(5, sizeof(int));
    if (miCola == NULL) 
    {
        printf("Error al crear la cola\n");
        while (1);
    }

    xTaskCreate(tareaReceptora, "Receptora", 256, NULL, 1, NULL);
    xTaskCreate(tareaEmisoraRetrasada, "Emisora", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}
