#include <stdio.h>
#include "pico/stdlib.h"
//Funciona en fisico
/* Exported constants --------------------------------------------------------*/
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

LCD_t lcd;
//----------------------------------------------------------------------------
static void lcd_setRowOffsets(LCD_t *lcd, int row0, int row1, int row2,int row3);
static void lcd_send(LCD_t *lcd, char data, uint8_t rs);
static void lcd_sendData(LCD_t *lcd, char data);
static void lcd_sendCmd(LCD_t *lcd, char cmd);
static void delay_us(uint16_t time);
void lcd_display(LCD_t *lcd);
void lcd_clear(LCD_t *lcd);
//----------------------------------------------------------------------------
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
    sleep_us(30);
    gpio_put(lcd->EN_pin,0);
    sleep_us(30);
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

static void delay_us(uint16_t time) 
{
	sleep_us(50);
}



int main()
{
    stdio_init_all();
		for (uint8_t GPIO = 0; GPIO < 6; GPIO++)
    {
        gpio_init(GPIO);
    }
    //configuro los pines
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
	lcd_setCursor(&lcd,0,0);
	lcd_print(&lcd,"UTN FRA");
	sleep_ms(3000);
	lcd_setCursor(&lcd,0,1);
	lcd_print(&lcd,"LCD 4 bits");
    while (true) 
    {
        
    }
}
