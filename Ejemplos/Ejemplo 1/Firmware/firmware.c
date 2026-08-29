#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS.h"
#include "hardware/i2c.h"
/*=============================================DEFINICIONES=========================================================================*/
#define I2C_MOD      i2c1
#define SDA_PIN      2
#define SCL_PIN      3
#define BAUD_RATE    400000
/*=============================================DECLARACION DE VARIABLES=============================================================*/
typedef struct 
{
    float temp;
    float pre;
} data_t;
/*==================================================================================================================================*/
/*===============================================PROTOTIPO DE FUNCIONES=============================================================*/
void begin_i2c(void)
{
    i2c_init(I2C_MOD, BAUD_RATE);
    //Inicializo pines para el I2C
    gpio_init(SDA_PIN);
    gpio_init(SCL_PIN);
    //Asigno funciona a los pines
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    //Habilito las ressitencias pull-up
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}
/*=============================================DEFINICION DE TAREA==================================================================*/
void task_bmp280(void *pvParameters) 
{
    
    while(true) 
    {
    }
}

void task_lcd(void *pvParameters)
{

    while(true)
    {

    }
}
int main() 
{
    // Inicializar stdio para printf
    stdio_init_all();    
    
    // Crear múltiples tareas
    xTaskCreate(task_bmp280,"Medicion_Sensor",256,NULL,2,NULL);
    xTaskCreate(task_lcd,"Vizualizacion",256,NULL,1,NULL);
    // Iniciar el scheduler de FreeRTOS
    vTaskStartScheduler();
    // Nunca debería llegar aquí
    while(1) 
    {
        printf("Error: Scheduler no iniciado\n");
        sleep_ms(1000);
    }
    return 0;
}