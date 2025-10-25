/*Ejemplo 2B: En este ejemplo se utiliza una cola para cargar datos y que luego sea leida por la tarea task_lectora*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/*===============================================DEFINICIONES=======================================================================*/
#define TIME_DELAY_MS 500
/*==================================================================================================================================*/
/*===============================================DEFINICION DE VARIABLES============================================================*/
QueueHandle_t   queue_recolectora;
uint8_t         cantidad=5;
typedef struct
{
    uint8_t     DatoSensor1;
    uint8_t     DatoSensor2;
    uint8_t     Opcion;
}datos_t;
/*==================================================================================================================================*/
/*====================================================DEFINICION DE TAREAS==========================================================*/
void task_lectora(void *params)
{   datos_t recopila;
    uint8_t contador = 0;
    uint8_t sensor;
    
    //vTaskDelay(pdMS_TO_TICKS(5000));
    while(true)
    {
        xQueueReceive(queue_recolectora, &recopila, portMAX_DELAY);
        sensor = recopila.Opcion;
        switch (sensor)
        {
        case 1:
            printf("Dato del sensor 1: %d\n",recopila.DatoSensor1);
            break;
        case 2:
            printf("Dato del sensor 2: %d\n",recopila.DatoSensor2);
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/*==================================================================================================================================*/
void task_sensor1(void *params) 
{
    uint8_t dato_recibido = 0;
    datos_t sensor1;
    
    while(true) 
    {   
        sensor1.DatoSensor1 = 50;
        sensor1.Opcion = 1;
        xQueueSend(queue_recolectora, &sensor1, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(2000));    
    }
}
/*==================================================================================================================================*/
void task_sensor2(void *params) 
{
    uint8_t dato_recibido = 0;
    datos_t sensor2;
    
    while(true) 
    {   
        sensor2.DatoSensor2 = 100;
        sensor2.Opcion = 2;
        xQueueSend(queue_recolectora, &sensor2, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(3000));    
    }
}
/*==================================================================================================================================*/
int main(void) 
{
    stdio_init_all();
    //sleep_ms(5000); // Wait for serial connection
    printf("Iniciando programa...\n");
    
    // Create queue with capacity for 5 uint8_t items
    queue_recolectora = xQueueCreate(cantidad, sizeof(datos_t));
    
    if (queue_recolectora == NULL) {
        printf("Error: No se pudo crear la queue_tE_to_tR\n");
        while(1);
    }
    
    // Create tasks - emitter has higher priority (2) than receiver (1)
    xTaskCreate(task_lectora, "Lectora", 256, NULL, 1, NULL);
    xTaskCreate(task_sensor1, "Sensor1", 256, NULL, 1, NULL);
    xTaskCreate(task_sensor2, "Sensor2", 256, NULL, 1, NULL);
    
    // Start the scheduler
    vTaskStartScheduler();
    
    // Should never reach here
    while(1);
}