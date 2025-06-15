/*
* Autor: Calibeta
*
*The library was obtained of an project of Bluepill
*and it was modified for Raspberry Pico 2
*Library original:  https://github.com/CaliBeta/LCD1602-STM32.git
*YouTube: https://www.youtube.com/watch?v=RVZfUnlsx0A
*
**/

#include "lcd.h"
#include "hardware/gpio.h"

void lcd_begin(LCD_t *lcd, uint8_t cols, uint8_t rows, uint8_t dotsize) 
{
	// inicializa en modo 4 bits
	lcd->displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	if (rows > 1) lcd->displayfunction |= LCD_2LINE;
	lcd->rows = rows;
	lcd->cols = cols;
	lcd_setRowOffsets(lcd, 0x00, 0x40, 0x00 + cols, 0x40 + cols);

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != LCD_5x8DOTS) && (rows == 1)) 
    {
		lcd->displayfunction |= LCD_5x10DOTS;
	}

	sleep_ms(50);	// wait for >40ms
	lcd_sendCmd(lcd, 0x03);
	sleep_ms(5);		// wait for >4.1ms
	lcd_sendCmd(lcd, 0x03);
	sleep_us(150);	// wait for >100us
	lcd_sendCmd(lcd, 0x03);
	lcd_sendCmd(lcd, 0x02);	//set 4 bit mode
	sleep_ms(1);

	// finally, set interface type, # lines and font size
	lcd_sendCmd(lcd, LCD_FUNCTIONSET | lcd->displayfunction);
	//set display control D=1,C=0, B=0, display on, cursor off and cursor blink off
	lcd->displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_display(lcd);
	lcd_clear(lcd);  // clear display
	sleep_ms(2);
	// set the entry mode --> I/D = 1 (increment cursor) & S = 0 (no shift)
	lcd->displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	lcd_sendCmd(lcd, LCD_ENTRYMODESET | lcd->displaymode);
}
void lcd_home(LCD_t *lcd) 
{
	lcd_sendCmd(lcd, LCD_RETURNHOME);
	sleep_ms(2);
}
void lcd_clear(LCD_t *lcd)
{
	lcd_sendCmd(lcd, LCD_CLEARDISPLAY);
}
void lcd_noDisplay(LCD_t *lcd) 
{
	lcd->displaycontrol &= ~LCD_DISPLAYON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
void lcd_display(LCD_t *lcd) 
{
	lcd->displaycontrol |= LCD_DISPLAYON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
void lcd_noBlink(LCD_t *lcd) 
{
	lcd->displaycontrol &= ~LCD_BLINKON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
void lcd_blink(LCD_t *lcd) 
{
	lcd->displaycontrol |= LCD_BLINKON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
void lcd_noCursor(LCD_t *lcd) 
{
	lcd->displaycontrol &= ~LCD_CURSORON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
void lcd_cursor(LCD_t *lcd) 
{
	lcd->displaycontrol |= LCD_CURSORON;
	lcd_sendCmd(lcd, LCD_DISPLAYCONTROL | lcd->displaycontrol);
}
void lcd_scrollDisplayLeft(LCD_t *lcd) 
{
	lcd_sendCmd(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_scrollDisplayRight(LCD_t *lcd) 
{
	lcd_sendCmd(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}
void lcd_setCursor(LCD_t *lcd, uint8_t col, uint8_t row) 
{
	const size_t max_rows = sizeof(lcd->row_offsets) / sizeof(*lcd->row_offsets);
	if (row >= max_rows) {
		row = max_rows - 1;    // we count rows starting w/0
	}
	if (row >= lcd->rows) {
		row = lcd->rows - 1;    // we count rows starting w/0
	}

	lcd_sendCmd(lcd, LCD_SETDDRAMADDR | (col + lcd->row_offsets[row]));
}
void lcd_print(LCD_t *lcd, char *str) 
{
	while (*str)
		lcd_sendData(lcd, *str++);
}
void lcd_write(LCD_t *lcd, uint8_t data) 
{
	lcd_sendData(lcd, data);
}
void lcd_createChar(LCD_t *lcd, char *data, uint8_t pos) 
{
	if (pos > 8) pos = 8;

	uint8_t cmd = LCD_SETCGRAMADDR + (pos * 8);
	lcd_sendCmd(lcd, cmd);
	for (int i = 0; i < 8; i++)
		lcd_sendData(lcd, data[i]);
}
static void lcd_setRowOffsets(LCD_t *lcd, int row0, int row1, int row2,int row3) 
{
	lcd->row_offsets[0] = row0;
	lcd->row_offsets[1] = row1;
	lcd->row_offsets[2] = row2;
	lcd->row_offsets[3] = row3;
}

static void lcd_send(LCD_t *lcd, char data, uint8_t rs) {
	// rs = 1 for data, rs=0 for command
    gpio_put(lcd->RS_pin,rs);
	// write the data to the respective pin
    gpio_put(lcd->D7_pin,((data >> 3) & 0x01));
    gpio_put(lcd->D6_pin,((data >> 2) & 0x01));
    gpio_put(lcd->D5_pin,((data >> 1) & 0x01));
    gpio_put(lcd->D4_pin,((data >> 0) & 0x01));

    gpio_put(lcd->EN_pin,1);
    sleep_us(2);
    gpio_put(lcd->EN_pin,0);
    sleep_us(2);
}

static void lcd_sendData(LCD_t *lcd, char data) {
	char _data;

	// send higher nibble
	_data = ((data >> 4) & 0x0f);
	lcd_send(lcd, _data, 1);  // rs =1 for sending data

	// send Lower nibble
	_data = ((data) & 0x0f);
	lcd_send(lcd, _data, 1);
}

static void lcd_sendCmd(LCD_t *lcd, char cmd) 
{
	char _cmd;

	// send higher nibble
	_cmd = ((cmd >> 4) & 0x0f);
	lcd_send(lcd, _cmd, 0);  // rs = 0 for sending command

	// send Lower nibble
	_cmd = ((cmd) & 0x0f);
	lcd_send(lcd, _cmd, 0);
}
