#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#include <stdbool.h>
//=====================================================================================================================================================================================================
// 0- Librerieas externas
//=====================================================================================================================================================================================================
#include "OneWire.h"
//=====================================================================================================================================================================================================
// 1- Definicion de comandos de funciones
//=====================================================================================================================================================================================================
// 1.1- Comandos del DS18B20
#define DS18B20_COMMAND_CONVERT             0x44 // Inicia una medicion, covnierte el valor y lo escribe en memoria para ser leido
#define DS18B20_COMMAND_WRITE               0x4E // Escribe en los registros TH, TL y CONFIGURATION
#define DS18B20_COMMAND_READ                0xBE // Lee todo el contenido de la memoria scracthpad
#define DS18B20_COMMAND_COPY                0x48 // Los datos de los registros TH, TL y CONFIGURATION se copia en la EEPROM de cada uno
#define DS18B20_COMMAND_RECALL_E2           0xB8 // Los datos almacenados en la EEPROM de TH, TL y CONFIGURATION se carga al scratchpad
#define DS18B20_COMMAND_READ_POWER_SUPPLY   0xB4 // Determina si el dispositivo esta en modo de alimentacion parasita o normal
// 1.2- Valores de configuracion en registros
#define DS18B20_REG_CONFIGURATION_9         0x1F // Configura la resolucion en 9 bits
#define DS18B20_REG_CONFIGURATION_10        0x3F // Configura la resolucion en 10 bits
#define DS18B20_REG_CONFIGURATION_11        0x5F // Configura la resolucion en 11 bits
#define DS18B20_REG_CONFIGURATION_12        0x7F // Configura la resolucion en 12 bits
// 1.3- Datos para el dispositivo DS18B20
#define DS18B20_FAMILY_CODE                 0x28 // Es el codigo de identificacion de familia del dispositivo DS18B20, no es su ID
#define DS18B20_CRC_POLYNOMIAL              0x8C // Usado para el calculo del CRC
#define DS18B20_CONV_TIME_9BIT              94   // Tiempo de conversion en mili segundos
#define DS18B20_CONV_TIME_10BIT             187  // Tiempo de conversion en mili segundos
#define DS18B20_CONV_TIME_11BIT             375  // Tiempo de conversion en mili segundos
#define DS18B20_CONV_TIME_12BIT             750  // Tiempo de conversion en mili segundos
//=====================================================================================================================================================================================================
// 2- Estructura para la incializacion, valores de configuracion
//=====================================================================================================================================================================================================
/**
 * @brief                                   Esta estructura permite inicializar el sensor con una configuracion especifica.
 *                                          Luego el programador decidira se esta configuracion se almaceanra en la ROM.
 */
typedef struct
{
    uint8_t th;                                 // Se configura el valor umbral maximo de la tempreratura para disparar la alarma   
    uint8_t tl;                                 // Se configura el valor umbral minimo de la temperatura para disparar la alarma
    uint8_t resolution;                         // Se configura la resolucion del sensor. Se usan las macros DS18B20_REG_CONFIGURATION_XX
}DS18B20_Config_t;

//=====================================================================================================================================================================================================
// 3- Prototipo de funciones
// NOTA1: Los comandos estaran dentro de cada funcion no es nesesario pasarslo como parametros.
//=====================================================================================================================================================================================================

/**
 * @brief                                   Funcion de inicializacion, aca debe inciarce el bus 1-Wire en primer lugar. Tambien es posible cargar una configracion que el usuario desee.
 * @param config                            Configuracion del sensor, TH, TL y resolucion. El comando para escribir estara dentro de la funcion.
 * @param bus                               Puntero a la configuracion del bus 1-Wire.
 * @param rom_code                          Es el puntero a la varaible que almacena la ID ROM del dipositivo, si solo hay uno se pasa NULL.
 * @return                                  Retorna un true si fue exitosa en caso contrario un false.
 */
bool DS18B20_Init(DS18B20_Config_t* config, OneWireBus_t* bus, uint8_t* rom_code);

/**
 * @brief                                   Inicia la medicion de una temperatura y captura el dato.
 * @param rom_code                          Es el puntero a la varaible que almacena la ID ROM del dipositivo, si solo hay uno se pasa NULL.
 * @param bus                               Puntero a la configuracion del bus 1-Wire.
 * @return                                  Retorna el valor real de la temperatura.                                   
 */
float DS18B20_Read_Temperature(OneWireBus_t* bus, uint8_t* rom_code);

/**
 * @brief                                   Lectura completa del scratchpad, lee los 9 bytes de memoria volatil.
 * @param bus                               Puntero a la configuracion del bus 1-Wire.
 * @param read_memory                       Es un puntero al lugar donde se alamcenara los 9 bytes leidos. El ocmando estara dentro de la funcion.
 * @param rom_code                          Es el puntero a la varaible que almacena la ID ROM del dipositivo, si solo hay uno se pasa NULL.
 * @return                                  Retorna un true si la lectura fue exitosa, en caso contrario retorna un false.
 */
bool DS18B20_Read_Memory_Scratchpad(OneWireBus_t* bus, uint8_t* read_memory, uint8_t* rom_code);

/**
 * @brief                                   Guarda la configuración actual (TH, TL y resolución) en la EEPROM del sensor.
 * @param bus                               Puntero a la configuración del bus 1-Wire.
 * @param rom_code                          Puntero al código ROM del dispositivo (NULL si solo hay uno).
 * @return                                  true si fue exitosa, false en caso contrario.
 */
bool DS18B20_Save_Config(OneWireBus_t* bus, uint8_t* rom_code);

/**
 * @brief                                   Cambia la configuración del sensor en tiempo de ejecución.
 * @param bus                               Puntero a la configuración del bus 1-Wire.
 * @param th                                Nuevo límite superior de alarma (en °C).
 * @param tl                                Nuevo límite inferior de alarma (en °C).
 * @param resolution                        Nueva resolución (usar macros DS18B20_REG_CONFIGURATION_XX).
 * @param rom_code                          Puntero al código ROM del dispositivo (NULL si solo hay uno).
 * @return                                  true si fue exitosa, false en caso contrario.
 */
bool DS18B20_Set_Config(OneWireBus_t* bus, uint8_t th, uint8_t tl, uint8_t resolution, uint8_t* rom_code);
//=====================================================================================================================================================================================================
// 4-
//=====================================================================================================================================================================================================

//=====================================================================================================================================================================================================
// 5-
//=====================================================================================================================================================================================================

#endif // DS18B20_H