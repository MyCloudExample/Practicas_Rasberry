/**
 * @file            bmp_280.c
 * @brief           ImplementaciOn de la librería para el sensor BMP280.
 * @version         1.0
 * @date            2023-10-27
 * 
 * Este archivo contiene la implementaciOn de las funciones de la librería.
 * Depende de las funciones de bajo nivel (callbacks) que el usuario debe implementar.
 */

 #include "bmp_280.h"
 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/i2c.h"

//*************************************************************************************** */
//========================================================================================
// Estructura de datos para la iniciliazacion del hadrware con comunicacion I2C
// Tambien se definen dos funciones para suar del el programa principal llamadas:
// BMP280_SetI2CCOnfig y get_i2_address.
//========================================================================================
 /**
 * @brief Estructura que contiene la configuración I2C.
 *        El usuario debe inicializar esta estructura en el main.
 */
typedef struct 
{
    i2c_inst_t* port;      /**< Puerto I2C (i2c0 o i2c1) */
    uint8_t sda_pin;       /**< Pin SDA */
    uint8_t scl_pin;       /**< Pin SCL */
    uint32_t baudrate;     /**< Velocidad del bus (Hz) */
} I2C_Config_t;

// Variable global para almacenar la configuración I2C
static I2C_Config_t g_i2c_config;

/**
 * @brief Establece la configuración I2C para la librería.
 * @param port Puerto I2C (i2c0 o i2c1).
 * @param sda_pin Pin SDA.
 * @param scl_pin Pin SCL.
 * @param baudrate Velocidad del bus (Hz).
 * 
 * @note Esta función debe llamarse antes de cualquier operación con el sensor.
 *       La inicialización del hardware I2C debe realizarse en el main.
 */
void BMP280_SetI2CConfig(i2c_inst_t* port, uint8_t sda_pin, uint8_t scl_pin, uint32_t baudrate)
{
    g_i2c_config.port = port;
    g_i2c_config.sda_pin = sda_pin;
    g_i2c_config.scl_pin = scl_pin;
    g_i2c_config.baudrate = baudrate;
}

/**
 * @brief Obtiene la dirección I2C del sensor.
 * @param config Puntero a la estructura de configuración del BMP280.
 * @return Dirección I2C del sensor (0x76 o 0x77).
 */
static uint8_t get_i2c_address(BMP280_Config_t* config)
{
    // Si el usuario ha configurado una dirección I2C, la usamos.
    // Si no, usamos la dirección primaria (0x76) por defecto.
    if (config != NULL && config->i2c_addr != 0) 
    {
        return config->i2c_addr;
    }
    return BMP280_I2C_ADDR_PRIM;  // 0x76 por defecto
}

//*************************************************************************************** */

 // =======================================================================================
// 1- VARIABLES GLOBALES INTERNAS (ESTÁTICAS)
// =======================================================================================
/**
 * @brief           Se suara para almacenar la configuracion actual del sensor.
 *                  Se usara de forma interna solo para recordar la configuracion entre llamadas.
 */
static BMP280_Config_t Current_config;

/**
 * @brief           Variable global la cual almacena el parametro t_fine para calcular la presion.,
 *                  
 */
static int32_t t_fine=0;

// =======================================================================================
// 2- FUNCIONES AUXILIARES (INTERNAS):  Estas funciones se implemnetan en el archivo .c para fragmentar el codigo
//                                      y sea mas facil de depurar.  
// =======================================================================================
/**
 * @brief           Combina los 3 bytes del dato en formato RAW en un solo dato de 20 bits.
 * @param msb       Byte mas significativo.
 * @param lsb       Byte intermedio.       
 * @param xlsb      Byte menos significativo.
 * @return          Retorna el dato de 20 bits.    
 */
static int32_t combine_20bit(uint8_t msb, uint8_t lsb, uint8_t xlsb)
{
    return (int32_t)((msb << 12) | (lsb << 4) | (xlsb >> 4));
}

/**
 * @brief           Si la plataforma utilizada no cuenta con una funcion delay se puede usar esta funcion.
 * @param time_ms   Tiempo en mili segundos.
 * @note            Esta funcion es una implmentacion generica, debe usarse segun los recursos de cada plataforma.
 *                  Si la pltaforma cuenta con una delay en mili segundos puede usarse este detro de la funcion.
 */
static void delay_ms(uint32_t time_ms)
{
    /**
     * Uso del delay en mili segundo de la paltaforma seleccionada.
     * Si el dealy lo aporta la plataforma ignoarar el codigo del bulce for.
     */
   sleep_ms(time_ms);
}

// =======================================================================================
// 3- IMPLEMENTACION DE FUNCIONES PUBLICAS: Son las funciones que usara el programador.
//                                          Las funciones no seran descriptas ya que esta tarea se realizo en el bmp_280.h
//                                          Solo se implmentara la logica de programacion.
// =======================================================================================
/**
 * Esta funcion es vital ya que nos permitira saber si la carga de los datos de calibracion tuvo exito.
 * Tambien indicara si la lectura y procesamiento de los datos de calibracion fue correcta.
 * Se verifica la direccion del sensor, es decir si el sensor esta conectado a la linea.
 * Se verifica la carga correcta de la configuracion.
 */
bool BMP280_Init(BMP280_Config_t* config, BMP280_CalibData_t* calib)
{
    //Validacion de parametros
    if(config == NULL || calib == NULL)
    {
        return false;
    }
    // 1- Resguardo la configuracion para uso interno
    Current_config = *config; 
    // 2- Reinicion del sensor
    if(!BMP280_Reset())
    {
        return false;
    }
    // 3- Se espera que se termine la carga de datos de calibracion dede NVM a los registros del sensor
    uint8_t status=0;
    uint32_t timeout=100;
    while(timeout > 0)
    {
        if(!BMP280_ReadRegister(BMP_280_STATUS, &status))
        {
            return false;
        }
        if((status & BMP_280_STATUS_IM_UPDATE) == 0)
        {
            break;
        }
        delay_ms(1);
        timeout--;
    }
    if(timeout == 0)
    {
        return false; //La calibracion no esta lista
    }
    // 4- Los datos de calibracion fueron cargados a los registros, ahora se pueden leer y procesar
    if(!BMP280_ReadCalibration(calib))
    {
        return false;
    }
    // 5- Verifico el ID del chip
    if(!BMP280_CheckChipID())
    {
        return false;
    }
    // 6- Si los datos de calibracion fueron cargados a los registros, leidos y procesados se procede a cargar la configuracion
    if(!BMP280_SetConfig(config))
    {
        return false;
    }
    return true;
}
//========================================================================================
bool BMP280_ReadCalibration(BMP280_CalibData_t* calib)
{
    if (calib == NULL) 
    {
        return false;
    }

    uint8_t buffer[BMP_280_CALIBRATION_SIZE];

    // Leer los 24 bytes de calibraciOn en ráfaga
    if (!BMP280_BurstReadRegister(BMP_280_CALIBRATION_START, buffer, BMP_280_CALIBRATION_SIZE)) 
    {
        return false;
    }

    // Combinar los bytes en valores de 16 bits (LSB primero, MSB después)
    calib->dig_T1 = (uint16_t)((buffer[1] << 8) | buffer[0]);   // 0x89 (MSB) y 0x88 (LSB)
    calib->dig_T2 = (int16_t)((buffer[3] << 8) | buffer[2]);    // 0x8B (MSB) y 0x8A (LSB)
    calib->dig_T3 = (int16_t)((buffer[5] << 8) | buffer[4]);    // 0x8D (MSB) y 0x8C (LSB)
    calib->dig_P1 = (uint16_t)((buffer[7] << 8) | buffer[6]);   // 0x8F (MSB) y 0x8E (LSB)
    calib->dig_P2 = (int16_t)((buffer[9] << 8) | buffer[8]);    // 0x91 (MSB) y 0x90 (LSB)
    calib->dig_P3 = (int16_t)((buffer[11] << 8) | buffer[10]);  // 0x93 (MSB) y 0x92 (LSB)
    calib->dig_P4 = (int16_t)((buffer[13] << 8) | buffer[12]);  // 0x95 (MSB) y 0x94 (LSB)
    calib->dig_P5 = (int16_t)((buffer[15] << 8) | buffer[14]);  // 0x97 (MSB) y 0x96 (LSB)
    calib->dig_P6 = (int16_t)((buffer[17] << 8) | buffer[16]);  // 0x99 (MSB) y 0x98 (LSB)
    calib->dig_P7 = (int16_t)((buffer[19] << 8) | buffer[18]);  // 0x9B (MSB) y 0x9A (LSB)
    calib->dig_P8 = (int16_t)((buffer[21] << 8) | buffer[20]);  // 0x9D (MSB) y 0x9C (LSB)
    calib->dig_P9 = (int16_t)((buffer[23] << 8) | buffer[22]);  // 0x9F (MSB) y 0x9E (LSB)

    return true;
}
//========================================================================================

bool BMP280_Reset(void) 
{
    // Escribir el comando de reset en el registro correspondiente
    return BMP280_WriteRegister(BMP_280_RESET, BMP_280_RESET_CMD);
}
//========================================================================================

bool BMP280_CheckChipID(void) 
{
    uint8_t chip_id = 0;

    if (!BMP280_ReadRegister(BMP_280_ID, &chip_id)) 
    {
        return false;
    }

    return (chip_id == BMP_280_ID_CHIP);
}
//========================================================================================

bool BMP280_SetConfig(BMP280_Config_t* config) 
{
    if (config == NULL) 
    {
        return false;
    }

    // Construir el byte para el registro CONFIG (0xF5)
    uint8_t config_byte = 0;
    config_byte |= (config->standby & 0xE0);      // Bits 7-5: t_sb
    config_byte |= (config->filter & 0x1C);       // Bits 4-2: filter
    if (config->spi3w_en) 
    {
        config_byte |= BMP_280_CONFIG_SPI3W_EN_3W;     // Bit 0: spi3w_en
    } 
    else 
    {
        config_byte |= BMP_280_CONFIG_SPI3W_EN_4W;
    }

    // Escribir el registro CONFIG
    if (!BMP280_WriteRegister(BMP_280_CONFIG, config_byte)) 
    {
        return false;
    }

    // Construir el byte para el registro CTRL_MEAS (0xF4)
    uint8_t ctrl_meas_byte = 0;
    ctrl_meas_byte |= (config->osrs_t & 0xE0);    // Bits 7-5: osrs_t
    ctrl_meas_byte |= (config->osrs_p & 0x1C);    // Bits 4-2: osrs_p
    ctrl_meas_byte |= (config->mode & 0x03);      // Bits 1-0: mode

    // Escribir el registro CTRL_MEAS
    if (!BMP280_WriteRegister(BMP_280_CTRL_MEAS, ctrl_meas_byte)) 
    {
        return false;
    }

    return true;
}
//========================================================================================

bool BMP280_StartForcedMeasurement(void) 
{
    // En modo forzado, escribir el mismo valor de CTRL_MEAS inicia una mediciOn
    // Reutilizamos la configuraciOn almacenada en g_config
    uint8_t ctrl_meas_byte = 0;
    ctrl_meas_byte |= (Current_config.osrs_t & 0xE0);
    ctrl_meas_byte |= (Current_config.osrs_p & 0x1C);
    ctrl_meas_byte |= BMP_280_CTRL_MEAS_MODE_FORCED;  // Forzar modo forzado

    return BMP280_WriteRegister(BMP_280_CTRL_MEAS, ctrl_meas_byte);
}
//========================================================================================

bool BMP280_WaitForMeasurement(uint32_t timeout_ms) 
{
    uint8_t status = 0;

    while (timeout_ms > 0) 
    {
        if (!BMP280_ReadRegister(BMP_280_STATUS, &status)) 
        {
            return false;
        }
        if ((status & BMP_280_STATUS_MEASURING) == 0) 
        {
            return true;  // MediciOn terminada
        }
        delay_ms(1);
        timeout_ms--;
    }

    return false;  // Timeout
}
//========================================================================================

bool BMP280_ReadRawData(int32_t* adc_T, int32_t* adc_P) 
{
    if (adc_T == NULL || adc_P == NULL) 
    {
        return false;
    }

    uint8_t buffer[BMP_280_RAW_DATA_SIZE];

    // Leer los 6 bytes de datos RAW en ráfaga desde 0xF7 hasta 0xFC
    if (!BMP280_BurstReadRegister(BMP_280_PRESS_MSB, buffer, BMP_280_RAW_DATA_SIZE)) 
    {
        return false;
    }

    // Combinar los bytes en valores de 20 bits
    // PresiOn: buffer[0] = press_msb, buffer[1] = press_lsb, buffer[2] = press_xlsb
    *adc_P = combine_20bit(buffer[0], buffer[1], buffer[2]);

    // Temperatura: buffer[3] = temp_msb, buffer[4] = temp_lsb, buffer[5] = temp_xlsb
    *adc_T = combine_20bit(buffer[3], buffer[4], buffer[5]);

    return true;
}
//========================================================================================

int32_t BMP280_CompensateTemperature(int32_t adc_T, BMP280_CalibData_t* calib, int32_t* t_fine) 
{
    if (calib == NULL || t_fine == NULL) 
    {
        return 0;
    }

    int32_t var1, var2, T;

    // FOrmula de compensaciOn de temperatura (versiOn 32 bits)
    var1 = ((((adc_T >> 3) - ((int32_t)calib->dig_T1 << 1))) *
            ((int32_t)calib->dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)calib->dig_T1)) *
              ((adc_T >> 4) - ((int32_t)calib->dig_T1))) >> 12) *
            ((int32_t)calib->dig_T3)) >> 14;

    *t_fine = var1 + var2;  // Guardar t_fine para la presiOn

    T = (*t_fine * 5 + 128) >> 8;  // Temperatura en °C * 100

    return T;
}
// =======================================================================================

uint32_t BMP280_CompensatePressure(int32_t adc_P, BMP280_CalibData_t* calib, int32_t t_fine) 
{
    if (calib == NULL) 
    {
        return 0;
    }

    int64_t var1, var2, p;

    // FOrmula de compensaciOn de presiOn (versiOn 64 bits, recomendada)
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)calib->dig_P6;
    var2 = var2 + ((var1 * (int64_t)calib->dig_P5) << 17);
    var2 = var2 + (((int64_t)calib->dig_P4) << 35);

    var1 = ((var1 * var1 * (int64_t)calib->dig_P3) >> 8) +
           ((var1 * (int64_t)calib->dig_P2) << 12);

    var1 = (((((int64_t)1) << 47) + var1)) *
           ((int64_t)calib->dig_P1) >> 33;

    if (var1 == 0) 
    {
        return 0;  // Evitar divisiOn por cero
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)calib->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)calib->dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)calib->dig_P7) << 4);

    return (uint32_t)p;
}
// =======================================================================================

bool BMP280_ReadMeasuredData(BMP280_Data_t* data, BMP280_CalibData_t* calib) 
{
    if (data == NULL || calib == NULL) 
    {
        return false;
    }

    int32_t adc_T = 0;
    int32_t adc_P = 0;
    int32_t t_fine = 0;
    int32_t temp_comp = 0;
    uint32_t pres_comp = 0;

    // 1. Iniciar una mediciOn en modo forzado
    if (!BMP280_StartForcedMeasurement()) 
    {
        return false;
    }

    // 2. Esperar a que termine la conversiOn
    //    El tiempo de mediciOn depende del oversampling (máximo 43 ms para x16)
    if (!BMP280_WaitForMeasurement(100)) 
    {
        return false;
    }

    // 3. Leer los datos RAW
    if (!BMP280_ReadRawData(&adc_T, &adc_P)) 
    {
        return false;
    }

    // 4. Calcular la temperatura compensada
    temp_comp = BMP280_CompensateTemperature(adc_T, calib, &t_fine);

    // 5. Calcular la presiOn compensada (usa t_fine)
    pres_comp = BMP280_CompensatePressure(adc_P, calib, t_fine);

    // 6. Convertir a unidades físicas
    data->temperature = (float)temp_comp / 100.0f;          // °C
    data->pressure = (float)pres_comp / 256.0f / 100.0f;    // hPa (Q24.8 / 256 / 100)

    return true;
}
// =======================================================================================
// 4- FUNCIONES DE BAJO NIVEL (CALLBACKS):  Estas funciones deben implementarse usando las APIs de la plataforma seleccionada.
//                                          Se muestran las funciones con sus nombres, pero el usuario debera implemtar la logica
//                                          segun su plataforma.
//                                          Se implementara comunicacion I2C o SPI, debe seleccionar una de las dos. Luego implemntar
//                                          las APIs correspondientes segun la plataforma.
//NOTA: Con plataforma nos referimos a los diferentes microcontroladores
//*Raspberry Pi Pico
//*Arduino
//*ESP32
// =======================================================================================
/**
 * @brief Escribe un byte en un registro del BMP280.
 * @param reg_adrr Dirección del registro.
 * @param data Dato a escribir.
 * @return true si fue exitoso, false en caso de error.
 */
bool BMP280_WriteRegister(uint8_t reg_adrr, uint8_t data )
{
    // Verificar que la configuración I2C esté inicializada
    if (g_i2c_config.port == NULL) 
    {
        return false;
    }
    
    // Crear un buffer con la dirección del registro y el dato
    uint8_t buffer[2];
    buffer[0] = reg_adrr;
    buffer[1] = data;
    
    // Usar la dirección I2C almacenada en Current_config
    // Para acceder a Current_config, necesitamos que sea global.
    // Como es estática en bmp_280.c, la hacemos accesible con una función.
    // Alternativa: usar la dirección por defecto (0x76).
    uint8_t i2c_addr = BMP280_I2C_ADDR_PRIM;  // 0x76
    
    int result = i2c_write_blocking(g_i2c_config.port, i2c_addr, buffer, 2, false);
    
    return (result == 2);
}

/**
 * @brief Lee un byte de un registro del BMP280.
 * @param reg_adrr Dirección del registro.
 * @param data Puntero donde se almacenará el dato leído.
 * @return true si fue exitoso, false en caso de error.
 */
bool BMP280_ReadRegister(uint8_t reg_adrr, uint8_t* data)
{
    if (data == NULL || g_i2c_config.port == NULL) 
    {
        return false;
    }
    
    uint8_t i2c_addr = BMP280_I2C_ADDR_PRIM;  // 0x76
    
    // 1. Escribir la dirección del registro para seleccionarlo
    int result = i2c_write_blocking(g_i2c_config.port, i2c_addr, &reg_adrr, 1, true);
    if (result != 1) 
    {
        return false;
    }
    
    // 2. Leer un byte del registro seleccionado
    result = i2c_read_blocking(g_i2c_config.port, i2c_addr, data, 1, false);
    
    return (result == 1);
}

/**
 * @brief Lee múltiples bytes en ráfaga desde un registro del BMP280.
 * @param reg_adrr Dirección del primer registro.
 * @param buffer Puntero al buffer donde se almacenarán los datos.
 * @param len Número de bytes a leer.
 * @return true si fue exitoso, false en caso de error.
 */
bool BMP280_BurstReadRegister(uint8_t reg_adrr, uint8_t* buffer, uint8_t len)
{
    if (buffer == NULL || len == 0 || g_i2c_config.port == NULL) {
        return false;
    }
    
    uint8_t i2c_addr = BMP280_I2C_ADDR_PRIM;  // 0x76
    
    // 1. Escribir la dirección del registro para seleccionarlo
    int result = i2c_write_blocking(g_i2c_config.port, i2c_addr, &reg_adrr, 1, true);
    if (result != 1) {
        return false;
    }
    
    // 2. Leer 'len' bytes desde el registro seleccionado
    result = i2c_read_blocking(g_i2c_config.port, i2c_addr, buffer, len, false);
    
    return (result == len);
}