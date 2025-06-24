#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"
#include "bmp280.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hardware/pwm.h"
/*Consigna 2: Gestionar los recursos del microcontrolador para que compartiendo el mismo bus de I2C se pueda ver
* la temperatura en grados Celsius y la presión en KPa con su unidad en las primeras dos líneas del display LCD.
* Se debe colocar un puslador para intercambiar pantallas, en una pantalla se debera msotrar la temperatura y presion
* y en otra pantalla el error. Ademas se debe generar un PWM que encienda el led el cual sera proporcional al error.*/

/*Para esta parte se considero el uso de 4 tareas:
1- La tarea de inicializacion general configura el I2C junto a sus pines, inicializa el BMP280 y el LCD junto con 
mensajes de prueba para cada lina del LCD
2- La tarea task_bmp280 lee el estado cada 1 segundo y luego envia por una cola los datos a la latara task_lcd
3- La tarea task_lcd muestra los datos obtenido y convertido de temperatura y presion
4- La tarea task_button sera la que intercambie de pantallas una vez que la interrupcion la habilite.
NOTA: El caracter ° no se imprimie en e LCD
Se utilizo semaforo mutex para controlar el acceso al recusrso compartido I2C y una cola para la trasnferencia
segura de datos entre tareas. El semaforo binario se usao para habilita la tarea de cambio de pagina.*/

/*----------------------------------------MACROS-------------------------------------------------------------------*/
#define I2C         i2c0 //Utilizo el bus 0 del i2c
#define PIN_SDA     4 //Corresponde al pin 6 de la placa
#define PIN_SCL     5 //Corresponde al pin 7 de la placa
#define I2C_FREQ    400000 //Se define una frecuencia de 400 KHz
#define LCD_ADDR    0x27 //Direccion del LCD
#define BMP_ADDR    0xFF //Direccion del BMP280
#define PIN_BUTTON  6 //Corresponde al pin 8 de la placa
#define PIN_PWM     15//Corresponde al pin 20, slice 7, canal B
#define WRAP        12499//Ajusto para un 1KHz
#define DIV_CLOCK   10.0f//divisor de clock
/*------------------------------------Variables, incluidas las de FreeRTOS y estructuras-------------------------------*/
//---------------------------------------Estructura que alamacena los datos del BMP280----------------------------------
typedef struct 
{
    float temp;
    uint32_t pre;    
}data_t;

uint8_t pagina=0;
uint slice_num, chan;
pwm_config config;
//------------------------------------------------Manejadores de cola y semaforo-----------------------------------------
SemaphoreHandle_t sinc_i2c, button;
QueueHandle_t datos;
/*------------------------------------------------Tareas--------------------------------------------------------------*/
//------------------------------------------------Lectura y conversion de datos del BMP280-----------------------------
void task_bmp280(void* pvParameters)
{ struct bmp280_calib_param params;
  int32_t raw_temp, raw_presion;
  float temp_c;
  int32_t presion_pa;
  data_t tp;
  
  bmp280_get_calib_params(&params);
  
  while (1)
  {
    if(xSemaphoreTake(sinc_i2c, portMAX_DELAY) == pdTRUE)
    {
        bmp280_read_raw(&raw_temp, &raw_presion);
        temp_c = bmp280_convert_temp(raw_temp, &params);
        presion_pa = bmp280_convert_pressure(raw_presion, raw_temp, &params);

        tp.temp = temp_c;
        tp.pre = presion_pa; 

        printf("Desde la tarea BMP280\n");
        printf("Temperatura= %.2f °C\n",temp_c);
        printf("Presion= %ld Pa\n",presion_pa);
        xQueueSend(datos, &tp, pdMS_TO_TICKS(10));
        xSemaphoreGive(sinc_i2c);
        vTaskDelay(pdMS_TO_TICKS(1000)); //tiempo de espera
    }   
  }
}
//---------------------------------------------------Impresion de informacion-------------------------------------------
void task_lcd(void* pvParameters)
{ char msg1[30], msg2[30];
  data_t tplcd;
  float error, error_p1, error_p2, SetPoint=25.0;

  while (1)
  {
    if(xSemaphoreTake(sinc_i2c,portMAX_DELAY) == pdTRUE)
    {   
        switch (pagina)
        {
        case 0:
                lcd_set_cursor(2,0);
                lcd_string("Ejecutando tareas:");
                lcd_set_cursor(3,0);
                lcd_string("task_bmp280 task_lcd");
                vTaskDelay(pdMS_TO_TICKS(10));
                xQueueReceive(datos, &tplcd, pdMS_TO_TICKS(100));
                sprintf(msg1,"T= %.2f Celcius ",tplcd.temp);
                sprintf(msg2,"P= %ld Pascales ",tplcd.pre);
                //lcd_clear();
                lcd_set_cursor(0,0);
                lcd_string(msg1);
                lcd_set_cursor(1,0);
                lcd_string(msg2);

                printf("Desde la tarea LCD\n");
                printf("Temperatura= %.2f °C\n",tplcd.temp);
                printf("Presion= %ld Pa\n",tplcd.pre);
            break;
        case 1:
                xQueueReceive(datos, &tplcd, pdMS_TO_TICKS(100));
                //lcd_clear();
                error = SetPoint - tplcd.temp;
                error_p1 = error/SetPoint;
                error_p2 = error_p1*100;

                sprintf(msg1,"Set Point= %.2f",SetPoint);
                sprintf(msg2,"Error= %.2f ",error_p2);
                lcd_set_cursor(0,0);
                lcd_string(msg1);
                lcd_set_cursor(1,0);
                lcd_string(msg2);

                pwm_set_enabled(slice_num,true);
                uint16_t duty = 12499*error_p1;
                pwm_set_chan_level(slice_num, chan, 1000);
            break;
        default:
            break;
        }
    }
    xSemaphoreGive(sinc_i2c);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
//-----------------------------------------------Rutina ISR para interrupcion externa---------------------------------------
void gpio_callback(uint gpio, uint32_t event) 
{   BaseType_t HPT=pdFALSE;
    static uint32_t last_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    
    if(gpio == PIN_BUTTON && (now - last_time) > 250) 
    {  // 250ms debounce
        xSemaphoreGive(button);    
        last_time = now;
    }
    portYIELD_FROM_ISR(HPT);
}
//----------------------------------Tarea de inicializacion, solo se jecuta una vez y luego se elimina------------------
void init_general(void* pvParameters)
{
    //Inicializo y configuro el I2C0
    i2c_init(I2C, I2C_FREQ);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);
    //Configuro interrupcion
    gpio_init(PIN_BUTTON);
    gpio_set_dir(PIN_BUTTON, GPIO_IN);
    gpio_pull_down(PIN_BUTTON);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON,GPIO_IRQ_EDGE_RISE,true,&gpio_callback);
    //Configuro el PWM
    gpio_set_function(PIN_PWM,GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(PIN_PWM);
    chan = pwm_gpio_to_channel(PIN_PWM);
    config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, DIV_CLOCK); // Divisor de clock = 10 (para wrap manejable)
    pwm_config_set_wrap(&config, WRAP);   // wrap = 12499 (1 kHz)
    pwm_init(slice_num, &config, false);
    //Inicializo el BMP280
    bmp280_init(I2C);
    
    //Inicializo y configuro el LCD
    lcd_init(I2C,LCD_ADDR);
    lcd_clear();
    lcd_set_cursor(0,0);
    lcd_string("Procesando...");
    lcd_set_cursor(1,0);
    lcd_string("Sensor activado");
    lcd_set_cursor(2,0);
    lcd_string("LCD activado");
    lcd_set_cursor(3,0);
    lcd_string("Tarea Borrada");
    lcd_clear();

    vTaskDelete(NULL);
}
//----------------------------------------Tarea que responde a la interrupcion, habilitada desde la ISR---------------------
void task_button (void* pvParameters)
{

  while(true)
  {
    if(xSemaphoreTake(button, portMAX_DELAY) == pdTRUE)
    { 
      pagina = !pagina;
      lcd_clear();
      printf("Cambio de pagina\n");
    }
  }  
}
/*------------------------------------Progrma principal-------------------------------------------------------------------*/
int main()
{
    stdio_init_all();

    sinc_i2c = xSemaphoreCreateMutex();
    button = xSemaphoreCreateBinary();
    datos = xQueueCreate(5,sizeof(data_t));

    if(sinc_i2c == NULL || datos == NULL)
    {
        printf("Problemas para crear la cola o el semaforo revisar\n");
    }

    xTaskCreate(task_bmp280,"BMP280",256,NULL,1,NULL);
    xTaskCreate(task_lcd,"LCD",256,NULL,1,NULL);
    xTaskCreate(task_button,"BOTON",256,NULL,3,NULL);
    xTaskCreate(init_general,"Inicializacion",256,NULL,4,NULL);
    xSemaphoreGive(sinc_i2c);

    vTaskStartScheduler();

    while (true) 
    {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
