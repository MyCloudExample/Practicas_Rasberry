/*
* Autor: Calibeta
*
*The library was obtained of an project of Bluepill
*and it was modified for Raspberry Pico 2
*Library original:  https://github.com/CaliBeta/LCD1602-STM32.git
*YouTube: https://www.youtube.com/watch?v=RVZfUnlsx0A
*
**/

#ifndef MI_LIBRERIA_H
#define MI_LIBRERIA_H

#include "pico/stdlib.h"
// commands
#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME 			0x02
#define LCD_ENTRYMODESET 		0x04
#define LCD_DISPLAYCONTROL	0x08
#define LCD_CURSORSHIFT 		0x10
#define LCD_FUNCTIONSET 		0x20
#define LCD_SETCGRAMADDR 		0x40
#define LCD_SETDDRAMADDR 		0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 					0x00
#define LCD_ENTRYLEFT 					0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 		0x04
#define LCD_DISPLAYOFF 		0x00
#define LCD_CURSORON 			0x02
#define LCD_CURSOROFF			0x00
#define LCD_BLINKON 			0x01
#define LCD_BLINKOFF 			0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 	0x08
#define LCD_CURSORMOVE 		0x00
#define LCD_MOVERIGHT 		0x04
#define LCD_MOVELEFT 			0x00

// flags for function set
#define LCD_8BITMODE 			0x10
#define LCD_4BITMODE 			0x00
#define LCD_2LINE 				0x08
#define LCD_1LINE 				0x00
#define LCD_5x10DOTS 			0x04
#define LCD_5x8DOTS 			0x00

typedef struct 
{
	uint16_t RS_pin;
	uint16_t RW_pin;
	uint16_t EN_pin;
	uint16_t D4_pin;
	uint16_t D5_pin;
	uint16_t D6_pin;
	uint16_t D7_pin;
  uint8_t displaycontrol; 	
	uint8_t displayfunction;	
	uint8_t displaymode;			
	uint8_t row_offsets[4];	//
	uint8_t cols; //nuemro de fila
	uint8_t rows; //nuemro de columnas			
} LCD_t;

//----------------------------------------------------------------------------
static void lcd_setRowOffsets(LCD_t *lcd, int row0, int row1, int row2,int row3);
static void lcd_send(LCD_t *lcd, char data, uint8_t rs);
static void lcd_sendData(LCD_t *lcd, char data);
static void lcd_sendCmd(LCD_t *lcd, char cmd);
void lcd_begin(LCD_t *lcd, uint8_t cols, uint8_t rows, uint8_t dotsize);
void lcd_home(LCD_t *lcd);
void lcd_clear(LCD_t *lcd);
void lcd_noDisplay(LCD_t *lcd);
void lcd_display(LCD_t *lcd);
void lcd_noBlink(LCD_t *lcd);
void lcd_blink(LCD_t *lcd);
void lcd_noCursor(LCD_t *lcd);
void lcd_cursor(LCD_t *lcd);
void lcd_scrollDisplayLeft(LCD_t *lcd);
void lcd_scrollDisplayRight(LCD_t *lcd);
void lcd_setCursor(LCD_t *lcd, uint8_t col, uint8_t row);
void lcd_print(LCD_t *lcd, char *str);
void lcd_write(LCD_t *lcd, uint8_t data);
void lcd_createChar(LCD_t *lcd, char *data, uint8_t pos);

#endif