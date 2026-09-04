#ifndef ONEWIRE_H
#define ONEWIREH

#include <stdint.h>
#include <stdbool.h>
//=====================================================================================================================================================================================================
// Tiempos del protocolo tomados de las notas de aplicacion de Analog Devices
// Pagina Web: https://www.analog.com/en/resources/technical-articles/1wire-communication-through-software.html
// MODO STANDAR
// - Duracion de SLOT: 70 us
//  - Escritura:    6 us el maestro mantiene la linea abajo
//                  64 us se mantiene en alto la linea para escribir un 1 logico
//                  70 us se mantiene en bajo la linea para escribir un 0 logico
//  - Lectura:      6 us el maestro mantiene la linea abajo
//                  Luego de 15 us el maestro lee la linea, en ese tiempo el esclavo escribio un 1 logico si la dejo en alto
//                  Luego de 15 us el maestro lee la linea, en ese tiempo el esclavo escribio un 0 logico si la tiro abajo
// MODO OVERDRIVE
// - Duracion de SLOT: 8.5 us (podria usarse 9 us)
//  - Escritura:    1 us el maestro mantiene la linea abajo
//                  7.5 us se mantiene en alto la linea para escribir un 1 logico
//                  8.5 us se mantiene en bajo la linea para escribir un 0 logico
//  - Lectura:      1 us el maestro mantiene la linea abajo
//                  Luego de 2 us el maestro lee la linea, en ese tiempo el esclavo escribio un 1 logico si la dejo en alto
//                  Luego de 2 us el maestro lee la linea, en ese tiempo el esclavo escribio un 0 logico si la tiro abajo
//=====================================================================================================================================================================================================
//=====================================================================================================================================================================================================
// 1- Comandos de ROM, sirven para todos los dispositivo qu usen 1-Wire
//=====================================================================================================================================================================================================
#define ONE_WIRE_COMMAND_READ          0x33                 //Comando para inciar la lectura de un dispositivo
#define ONE_WIRE_COMMAND_MATCH         0x55                 //Comando para direccionar un dispositivo especifico luego de enviar el comando es nesesario enviar la direccion del dispositivo
#define ONE_WIRE_COMMAND_SEARCH        0xF0                 //Comando para leer las ID de todos los dispositivos del bus
#define ONE_WIRE_COMMAND_SKIP          0xCC                 //Comando para leer un dispositivo sin utilizar su ID, solo es util cunado hay un solo dispositivo en el bus
//=====================================================================================================================================================================================================
// 2- Tiempos estandarizados para el protocolo provistos por Analog Devices de sus notas de aplicacion
// Para la escritura de 1 logico se utilizan los tiempos A, B
// Para la escritura de 0 logico se utilizan los tiempos A, C, D
// Para la lectura del 1 logico se utilizan los tiempos A, E, F
// Para la lectura del 0 logico se utilizan los tiempos A, E, F
//=====================================================================================================================================================================================================
// 2.1- Velocidad estandar
#define ONE_WIRE_STANDAR_SLOT_INIT_US                 5       // A: Duracion de tirada abajo de la linea, usado para dar inicio al slot    
#define ONE_WIRE_STANDAR_WRITE_1_US                   55      // B: Escritura de 1 logico luego trasncurrido el pulso de inicio de slot se mantiene en alto lo que resta del slot
#define ONE_WIRE_STANDAR_WRITE_0_US                   55      // C: Escritura de 0 logico luego del pulso de inicio de slot se mantiene abajo lo que resta del slot
#define ONE_WIRE_STANDAR_WRITE_0_RELEASE              5      // D: Libera la linea de escritura de 0 logico antes de terminar el slot
#define ONE_WIRE_STANDAR_READ_WAIT_US                 10       // E: Cuando el maestro libera el bus hay un retardo de 9 us, en los cuales el maestro lee el bus
#define ONE_WIRE_STANDAR_READ_SAMPLE_US               45      // F: El esclavo mantiene la linea en bajo o alto segun corresponda y 10 us antes de finalizar suelta la linea para que retorne
#define ONE_WIRE_STANDAR_G                            0       // G: Este tiempo se usa para el pulso de RESET
#define ONE_WIRE_STANDAR_RESET_US                     480     // H: Tiempo que el maestro mantiene la linea abajo
#define ONE_WIRE_STANDAR_PRESENCE_US                  60      // I: En este tiempo el bus se libera y el esclavo responde, pero es donde el maestro lee el estado
#define ONE_WIRE_STANDAR_PRESSENCE_TIMEOUT            420     // J: Sin dispositivos presentes, el maestro sigue leyendo la linea hasta encontrar un dipositivo
// 2.2- Velocidad de sobre marcha
#define ONE_WIRE_OVERDRIVE_SLOT_INIT_US               1       // A: Duracion de tirada abajo de la linea, usado para dar inicio al slot    
#define ONE_WIRE_OVERDRIVE_WRITE_1_US                 7.5     // B: Escritura de 1 logico luego del pulso de inicio de slot se mantiene en alto lo que resta del slot
#define ONE_WIRE_OVERDRIVE_WRITE_0_US                 7.5     // C: Escritura de 0 logico luego del pulso de inicio de slot se mantiene abajo lo que resta del slot
#define ONE_WIRE_OVERDRIVE_WRITE_0_RELEASE            1       // D: Libera la linea de escritura de 0 logico antes de terminar el slot
#define ONE_WIRE_OVERDRIVE_READ_WAIT_US               1       // E: Cuando el maestro libera el bus hay un retardo de 9 us, en los cuales el maestro lee el bus
#define ONE_WIRE_OVERDRIVE_READ_SAMPLE_US             7       // F: El esclavo mantiene la linea en bajo o alto segun corresponda y 10 us antes de finalizar suelta la linea para que retorne
#define ONE_WIRE_OVERDRIVE_G                          0       // G: Este tiempo se usa para el pulso de RESET
#define ONE_WIRE_OVERDRIVE_RESET_US                   70      // H: Tiempo que el maestro mantiene la linea abajo
#define ONE_WIRE_OVERDRIVE_PRESENCE_US                8.5     // I: En este tiempo el bus se libera y el esclavo responde, pero es donde el maestro lee el estado
#define ONE_WIRE_OVERDRIVE_PRESSENCE_TIMEOUT          40      // J: Sin dispositivos presentes, el maestro sigue leyendo la linea hasta encontrar un dipositivo
// 2.3- Seleccion de velocidad
#define ONE_WIRE_MODE_STANDARD                        0       //Velocidad estandar de 15kbps
#define ONE_WIRE_MODE_OVERDRIVE                       1       //velocidad elevada de 125kbps
//=====================================================================================================================================================================================================
// 3- EStructura de datos para uso interno de la libreria
//=====================================================================================================================================================================================================
/**
 * @brief           Esta estructura contiene la configuracion del modo de velocidad. Donde cada campo contendra los tiempos definidos para 
 *                  cada modo. De esta manera la estructura se usara en todo el codigo sleccionadno solo el tiempo que se nesecite utilizar.
 */
typedef struct
{
    uint8_t slot_init;                      // Inicio del slot
    uint8_t write_1;                        // Escritura de 1 logico 
    uint8_t write_0;                        // Escritura de 0 logico
    uint8_t write_0_release;                // Escritura de 0 logico, este tiempo levanta la linea antes de terminar el slot
    uint8_t read_wait;                      // El maestro espera un tiempo antes de leer el bus
    uint8_t read_sample;                    // El maestro lee el bus
    uint16_t reset;                         // Pulso de RESET
    uint16_t presence;                      // Respuesta del esclavo al pulso de RESET
    uint16_t presence_timeout;              // Tiempo para que el maetro lee el bus
}OneWireMode_t;

/**
 * @brief           Esta estructura almacenara punteros a funciones de bajo nivel. Estas funciones dependeran del hardware
 *                  de la plataforma utilizada. Estas funciones se definiran en el codigo principal y luego sus nombres seran pasados
 *                  a los punteros de la estructura. Es posible realizar esto dentro de la libreria, en ese caso sera nesesario implementar
 *                  las funciones de la plataforma dentro de la libreria.
 */
typedef struct
{
    void (*pin_mode_output)(void);          //Puntero a funcion para configurar el pin como salida. El maestro escribe en la linea
    void (*pin_mode_input)(void);           //Puntero a funcion para confiugrar el pin como entrada. El maestro lee de la linea
    void (*pin_write_high)(void);          //Puntero a funcion para escribir un 1 en la linea.
    void (*pin_write_low)(void);            //Puntero a funcion para escribir un 0 en la linea
    uint8_t (*pin_read)(void);              //Puntero a funcion para leer el estado logico de un pin
    void (*delay_us)(uint32_t time_us);     //Puntero a funcion para generar retardos en microsegundos
    uint8_t mode_rate;                      //Por defecto es 0 para standar y 1 para override  
    OneWireMode_t *mode;                    //Puntero a losmtiempos segun el modo 
}OneWireBus_t;

//=====================================================================================================================================================================================================
// 4- Prototipo de funciones
//=====================================================================================================================================================================================================
/**
 * @brief                   Esta funcion inicializa el protocolo. Recibe las callback del codigo princiapl para usarlas en la libreria. 
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @return                  Si es False se produjo en error, pero si es True la carga fue exsitosa. 
 */
bool OneWire_Init(OneWireBus_t *bus);

/**
 * @brief                   Envia el pulso de RESET.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @return                  Retornara un estado logico que indicara si hay elementos en el bus.
 */
bool OneWire_Reset(OneWireBus_t *bus);

/**
 * @brief                   Escribe un bit en el bus.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @param w_bit             Estado del bit a escribir.
 */
void OneWire_Write_Bit(OneWireBus_t *bus, uint8_t w_bit);

/**
 * @brief                   Lee un bit de bus.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @return                  Retorna el bit leido.
 */
 uint8_t OneWire_Read_Bit(OneWireBus_t *bus);

/**
 * @brief                   Escribe un byte en el bus.
 * @param data              Byte de dato a escribir.
 * @return                  Si la escritura del byte fue exitosa retorna True, en caso contrario False.
 */
bool OneWire_Write_Byte(OneWireBus_t *bus, uint8_t data);

/**
 * @brief                   Lee un byte del bus.
 * @return                  Retorna el byte leido.
 */
uint8_t OneWire_Read_Byte(OneWireBus_t *bus);
//=====================================================================================================================================================================================================
// 5- Prototipo de funciones para los comandos de ROM
//      NOTA1: Los punteros que se han definido del tipo uint8_t para esta seccion, se refieren a arrays de 8 bytes.
//      Recordar que la direccion de un dispositivo 1-Wire es de 64 bits.
//      NOTA2: Los comandos de ROM solo permiten buscar y seleccionar un dispositivo, no interactuan con el dispositivo seleccionado.
//=====================================================================================================================================================================================================
/**
 * @brief                   Lectura directa de un dispositivo, debe ser el unico en le bus para utilizar esta funcion.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 */
void OneWire_Command_ROM_SKIP(OneWireBus_t *bus);

/**
 * @brief                   Funcion para direccionar un dispósitivo especifico.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @param id                Identificacion de dispositivo, es un puntero a un array donde se almaceanran los 64 bits.
 */
void OneWire_Command_ROM_MATCH(OneWireBus_t *bus, const uint8_t *id);

/**
 * @brief                   Funcion para leer el id de un dipositivo.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @param id                Identificacion de dispositivo, es un puntero a un array donde se almaceanran los 64 bits.
 * @return                  Si falla retorna False, en caso contrario retorna True.
 */
bool OneWire_Command_ROM_READ(OneWireBus_t *bus, uint8_t *id);

/**
 * @brief                   Lectura directa de un dispositivo, debe ser el unico en le bus para suar esta funcion.
 * @param bus               Puntero a la estructura de funciones de bajo nivel.
 * @param id                Identificacion de dispositivo, es un puntero a un array donde se almaceanran los 64 bits.
 * @param stack_dis         Puntero hacia la pila para alamcenar la discrepancia de la interacion anterior.
 */
void OneWire_Command_ROM_SEARCH(OneWireBus_t *bus, uint8_t *id, uint8_t *stack_dis);
#endif // ONEWIRE_BASE_H