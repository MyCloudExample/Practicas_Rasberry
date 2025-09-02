/*Este programa muestra el uso de una cola, se pasa una dato de una tarea a otra*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

QueueSetHandle_t cola;
#define time 500
void task_emisora(void *params)
{ uint8_t contador=0;

    while(true)
    {
        if (xQueueSend(cola,&contador, portMAX_DELAY) == pdPASS)
        {
            printf("Tarea Emisora: %d\n",contador);
            contador++;
        }
    }
    
}

void task_receptora(void *params) 
{ uint8_t contador=0;
    
    while(true) 
    {   
        vTaskDelay(pdMS_TO_TICKS(time));
        xQueueReceive(cola,&contador,portMAX_DELAY);
        printf("Tarea Receptora: %d\n", contador);
        
    }
}


int main(void) 
{
    stdio_init_all();

    cola=xQueueCreate(1,sizeof(uint8_t));
    // Creacion de tareas
    xTaskCreate(task_emisora, "Emisor", 256, NULL, 2, NULL);
    xTaskCreate(task_receptora, "Receptor", 256, NULL, 1, NULL);
    // Arranca el scheduler
    vTaskStartScheduler();
    while(1);
}