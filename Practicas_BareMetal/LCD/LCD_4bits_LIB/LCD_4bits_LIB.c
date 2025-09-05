#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd.h"

void lcd_Pin();

LCD_t lcd;
int contador=0;
char buffer[10];

int main()
{
    stdio_init_all();
		for (uint8_t GPIO = 0; GPIO < 6; GPIO++)
    {
        gpio_init(GPIO);
    }  
	
	lcd_Pin();
  //inicio el LCD y lo configuro
    lcd_begin(&lcd,16,2,LCD_5x8DOTS);
    lcd_home(&lcd);
    lcd_clear(&lcd);
	lcd_cursor(&lcd);
	sleep_ms(2000);
	lcd_noCursor(&lcd);
	lcd_blink(&lcd);
	sleep_ms(2000);
	lcd_noBlink(&lcd);
	lcd_print(&lcd,"HOLA MUNDO");

  while (true) 
  {
	lcd_setCursor(&lcd,0,1);
	sprintf(buffer,"Contador %d ",contador);
	lcd_print(&lcd,buffer);
	sleep_ms(1000);
	contador++;    
  }
}

void lcd_Pin()
{
    lcd.RS_pin = 0;
    lcd.EN_pin = 1;
    lcd.D4_pin = 2;
    lcd.D5_pin = 3;
	lcd.D6_pin = 4;
    lcd.D7_pin = 5;
	gpio_set_dir(lcd.D7_pin,GPIO_OUT);
    gpio_set_dir(lcd.D6_pin,GPIO_OUT);
    gpio_set_dir(lcd.D5_pin,GPIO_OUT);
    gpio_set_dir(lcd.D4_pin,GPIO_OUT);
    gpio_set_dir(lcd.RS_pin,GPIO_OUT);
    gpio_set_dir(lcd.EN_pin,GPIO_OUT);
}