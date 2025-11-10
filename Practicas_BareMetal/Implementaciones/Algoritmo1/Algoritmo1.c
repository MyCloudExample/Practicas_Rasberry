/*Este codigo toma una cadena de carecteres la cual contiene el nombre de un parametro junto a su valor. La cadena consiste
* de tres parametros Setpoint, SetpointM y Setpointm cada uno con un valor, se pretende tomar la cadena y configurar cada
* parametro de codigo con el valor recibido en la cadena*/
/*===========================ARCHIVOS DE CABECERA================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
/*===============================================================================================================================*/
/*===========================DEFINICIONES========================================================================================*/
#define BUFFER_SIZE 256

typedef struct
{
    uint32_t setpoint;
    float setpoint_min;
    float setpoint_max;
    float altura_medida;
    uint8_t linea;
    uint8_t guardado;
} estructura_setpoint; // Estructura para almacenar los parámetros
/*===============================================================================================================================*/
/*===========================VARIABLES===========================================================================================*/
estructura_setpoint config = {
    .setpoint = 0,
    .setpoint_min = 0.0f,
    .setpoint_max = 0.0f,
    .altura_medida = 0.0f,
    .linea = 0,
    .guardado = 0
};// Variable global de la estructura, variable del tipo estrucutra_setpoint
/*===========================FUNCIONES===========================================================================================*/
//===========================Función para extraer números de una cadena==========================================================
float extract_number(const char *str) 
{
    char temp[32];
    int j = 0;
    int found_digit = 0;
    
    for (int i = 0; str[i] != '\0' && j < 31; i++) 
    {
        if ((str[i] >= '0' && str[i] <= '9') ||  str[i] == '.' || (str[i] == '-' && !found_digit)) 
            {
            temp[j++] = str[i];
            found_digit = 1;
        } 
        else if (found_digit) 
        {
            break;
        }
    }
    temp[j] = '\0';
    
    return atof(temp);
}
//===========================Función para extraer números enteros================================================================
uint32_t extract_integer(const char *str) 
{
    char temp[32];
    int j = 0;
    int found_digit = 0;
    
    for (int i = 0; str[i] != '\0' && j < 31; i++) 
    {
        if (str[i] >= '0' && str[i] <= '9') 
        {
            temp[j++] = str[i];
            found_digit = 1;
        } 
        else if (found_digit) 
        {
            break;
        }
    }
    temp[j] = '\0';
    
    return (uint32_t)atoi(temp);
}
//===========================Función para procesar el comando recibido===========================================================
void process_command(const char *command) 
{
    printf(">>> Comando recibido: %s\n", command);
    
    char *token;
    char working_buffer[BUFFER_SIZE];
    
    // Hacer una copia para no modificar el original
    strcpy(working_buffer, command);
    
    // Tokenizar por comas
    token = strtok(working_buffer, ",");
    
    while (token != NULL) 
    {
        // Eliminar espacios en blanco al inicio y final
        char *clean_token = token;
        while (*clean_token == ' ') clean_token++;
        
        // Buscar los parámetros específicos y asignar a la estructura
        if (strstr(clean_token, "Setpoint:") != NULL) 
        {
            config.setpoint = extract_integer(clean_token);
            printf("✓ Setpoint configurado: %lu\n", config.setpoint);
        }
        else if (strstr(clean_token, "Setpointm:") != NULL) 
        {
            config.setpoint_min = extract_number(clean_token);
            printf("✓ Setpoint_min configurado: %.2f\n", config.setpoint_min);
        }
        else if (strstr(clean_token, "SetpointM:") != NULL) 
        {
            config.setpoint_max = extract_number(clean_token);
            printf("✓ Setpoint_max configurado: %.2f\n", config.setpoint_max);
        }
        else if (strstr(clean_token, "Altura:") != NULL) 
        {
            config.altura_medida = extract_number(clean_token);
            printf("✓ Altura_medida configurada: %.2f\n", config.altura_medida);
        }
        else 
        {
            printf("? Parámetro no reconocido: %s\n", clean_token);
        }
        
        token = strtok(NULL, ",");
    }
    
    printf("----------------------------------------\n");
}
//===========================Función para mostrar el estado actual===============================================================
void mostrar_estado_actual() 
{
    printf("\n=== ESTADO ACTUAL DE CONFIGURACIÓN ===\n");
    printf("Setpoint: %lu\n", config.setpoint);
    printf("Setpoint_min: %.2f\n", config.setpoint_min);
    printf("Setpoint_max: %.2f\n", config.setpoint_max);
    printf("Altura_medida: %.2f\n", config.altura_medida);
    printf("Linea: %u\n", config.linea);
    printf("Guardado: %u\n", config.guardado);
    printf("=======================================\n\n");
}
//===========================Función para mostrar el menú de ayuda===============================================================
void mostrar_ayuda() 
{
    printf("\n*** MODO PRUEBA USB - COMANDOS DISPONIBLES ***\n");
    printf("Formato: Setpoint: valor , SetpointM: valor , Setpointm: valor , Altura: valor\n");
    printf("Ejemplos:\n");
    printf("  Setpoint: 100 , SetpointM: 25.5 , Setpointm: 10.2\n");
    printf("  Setpoint: 50 , Altura: 15.7\n");
    printf("  SetpointM: 30.0 , Setpointm: 5.5 , Setpoint: 75\n");
    printf("Comandos especiales:\n");
    printf("  estado  - Mostrar estado actual\n");
    printf("  ayuda   - Mostrar esta ayuda\n");
    printf("  reset   - Reiniciar todos los valores a cero\n");
    printf("  test    - Ejecutar prueba automática\n");
    printf("***********************************************\n\n");
}
//===========================Función para reiniciar los valores==================================================================
void resetear_valores() 
{
    config.setpoint = 0;
    config.setpoint_min = 0.0f;
    config.setpoint_max = 0.0f;
    config.altura_medida = 0.0f;
    config.linea = 0;
    config.guardado = 0;
    printf("✓ Todos los valores han sido reiniciados a cero\n");
}
//===========================Función para prueba automática======================================================================
void prueba_automatica() 
{
    printf("\n>>> EJECUTANDO PRUEBA AUTOMÁTICA <<<\n");
    
    // Test 1
    printf("\n--- Prueba 1: Comando completo ---\n");
    process_command("Setpoint: 100 , SetpointM: 30.5 , Setpointm: 10.2 , Altura: 25.7");
    
    // Test 2
    printf("\n--- Prueba 2: Orden diferente ---\n");
    process_command("Setpointm: 5.5 , Altura: 15.0 , Setpoint: 75 , SetpointM: 20.0");
    
    // Test 3
    printf("\n--- Prueba 3: Valores negativos ---\n");
    process_command("Setpointm: -5.5 , SetpointM: 25.0 , Altura: -2.5");
    
    // Test 4
    printf("\n--- Prueba 4: Con espacios extra ---\n");
    process_command("  Setpoint:   200  ,  SetpointM:  40.5  ,  Setpointm:  15.2  ");
    
    mostrar_estado_actual();
}
/*===============================================================================================================================*/
/*===========================PROGRAMA PRINCIPAL==================================================================================*/
int main() 
{
    stdio_init_all();
    
    // Esperar a que se conecte el USB
    sleep_ms(2000);
    
    printf("\n");
    printf("========================================\n");
    printf("    PRUEBA USB - PROCESADOR DE COMANDOS\n");
    printf("========================================\n");
    
    mostrar_ayuda();
    mostrar_estado_actual();
    
    char input_buffer[BUFFER_SIZE];
    
    while (true) 
    {
        printf("Ingrese comando > ");
        
        // Leer entrada por USB
        if (fgets(input_buffer, sizeof(input_buffer), stdin)) 
        {
            // Eliminar el salto de línea
            input_buffer[strcspn(input_buffer, "\n")] = 0;
            
            // Procesar comandos especiales
            if (strcmp(input_buffer, "estado") == 0) 
            {
                mostrar_estado_actual();
            }
            else if (strcmp(input_buffer, "ayuda") == 0) 
            {
                mostrar_ayuda();
            }
            else if (strcmp(input_buffer, "reset") == 0) 
            {
                resetear_valores();
            }
            else if (strcmp(input_buffer, "test") == 0) 
            {
                prueba_automatica();
            }
            else if (strlen(input_buffer) > 0) 
            {
                // Procesar comando normal
                process_command(input_buffer);
                mostrar_estado_actual();
            }
        }
        
        sleep_ms(100);
    }
    
    return 0;
}