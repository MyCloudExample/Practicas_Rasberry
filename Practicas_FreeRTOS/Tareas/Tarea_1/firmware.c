#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
/*=============================================DECLARACION DE VARIABLES=============================================================*/
// Estructura para configuración de cada tarea
typedef struct 
{
    const char *nombre;
    uint delay_ms;
    const char *mensaje;
} tarea_config_t;
/*==================================================================================================================================*/
/*=============================================DEFINICION DE TAREA==================================================================*/
//Función única para mostrar mensajes en consola
void tareaMensajes(void *pvParameters) 
{
    tarea_config_t *config = (tarea_config_t*)pvParameters;
    int contador = 0;
    
    printf("Iniciando: %s (%d ms)\n", config->nombre, config->delay_ms);
    
    while(true) 
    {
        printf("[%s] %s - %d\n", config->nombre, config->mensaje, contador);
        contador++;
        vTaskDelay(pdMS_TO_TICKS(config->delay_ms));
    }
}

int main() 
{
    // Inicializar stdio para printf
    stdio_init_all();    
    sleep_ms(2000);
    // Configuraciones para las tareas
    tarea_config_t config_tarea1 = {"TAREA1", 500, "Ejemplo de tarea"};
    tarea_config_t config_tarea2 = {"TAREA2", 700, "UTN-FRA"};
    tarea_config_t config_tarea3 = {"TAREA3", 300, "TD3"};
    // Crear múltiples tareas
    xTaskCreate(tareaMensajes,"Tarea Mensajes 1",256,&config_tarea1,1,NULL);
    xTaskCreate(tareaMensajes,"Tarea Mensajes 2",256,&config_tarea2,1,NULL);
    xTaskCreate(tareaMensajes,"Tarea Mensajes 3",256,&config_tarea3,1,NULL);
    printf("Tareas creadas. Iniciando scheduler...\n");
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