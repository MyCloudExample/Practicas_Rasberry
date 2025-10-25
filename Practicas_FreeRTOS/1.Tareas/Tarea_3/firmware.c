#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

/*=============================================Handle de la tarea trabajadora=======================================================*/
TaskHandle_t xTareaTrabajadoraHandle = NULL; //La tarea no sera usspendida dentro de la misma tarea
/*==================================================================================================================================*/
/*=============================================TAREAS===============================================================================*/
void vTareaTrabajadora(void *pvParameters) 
{
    int contador_trabajo = 0;
    
    while(1) 
    {
        contador_trabajo++;
        printf("Trabajadora: Ciclo %d\n", contador_trabajo);
        
        vTaskDelay(pdMS_TO_TICKS(500)); // Trabajar cada 500ms
    }
}
//==================================================================================================================================
void vTareaControladora(void *pvParameters) 
{
    int contador_control = 0;
    
    printf("Iniciando ejemplo vTaskSuspend/vTaskResume\n");
    printf("=========================================\n");
    
    while(1) 
    {
        contador_control++;
        printf("\nControl: Ciclo %d - ", contador_control);
        
        // Cada 3 ciclos, suspender la tarea trabajadora
        if(contador_control % 3 == 0) 
        {
            printf("SUSPENDIENDO trabajadora\n");
            vTaskSuspend(xTareaTrabajadoraHandle);
            
            printf("Trabajadora suspendida 2 segundos...\n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            
            printf("REANUDANDO trabajadora\n");
            vTaskResume(xTareaTrabajadoraHandle);
        } 
        else 
        {
            printf("Trabajadora activa\n");
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Control cada 1 segundo
    }
}
/*==================================================================================================================================*/
int main() 
{
    // Inicializar USB para printf
    stdio_init_all();
    
    // Dar tiempo para conectar la consola
    sleep_ms(2000);
    printf("\n=== FreeRTOS vTaskSuspend/vTaskResume ===\n\n");
    // Crear tarea trabajadora, es la que se suspendera
    xTaskCreate(vTareaTrabajadora,"Trabajadora",256,NULL,1,&xTareaTrabajadoraHandle);
    // Crear tarea controladora
    xTaskCreate(vTareaControladora,"Controladora",256,NULL,2,NULL);
    // Iniciar planificador
    vTaskStartScheduler();
    // Nunca debería llegar aquí
    while(1) {
        printf("Error: Planificador falló\n");
        sleep_ms(1000);
    }
    
    return 0;
}