#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueHandle_t miCola;

void tareaEmisora(void *pvParameters) 
{
    int valor = 0;

    while (1) 
    {
        printf("Intentando enviar: %d\n", valor);
        if (xQueueSend(miCola, &valor, portMAX_DELAY) == pdPASS) 
        {
            printf("Enviado: %d\n", valor);
            valor++;
        }
        vTaskDelay(pdMS_TO_TICKS(100));  // Envía rápido (cada 100 ms)
    }
}

void tareaReceptoraRetrasada(void *pvParameters) 
{
    vTaskDelay(pdMS_TO_TICKS(5000));  // Espera 5 segundos

    int recibido;
    while (1) 
    {
        if (xQueueReceive(miCola, &recibido, portMAX_DELAY) == pdPASS) 
        {
            printf("Recibido: %d\n", recibido);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));  // Recibe lento (cada 1s)
    }
}

int main() 
{
    stdio_init_all();
    sleep_ms(2000);  // Espera al USB para printf

    miCola = xQueueCreate(5, sizeof(int));  // Cola de 5 elementos
    if (miCola == NULL) 
    {
        printf("Error al crear la cola\n");
        while (1);
    }

    xTaskCreate(tareaEmisora, "Emisora", 256, NULL, 1, NULL);
    xTaskCreate(tareaReceptoraRetrasada, "Receptora", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}
