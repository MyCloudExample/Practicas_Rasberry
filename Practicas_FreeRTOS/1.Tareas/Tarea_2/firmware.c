#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
/*=============================================VARIABLES============================================================================*/
// Contadores para cada tarea
uint32_t contador_tarea1 = 0;
uint32_t contador_tarea2 = 0;
uint32_t contador_tarea3 = 0;
/*==================================================================================================================================*/
/*=============================================DEFINCION DE TAREAS==================================================================*/
// 1. Tarea de BAJA prioridad
void tareaBajaPrioridad(void *pvParameters) 
{
    while(1) 
    {
        contador_tarea1++;
        printf("[BAJA] Contador: %lu\n", contador_tarea1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
// 2. Tarea de MEDIA prioridad (misma que la anterior)
void tareaMediaPrioridad(void *pvParameters) 
{
    while(1) 
    {
        contador_tarea2++;
        printf("[MEDIA] Contador: %lu\n", contador_tarea2);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
// 3. Tarea de ALTA prioridad
void tareaAltaPrioridad(void *pvParameters) 
{
    while(1) 
    {
        contador_tarea3++;
        printf("[ALTA] Contador: %lu - ¡Tengo prioridad!\n", contador_tarea3);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
// 4. Tarea de MÁXIMA prioridad (interrumpe a todas)
void tareaMaximaPrioridad(void *pvParameters) 
{
    while(1) 
    {
        printf("[MÁXIMA] ¡Interrumpiendo! \n");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
/*=================================================================================================================================*/
int main() 
{
    stdio_init_all();
    sleep_ms(2000);
    // Crear tareas con DIFERENTES prioridades
    xTaskCreate(tareaBajaPrioridad,"Baja Prioridad",256,NULL,1,NULL); //Prioridad baja
    xTaskCreate(tareaMediaPrioridad, "Media Prioridad",256,NULL,1,NULL); //Prioridad baja
    xTaskCreate(tareaAltaPrioridad,"Alta Prioridad", 256,NULL,2,NULL); //Prioridad alta
    xTaskCreate(tareaMaximaPrioridad,"Maxima Prioridad",256, NULL,3,  NULL); //Prioridad maxima
    printf("Tareas creadas con prioridades:\n");
    printf("- Baja: 1\n- Media: 1\n- Alta: 2\n- Máxima: 3\n");
    printf("Iniciando scheduler...\n\n");
    vTaskStartScheduler();
    while(1);
    return 0;
}