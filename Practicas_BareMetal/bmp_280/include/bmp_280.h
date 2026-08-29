#ifndef BMP_280_H
#define BMP_280_H
#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h" //Para las APIs de la Raspberry Pi Pico

//=======================================================================================
// 1- Direcciones de registros
//=======================================================================================
#define BMP_280_ID                          0xD0
#define BMP_280_RESET                       0xE0
#define BMP_280_STATUS                      0xF3
#define BMP_280_CTRL_MEAS                   0xF4
#define BMP_280_CONFIG                      0xF5
#define BMP_280_PRESS_MSB                   0xF7
#define BMP_280_PRESS_LSB                   0xF8
#define BMP_280_PRESS_XLSB                  0xF9
#define BMP_280_TEMP_MSB                    0xFA
#define BMP_280_TEMP_LSB                    0xFB
#define BMP_280_TEMP_XLSB                   0xFC
//=======================================================================================
// 2- Comandos de uso comun
//=======================================================================================
#define BMP_280_RESET_CMD                   0xB6 //Reset BMP280
#define BMP_280_ID_CHIP                     0x58 //Id sensor
#define BMP280_I2C_ADDR_PRIM                0x76  /**< Dirección primaria (SDO = 0) */
#define BMP280_I2C_ADDR_SEC                 0x77  /**< Dirección secundaria (SDO = 1) */
//=======================================================================================
// 3- Comandos para los registros de configruacion, CONFIG y CTRL_MEAS
//=======================================================================================
// 3.1- Registro: CONFIG
// Bits t_bs
#define BMP_280_CONFIG_T_BS_0_5             0x00 // Standby time 0,5 mseg
#define BMP_280_CONFIG_T_BS_62_5            0x20 // Standby time 62,5 mseg
#define BMP_280_CONFIG_T_BS_125             0x40 // Standby time 125 mseg
#define BMP_280_CONFIG_T_BS_250             0x60 // Standby time 250 mseg
#define BMP_280_CONFIG_T_BS_500             0x80 // Standby time 500 mseg
#define BMP_280_CONFIG_T_BS_1000            0xA0 // Standby time 1000 mseg
#define BMP_280_CONFIG_T_BS_2000            0xC0 // Standby time 2000 mseg
#define BMP_280_CONFIG_T_BS_4000            0xE0 // Standby time 4000 mseg
// Bits filter
#define BMP_280_CONFIG_FILTER_OFF           0x00 // Constant filter OFF      
#define BMP_280_CONFIG_FILTER_C2            0x04 // Constant filter 2
#define BMP_280_CONFIG_FILTER_C4            0x08 // Constant filter 4
#define BMP_280_CONFIG_FILTER_C8            0x0C // Constant filter 8
#define BMP_280_CONFIG_FILTER_C16           0x10 // Constant filter 16
// Bits spi2w_en
#define BMP_280_CONFIG_SPI3W_EN_4W          0x00 // SPI Mode 4 wire
#define BMP_280_CONFIG_SPI3W_EN_3W          0x01 // SPI Mode 3 wire
//---------------------------------------------------------------------------------------
// 3.2 Registro: CTRL_MEAS
// Bits osr_t
#define BMP_280_CTRL_MEAS_OSR_T_OFF         0x00 //OFF oversamplig
#define BMP_280_CTRL_MEAS_OSR_T_X1          0x20 //Oversampling +1
#define BMP_280_CTRL_MEAS_OSR_T_X2          0x40 //Oversampling +2
#define BMP_280_CTRL_MEAS_OSR_T_X4          0x60 //Oversampling +4
#define BMP_280_CTRL_MEAS_OSR_T_X8          0x80 //Oversampling +8
#define BMP_280_CTRL_MEAS_OSR_T_X16         0xA0 //Oversampling +16
// Bits osr_p
#define BMP_280_CTRL_MEAS_OSR_P_OFF         0x00 //OFF oversampling
#define BMP_280_CTRL_MEAS_OSR_P_X1          0x04 //Oversampling +1
#define BMP_280_CTRL_MEAS_OSR_P_X2          0x08 //Oversampling +2
#define BMP_280_CTRL_MEAS_OSR_P_X4          0x0C //Oversampling +4
#define BMP_280_CTRL_MEAS_OSR_P_X8          0x10 //Oversampling +8
#define BMP_280_CTRL_MEAS_OSR_P_X16         0x14 //Oversampling +16
// Bits mode
#define BMP_280_CTRL_MEAS_MODE_SLEEP        0x00 //Mode Sleep
#define BMP_280_CTRL_MEAS_MODE_NORMAL       0x03 //Mode Normal
#define BMP_280_CTRL_MEAS_MODE_FORCED       0x01 //Mode Forced, it can used 0x02 too
//=======================================================================================
// 4- Regsitro STATUS, mascara para carga de datos de calibracion y mascara para fin de conversion
//=======================================================================================
#define BMP_280_STATUS_IM_UPDATE            (1 << 0) //Mask used to know whether calibration data has been load. Bit=1 (load), Bit=0 (end)
#define BMP_280_STATUS_MEASURING            (1 << 3) //Mask used to know whether the medition/convertion has been terminated. Bit=1 (convertion), Bit=0 (end)            
//=======================================================================================
// 5- Regsiter CALIBRATION
//=======================================================================================
#define BMP_280_CALIBRATION_START           0x88 // Firt register (calibration) 
#define BMP_280_CALIBRATION_END             0xA1 // Last regsiter
#define BMP_280_CALIBRATION_SIZE            24   // Size to buffer (uint8_t)
//=======================================================================================
// 6- Registros de temperatura y presion. Se crea un buffer de 6 bytes que contiene los RAWs
//=======================================================================================
#define BMP_280_RAW_DATA_SIZE               6   // Total of data RAW: 3 bytes temperature and 3 bytes presion. It is buffer with data type Unint32_t
//=======================================================================================
// 7- Estructura de datos
//=======================================================================================
/** 
 * @brief           La estructura BMP280_Calib_t almacenara los coeficientes de calibracion.
 *                  Estos coeficientes fueron leidos previamente y luego armados.
 *                  Se cargan durante el inicio del sensor y son unicos para cada sensor.
 *                  Se debe declarar como variable global.   
*/
typedef struct 
{
    uint16_t dig_T1;  //Coeficiente de temperatura 1 (unsigned)
    int16_t  dig_T2;  //Coeficiente de temperatura 2 (signed)
    int16_t  dig_T3;  //Coeficiente de temperatura 3 (signed)
    uint16_t dig_P1;  //Coeficiente de presión 1 (unsigned)
    int16_t  dig_P2;  //Coeficiente de presión 2 (signed)
    int16_t  dig_P3;  //Coeficiente de presión 3 (signed)
    int16_t  dig_P4;  //Coeficiente de presión 4 (signed)
    int16_t  dig_P5;  //Coeficiente de presión 5 (signed)
    int16_t  dig_P6;  //Coeficiente de presión 6 (signed)
    int16_t  dig_P7;  //Coeficiente de presión 7 (signed)
    int16_t  dig_P8;  //Coeficiente de presión 8 (signed)
    int16_t  dig_P9;  //Coeficiente de presión 9 (signed)
} BMP280_CalibData_t;

/**  
 * @brief           Son los valores de configuracion de los parametros.
 *                  Los valores fueron definidos en esta libreria.
 *                  El BMP280_COnfig_t se define como una varaible en el codigo.
 */
typedef struct 
{
    uint8_t osrs_t;      // Oversampling de temperatura (usar BMP_280_CTRL_OSR_T_*)
    uint8_t osrs_p;      // Oversampling de presión (usar BMP_280_CTRL_OSR_P_*)
    uint8_t mode;        // Modo de energía (usar BMP_280_CTRL_MODE_*)
    uint8_t standby;     // Tiempo de standby (usar BMP_280_CONFIG_T_SB_*)
    uint8_t filter;      // Coeficiente del filtro IIR (usar BMP_280_CONFIG_FILTER_*)
    bool spi3w_en;       // true = SPI 3 hilos, false = SPI 4 hilos (por defecto)
    uint8_t i2c_addr;    // Dirección I2C (BMP_280_I2C_ADDR_PRIM o BMP_280_I2C_ADDR_SEC)
} BMP280_Config_t;

/**
 * @brief           Almacena los valores reales de las magnitudes de presion y temperatura.
 */ 
typedef struct 
{
    float temperature;   // Temperatura en °C (ej. 25.08)
    float pressure;      // Presión en hPa (ej. 963.86)
} BMP280_Data_t;
//=======================================================================================
// 8- Funciones de bajo nivel: estas funciones dependenra de la plataforma que se utilice.
//=======================================================================================
/**
 * @brief           Escribe un comando en un registro
 * @param reg_adrr  Direccion del regsitro a escribir.
 * @param data      Comando del registro
 * @return          True si la escritura fue exitosa, en cado contrario False
 */
bool BMP280_WriteRegister(uint8_t reg_adrr, uint8_t data);

/**
 * @brief           Lee un registro
 * @param reg_adrr  Direccion del regsitro a escribir.
 * @param data      Puntero a donde se almaceanra el dato leido.
 * @return          True si la lectura fue exitosa, en cado contrario False
 */
bool BMP280_ReadRegister(uint8_t reg_adrr, uint8_t* data);

/**
 * @brief           Lee un registro
 * @param reg_adrr  Direccion del regsitro a leer.
 * @param buffer    Vector donde se almaceanra la lectura de varios bytes.
 * @param len       Longitud del buffer, esto se refiere a cuantos datos debere leer para detener la lectura.    
 * @return          True si la lectura fue exitosa, en cado contrario False
 */
bool BMP280_BurstReadRegister(uint8_t reg_adrr, uint8_t* buffer, uint8_t len);

//=======================================================================================
// 9- Funciones de uso del usuario. Estas seran las funciones de la libreria que se usaran en el codigo por parte del porgramador
//=======================================================================================
/**
 * @brief           Funcion para iniciar el sensor.
 * @param config    Configuracion del sensor indicada por el ussuario.
 * @param calib     Datos de calibracion leidos del sensor, procesados y luego colocados en calib para su posterior uso.    
 * @return          True si fue exitos, en caso contrario False
 */
bool BMP280_Init(BMP280_Config_t* config, BMP280_CalibData_t* calib );

/**
 * @brief           Funcion para procesar y cargar los datos de calibracion.
 * @param calib     Datos de calibracion leidos del sensor, procesados y luego colocados en calib para su posterior uso.    
 * @return          True si fue exitos, en caso contrario False
 */
bool BMP280_ReadCalibration(BMP280_CalibData_t* calib);

/**
 * @brief           Reinicia el sensor (soft reset).
 * @return          True si fue exitoso, False en caso de error.
 */
bool BMP280_Reset(void);

/**
 * @brief           Verifica el ID del chip (debe ser 0x58).
 * @return          True si el ID es correcto, False en caso contrario.
 */
bool BMP280_CheckChipID(void);

/**
 * @brief           Configura el sensor según los parámetros proporcionados.
 * @param config    Puntero a la estructura de configuración.
 * @return          True si fue exitoso, False en caso de error.
 */
bool BMP280_SetConfig(BMP280_Config_t *config);

/**
 * @brief           Inicia una medición en modo forzado.
 * @return          True si fue exitoso, False en caso de error.
 */
bool BMP280_StartForcedMeasurement(void);

/**
 * @brief           Espera a que termine la medición en curso.
 * @param timeout_ms Tiempo máximo de espera en milisegundos.
 * @return           True si terminó, False si se agotó el tiempo.
 */
bool BMP280_WaitForMeasurement(uint32_t timeout_ms);

/**
 * @brief           Lee los datos RAW de presión y temperatura.
 * @param adc_T     Puntero donde se almacenará el valor RAW de temperatura.
 * @param adc_P     Puntero donde se almacenará el valor RAW de presión.
 * @return          True si fue exitoso, False en caso de error.
 */
bool BMP280_ReadRawData(int32_t *adc_T, int32_t *adc_P);

/**
 * @brief           Calcula la temperatura compensada.
 * @param adc_T     Valor RAW de temperatura.
 * @param calib     Puntero a los datos de calibración.
 * @param t_fine    Puntero donde se almacenará t_fine (para presión).
 * @return          Temperatura en °C multiplicada por 100 (ej. 2508 = 25.08 °C).
 */
int32_t BMP280_CompensateTemperature(int32_t adc_T, BMP280_CalibData_t *calib, int32_t *t_fine);

/**
 * @brief           Calcula la presión compensada.
 * @param adc_P     Valor RAW de presión.
 * @param calib     Puntero a los datos de calibración.
 * @param t_fine    Valor intermedio de temperatura.
 * @return          Presión en Pascales (Pa). En formato Q24.8.
 */
uint32_t BMP280_CompensatePressure(int32_t adc_P, BMP280_CalibData_t *calib, int32_t t_fine);

/**
 * @brief           Realiza una medición completa: inicia, espera, lee y compensa.
 * @param data      Puntero donde se almacenarán los resultados.
 * @param calib     Puntero a los datos de calibración.
 * @return          True si fue exitoso, False en caso de error.
 */
bool BMP280_ReadMeasuredData(BMP280_Data_t *data, BMP280_CalibData_t *calib);

//========================================================================================
// Estas funciones solo se agregan de maenra opcional
//========================================================================================
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
void BMP280_SetI2CConfig(i2c_inst_t* port, uint8_t sda_pin, uint8_t scl_pin, uint32_t baudrate);

// Funciones especificas para la RAspberry Pi Pico
void BMP280_SetI2CConfig(i2c_inst_t* port, uint8_t sda_pin, uint8_t scl_pin, uint32_t baudrate);
uint8_t BMP280_GetI2CAddress(void); 

#endif 