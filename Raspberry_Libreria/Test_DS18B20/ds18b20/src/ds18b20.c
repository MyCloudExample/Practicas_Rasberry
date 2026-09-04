#include <stdlib.h>      // <--- AÑADIR para NULL y malloc
#include <stddef.h>      // <--- AÑADIR para NULL (por si acaso)
#include <string.h>      // <--- AÑADIR para memset, memcpy, etc.
#include <stdio.h>
//=====================================================================================================================================================================================================
// 0- Archivos de cabecera adicionales
//=====================================================================================================================================================================================================
#include "ds18b20.h"
//#include "OneWire.h"
//=====================================================================================================================================================================================================
// 1- Funciones de bajo nivel
//=====================================================================================================================================================================================================
/**
 * @brief                                           Esta funcion permite seleccionar con cual dispositivo se trabajara.
 * @param bus                                       Puntero a la configuracion del bus. ES una varaible global que se declara en el programa principal.
 * @param rom_code                                  Puntero a la ID del dispositivo sleccionado, se supone que las IDs con conocidas.
 */
static bool DS18B20_Select_Device(OneWireBus_t* bus, uint8_t* rom_code)
{
    if(bus == NULL)
    {
        return false;
    }
    // Si hay un solo dispositvo DS18B20
    if(rom_code == NULL)
    {
        OneWire_Command_ROM_SKIP(bus);
        return true;
    }
    // Selecciona un dispositivo especifico
    else
    {
        OneWire_Command_ROM_MATCH(bus, rom_code);
        return true;
    }
}

/**
 * @brief                                           Escribe los datos de configuracion del dispositvo en el scratchpad.
 * @param bus                                       Puntero a la configuracion del bus. ES una varaible global que se declara en el programa principal.
 * @param rom_code                                  Puntero a la ID del dispositivo sleccionado, se supone que las IDs con conocidas.
 * @param th                                        Limite de alarma superior.
 * @param tl                                        Limite de alarma inferior.
 * @param resolution                                Resolucion.
 */
static bool DS18B20_Write_Scrathpad(OneWireBus_t* bus, uint8_t* rom_code, uint8_t th, uint8_t tl, uint8_t resolution)
{
    if(bus == NULL)
    {
        return false;
    }
    // Transaccion de escritura en scratchpad
    if (!OneWire_Reset(bus)) 
    {
        return false;
    }
    // Seleccion del dispositivo
    if (!DS18B20_Select_Device(bus, rom_code)) 
    {
        return false;
    }
    // Se indica al dipsotivos que se va a escribir en el scartchpad
    if(!OneWire_Write_Byte(bus, DS18B20_COMMAND_WRITE))
    {
        return false;
    }
    // Se escriben los datos en el scratchpad
    if(!OneWire_Write_Byte(bus,th))
    {
        return false;
    }
    if(!OneWire_Write_Byte(bus,tl))
    {
        return false;
    }
    if(!OneWire_Write_Byte(bus,resolution))
    {
        return false;
    }

    return true;
} 

//=====================================================================================================================================================================================================
// 2- Funciones de uso para el programador
//=====================================================================================================================================================================================================
bool DS18B20_Init(DS18B20_Config_t* config, OneWireBus_t* bus, uint8_t* rom_code)
{   
    // Verifico que el puntero no sea nulo 
    if(config == NULL || bus == NULL)
    {
        return false;
    }
    // Verifico los rangos de alarma
    int8_t th= config->th;
    int8_t tl= config->tl;
    uint8_t resolution= config->resolution;
    if(th < -55 || th > 125 || tl < -55 || tl >125)
    {
        config->th= 80; // Valor estandar que puede ser modificado por el programador
        config->tl= 10; // Valor estandar que puede ser modificado por el programador
    }
    // Verifico la resolucion, debe ser uno de los valores especificados en la hoja de datos
    if (config->resolution != DS18B20_REG_CONFIGURATION_9 &&
        config->resolution != DS18B20_REG_CONFIGURATION_10 &&
        config->resolution != DS18B20_REG_CONFIGURATION_11 &&
        config->resolution != DS18B20_REG_CONFIGURATION_12) 
    {
        config->resolution= DS18B20_REG_CONFIGURATION_12;
    }
    // Una vez seleccionado el dipositivo escribo en el scratchpad del mismo.
    if(!DS18B20_Write_Scrathpad(bus, rom_code, config->th, config->tl, config->resolution))
    {
        return false;
    }

    return true;
}

float DS18B20_Read_Temperature(OneWireBus_t* bus, uint8_t* rom_code)
{
    uint8_t scratchpad[9];

    if(bus == NULL)
    {
        return -100;
    }
    if (!OneWire_Reset(bus)) 
    {
        return -999.0f;
    }
    if (!DS18B20_Select_Device(bus, rom_code)) 
    {
        return -999.0f;
    }
    if (!OneWire_Write_Byte(bus, DS18B20_COMMAND_CONVERT)) 
    {
        return -999.0f;
    }
    // Se nesecita un tiempo de espera para la conversion, se opto por el mayor tiempo mde conversion para todos los casos
    bus->delay_us(DS18B20_CONV_TIME_12BIT * 1000);
    /* Se inicia una segunda transaccion de informacion*/
    if (!OneWire_Reset(bus)) 
    {
        return -999.0f;
    }
    if (!DS18B20_Select_Device(bus, rom_code)) 
    {
        return -999.0f;
    }
    if (!OneWire_Write_Byte(bus, DS18B20_COMMAND_READ)) 
    {
        return -999.0f;
    }
    // Leer 9 bytes del scratchpad
    /* Usado para testear la medicion de temperatura sin llamar a DS18B20_Read_Memory_Scratchpad
    for (int i = 0; i < 9; i++) 
    {
        scratchpad[i] = OneWire_Read_Byte(bus);
    }*/
    if(!DS18B20_Read_Memory_Scratchpad(bus, scratchpad, rom_code))
    {
        return -999.0f;
    }
    // Se calcula la temperatura
    int16_t raw_temp = (int16_t)((scratchpad[1] << 8) | scratchpad[0]);
    float temperatura = raw_temp * 0.0625f;
    
    return temperatura;
}

bool DS18B20_Read_Memory_Scratchpad(OneWireBus_t* bus, uint8_t* read_memory, uint8_t* rom_code)
{
    // Verifico que los punteros apunten a una direccion valida
    if(bus == NULL || read_memory == NULL)
    {
        return false;
    }
    // Inicio una transaccion de informacion
    if(!OneWire_Reset(bus))
    {
        return false;
    }
    // Selecciono el dispositivo
    if(!DS18B20_Select_Device(bus,rom_code))
    {
        return false;
    }
    // Envio el comando para indicar lectura del scratchpad
    if(!OneWire_Write_Byte(bus, DS18B20_COMMAND_READ))
    {
        return false;
    }
    // Comienza la lectura del scratchpad
    for (uint8_t i = 0; i < 9; i++)
    {
        read_memory[i] = OneWire_Read_Byte(bus);
    }
    return true;
}

bool DS18B20_Save_Config(OneWireBus_t* bus, uint8_t* rom_code)
{
    // 1. Validar parámetros
    if (bus == NULL) 
    {
        return false;
    }

    // 2. Iniciar transacción: Reset + presencia
    if (!OneWire_Reset(bus)) 
    {
        return false;
    }

    // 3. Seleccionar dispositivo (Skip o Match ROM)
    if (!DS18B20_Select_Device(bus, rom_code)) 
    {
        return false;
    }

    // 4. Enviar comando Copy Scratchpad (0x48)
    if (!OneWire_Write_Byte(bus, DS18B20_COMMAND_COPY)) 
    {
        return false;
    }

    // 5. Esperar al menos 10 ms para la escritura en EEPROM
    // IMPORTANTE: En modo parásito, se necesita activar un Strong Pullup durante este tiempo.
    // Por simplicidad, asumimos que el usuario gestiona el Strong Pullup externamente.
    // Si el sensor está en modo parásito, el usuario debe activar el Strong Pullup antes de llamar a esta función.
    bus->delay_us(10000);  // 10 ms

    return true;
}
//=====================================================================================================================================================================================================
// 3- Implementacion de las funciones del dispositivo
//=====================================================================================================================================================================================================
