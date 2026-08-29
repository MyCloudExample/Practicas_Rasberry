#include <OneWire.h>
#include <stdlib.h>      // <--- AÑADIR para NULL y malloc
#include <stddef.h>      // <--- AÑADIR para NULL (por si acaso)
#include <string.h>      // <--- AÑADIR para memset, memcpy, etc.
#include <stdio.h>
//=====================================================================================================================================================================================================
// 0- Estrucutras con la conifguracion de cada modo de velocidad.
//=====================================================================================================================================================================================================
static OneWireMode_t mode_standar= 
{
    .slot_init = ONE_WIRE_STANDAR_SLOT_INIT_US,
    .write_1 = ONE_WIRE_STANDAR_WRITE_1_US,
    .write_0 = ONE_WIRE_STANDAR_WRITE_0_US,
    .write_0_release = ONE_WIRE_STANDAR_WRITE_0_RELEASE,
    .read_wait = ONE_WIRE_STANDAR_READ_WAIT_US,
    .read_sample = ONE_WIRE_STANDAR_READ_SAMPLE_US,
    .reset = ONE_WIRE_STANDAR_RESET_US,
    .presence = ONE_WIRE_STANDAR_PRESENCE_US,
    .presence_timeout = ONE_WIRE_STANDAR_PRESSENCE_TIMEOUT
};

static OneWireMode_t mode_overdrive= 
{
    .slot_init = ONE_WIRE_OVERDRIVE_SLOT_INIT_US,
    .write_1 = ONE_WIRE_OVERDRIVE_WRITE_1_US,
    .write_0 = ONE_WIRE_OVERDRIVE_WRITE_0_US,
    .write_0_release = ONE_WIRE_OVERDRIVE_WRITE_0_RELEASE,
    .read_wait = ONE_WIRE_OVERDRIVE_READ_WAIT_US,
    .read_sample = ONE_WIRE_OVERDRIVE_READ_SAMPLE_US,
    .reset = ONE_WIRE_OVERDRIVE_RESET_US,
    .presence = ONE_WIRE_OVERDRIVE_PRESENCE_US,
    .presence_timeout = ONE_WIRE_OVERDRIVE_PRESSENCE_TIMEOUT
};

//=====================================================================================================================================================================================================
// 1- Funciones de bajo nivel para uso interno de la libreria.
//=====================================================================================================================================================================================================
/**
 * @brief                       Esta funcion permite escribir un 1 logico en el bus.
 * @param bus                   Puntero a la configuracion del bus.
 */
static inline void OneWire_Set_High(OneWireBus_t *bus)
{
    bus->pin_mode_output();     // configura el pin en modo salida
    bus->pin_write_high();     // Coloca un nivel alto en el pin
}

/**
 * @brief                       Esta funcion permite escribir un 0 logico en el bus.
 * @param bus                   Puntero a la configuracion del bus.
 */
static inline void OneWire_Set_Low(OneWireBus_t *bus)
{
    bus->pin_mode_output();     // Configura el pin en modo salida
    bus->pin_write_low();       // Coloca un nivel bajo en el pin
}

/**
 * @brief                       Esta funcion permite el estado de un pin.
 * @param bus                   Puntero a la configuracion del bus.
 */
static inline uint8_t OneWire_Get(OneWireBus_t *bus)
{
    bus->pin_mode_input();      // Configura el pin en modo entrada
    return bus->pin_read();            // Lee el estado del pin
}
//=====================================================================================================================================================================================================
// 2- Implementacion de las funciones basicas.
//=====================================================================================================================================================================================================
bool OneWire_Init(OneWireBus_t *bus)
{
    if(bus == NULL) // Verifico si el puntero apunta al lugar correcto
    {
        return false;
    }
    // Configuracion por defecto en caso de que no se de una configuracion de modo de velocidad
    if(bus->mode_rate != ONE_WIRE_MODE_STANDARD && bus->mode_rate != ONE_WIRE_MODE_OVERDRIVE)
    {
        bus->mode_rate = ONE_WIRE_MODE_STANDARD;
        bus->mode = &mode_standar;
    }
    // Selecciono el modo de velocidad, segun la seleccion se usa una de los dos estructuras definidas con los tiempos
    if(bus->mode_rate == ONE_WIRE_MODE_OVERDRIVE)
    {
        bus->mode = &mode_overdrive;
    }
    else
    {
        bus->mode = &mode_standar;
    }
    // Dejo la linea en alto en caso de que estuviera en bajo
    OneWire_Set_High(bus);
    return true;
}

bool OneWire_Reset(OneWireBus_t *bus) 
{
    if (bus == NULL || bus->mode == NULL) 
    {
        return false;
    }
    const OneWireMode_t *mode = bus->mode;
    
    OneWire_Set_Low(bus);
    bus->delay_us(mode->reset);          // 480 us
    OneWire_Set_High(bus);
    bus->delay_us(mode->presence);       // 60 us
    uint8_t presence = OneWire_Get(bus);
    bus->delay_us(mode->presence_timeout); // 420 us
    return (presence == 0);
}

void OneWire_Write_Bit(OneWireBus_t *bus, uint8_t w_bit) 
{
    if (bus == NULL || bus->mode == NULL) 
    {
        return;
    }
    const OneWireMode_t *mode = bus->mode;
    
    OneWire_Set_Low(bus);
    bus->delay_us(mode->slot_init);      // 5 us
    
    if (w_bit) 
    {
        OneWire_Set_High(bus);
        bus->delay_us(mode->write_1);    // 55 us
    } 
    else 
    {
        bus->delay_us(mode->write_0);    // 55 us
        OneWire_Set_High(bus);
        bus->delay_us(mode->write_0_release); // 5 us
    }
}

uint8_t OneWire_Read_Bit(OneWireBus_t *bus) {
    if (bus == NULL || bus->mode == NULL) {
        return 0;
    }
    const OneWireMode_t *mode = bus->mode;
    uint8_t r_bit = 0;
    
    OneWire_Set_Low(bus);
    bus->delay_us(mode->slot_init);      // 5 us
    OneWire_Set_High(bus);
    bus->delay_us(mode->read_wait);      // 10 us
    r_bit = OneWire_Get(bus);
    bus->delay_us(mode->read_sample);    // 45 us
    
    return r_bit;
}

bool OneWire_Write_Byte(OneWireBus_t *bus, uint8_t data)
{
    // Verifico que los punteros no direccioanr una memoria vacia
    if(bus == NULL || bus->mode == NULL)
    {
        return false;
    }
    // Cuando tengo un dato de 8 bits transmito bit a bit
    for (uint8_t i = 0; i < 8; i++)
    {
        OneWire_Write_Bit(bus, data & 0x01);
        // Desplazo el dato en cada pasada
        data = data >> 1;
    }
    return true;
}

uint8_t OneWire_Read_Byte(OneWireBus_t *bus)
{
    uint8_t r_data=0;

    if(bus == NULL || bus->mode == NULL)
    {
        return 0;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        r_data = r_data >> 1;
        if(OneWire_Read_Bit(bus))
        {
            r_data = r_data | 0x80;
        }
    }
    return r_data;
}
//=====================================================================================================================================================================================================
// 3- Implementacion de las funciones de ROM
//=====================================================================================================================================================================================================
void OneWire_Command_ROM_SKIP(OneWireBus_t *bus)
{
    if(bus == NULL || bus->mode == NULL)
    {
        return;
    }
    // Este comando solos es util cuando hay un solo dispositivo en el bus
    /*
     Verifico nuevamente si hay dispositivo en el bus.
     Esta parte se puede omitir si el programdor verifico el bus con anterioridad.
    if(!OneWire_Reset(bus))
    {
        return;
    }*/

    if (!OneWire_Write_Byte(bus,ONE_WIRE_COMMAND_SKIP))
    {
        return;
    }
}

bool OneWire_Command_ROM_READ(OneWireBus_t *bus, uint8_t *id)
{
    if(bus == NULL || bus->mode == NULL || id == NULL)
    {
        return false;
    }

    /*if(!OneWire_Reset(bus))
    {
        return false;
    }*/

    // --- ESCRIBIR COMANDO READ ROM (0x33) MANUALMENTE ---
    uint8_t cmd = ONE_WIRE_COMMAND_READ;
    for (int i = 0; i < 8; i++)
    {
        OneWire_Write_Bit(bus, (cmd >> i) & 0x01);
    }

    // --- LEER ID ---
    for (uint8_t i = 0; i < 8; i++)
    {
        id[i] = OneWire_Read_Byte(bus);
    }
    return true;
}

void OneWire_Command_ROM_MATCH(OneWireBus_t *bus, const uint8_t *id)
{
    if(bus == NULL || bus->mode == NULL)
    {
        return;
    }

    /*if(!OneWire_Reset(bus))
    {
        return;
    }*/
    // Si la escritura del comando falla retornara
    if (!OneWire_Write_Byte(bus,ONE_WIRE_COMMAND_MATCH))
    {
        return;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        if(!OneWire_Write_Byte(bus, id[i]))
        {
            return;
        }

    }
    return;
}

void OneWire_Command_ROM_SEARCH(OneWireBus_t *bus, uint8_t *id, uint8_t *stack_dis)
{
    /**
     * Esta funcion quedara para desarrollar mas adelante ya que el algoritmo de busqueda es mas complejo.
     */
}