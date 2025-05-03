#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

void tarea1(void *pvParameters) 
{
    while (1) 
    {
        printf("Ejecutando Tarea 1\n");
        vTaskDelay(1000);
        //for (volatile int i = 0; i < 42000000; i++) {}
        //taskYIELD();
    }
}

void tarea2(void *pvParameters) 
{
    while (1) 
    {
        printf("Ejecutando Tarea 2\n");
        vTaskDelay(1000);
        //for (volatile int i = 0; i < 42000000; i++) {}
        //taskYIELD();
    }
}

int main() {
    stdio_init_all(); // Salida por USB UART

    // Mismo tamaño de pila, misma prioridad
    xTaskCreate(tarea1, "Tarea1", 512, NULL, 1, NULL);
    xTaskCreate(tarea2, "Tarea2", 512, NULL, 1, NULL);

    vTaskStartScheduler(); // Inicia el scheduler

    while (true); // Nunca debería llegar acá
}
