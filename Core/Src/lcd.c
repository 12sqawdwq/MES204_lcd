
#include "lcd.h"
const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
const uint8_t ROW_20[] = {0x00, 0x40, 0x14, 0x54};
/************************************** Static declarations **************************************/

void lcd_write_data(Lcd_HandleTypeDef * lcd, uint8_t data);
void lcd_write_command(Lcd_HandleTypeDef * lcd, uint8_t command);
void lcd_write(Lcd_HandleTypeDef * lcd, uint8_t data, uint8_t len);


/******************************** 自定义字节组，用于定义小恐龙游戏中的字符数据 ******************************/
// 恐龙游戏自定义字符数据 (5x8 像素)

// CGRAM 地址 0x00: Grass/Empty
const uint8_t char_empty[8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// CGRAM 地址 0x01: Grass Frame 1
const uint8_t char_grass_f1[8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

// CGRAM 地址 0x02: Grass Frame 2
const uint8_t char_grass_f2[8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x18
};

// CGRAM 地址 0x03: Grass Frame 3 (Full)
const uint8_t char_grass_f3[8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x18, 0x1C
};

// CGRAM 地址 0x04: Dino Standing/Jump Mid
const uint8_t char_dino_stand[8] = {
	0x00, 0x04, 0x0A, 0x1F, 0x15, 0x1F, 0x0A, 0x0A
};

// CGRAM 地址 0x05: Dino Jumping Top
const uint8_t char_dino_jump[8] = {
	0x02, 0x06, 0x0E, 0x1F, 0x15, 0x1F, 0x00, 0x00
};

// CGRAM 地址 0x06: Dino Running Frame 1
const uint8_t char_dino_run1[8] = {
	0x00, 0x04, 0x0A, 0x1F, 0x15, 0x1F, 0x0A, 0x10
};

// CGRAM 地址 0x07: Dino Running Frame 2
const uint8_t char_dino_run2[8] = {
	0x00, 0x04, 0x0A, 0x1F, 0x15, 0x1F, 0x0A, 0x05
};

// 定义自定义字符数组的集合
const uint8_t *custom_chars[] = {
	char_empty, char_grass_f1, char_grass_f2, char_grass_f3,
	char_dino_stand, char_dino_jump, char_dino_run1, char_dino_run2
};

/************************************** Function definitions **************************************/

/**
 * Create new Lcd_HandleTypeDef and initialize the Lcd
 */
Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin, Lcd_ModeTypeDef mode)
{
	Lcd_HandleTypeDef lcd;

	lcd.mode = mode;

	lcd.en_pin = en_pin;
	lcd.en_port = en_port;

	lcd.rs_pin = rs_pin;
	lcd.rs_port = rs_port;

	lcd.data_pin = pin;
	lcd.data_port = port;

	Lcd_init(&lcd);

	return lcd;
}

/**
 * Initialize 16x2-lcd without cursor
 */
void Lcd_init(Lcd_HandleTypeDef * lcd)
{
	if(lcd->mode == LCD_4_BIT_MODE)
	{
			lcd_write_command(lcd, 0x33);
			lcd_write_command(lcd, 0x32);
			lcd_write_command(lcd, FUNCTION_SET | OPT_N);				// 4-bit mode
	}
	else
		lcd_write_command(lcd, FUNCTION_SET | OPT_DL | OPT_N);


	lcd_write_command(lcd, CLEAR_DISPLAY);						// Clear screen
	lcd_write_command(lcd, DISPLAY_ON_OFF_CONTROL | OPT_D);		// Lcd-on, cursor-off, no-blink
	lcd_write_command(lcd, ENTRY_MODE_SET | OPT_INC);			// Increment cursor
}

/**
 * Write a number on the current position
 */
void Lcd_int(Lcd_HandleTypeDef * lcd, int number)
{
	char buffer[11];
	sprintf(buffer, "%d", number);

	Lcd_string(lcd, buffer);
}

/**
 * Write a string on the current position
 */
void Lcd_string(Lcd_HandleTypeDef * lcd, char * string)
{
	for(uint8_t i = 0; i < strlen(string); i++)
	{
		lcd_write_data(lcd, string[i]);
	}
}

/**
 * Set the cursor position
 */
void Lcd_cursor(Lcd_HandleTypeDef * lcd, uint8_t row, uint8_t col)
{
	#ifdef LCD20xN
	lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_20[row] + col);
	#endif

	#ifdef LCD16xN
	lcd_write_command(lcd, SET_DDRAM_ADDR + ROW_16[row] + col);
	#endif
}

/**
 * Clear the screen
 */
void Lcd_clear(Lcd_HandleTypeDef * lcd) {
	lcd_write_command(lcd, CLEAR_DISPLAY);
}

void Lcd_define_char(Lcd_HandleTypeDef * lcd, uint8_t code, uint8_t bitmap[]){
	lcd_write_command(lcd, SETCGRAM_ADDR + (code << 3));
	for(uint8_t i=0;i<8;++i){
		lcd_write_data(lcd, bitmap[i]);
	}

}

/**
  * @brief  将自定义字符数据写入 LCD1602 的 CGRAM。
  * @param  lcd: LCD 句柄
  * @retval None
  */
/**
  * @brief  将自定义字符数据写入 LCD1602 的 CGRAM。
  * @param  lcd: LCD 句柄
  * @retval None
  */
void Lcd_load_custom_chars(Lcd_HandleTypeDef *lcd)
{
	uint8_t i, j;

	// CGRAM 地址从 0x40 开始
	// 修正：Lcd_command -> lcd_write_command
	lcd_write_command(lcd, 0x40);

	// 循环写入 8 个自定义字符 (i=0 到 i=7)
	for (i = 0; i < 8; i++)
	{
		// 每个字符由 8 个字节组成 (j=0 到 j=7)
		for (j = 0; j < 8; j++)
		{
			// 直接发送数据（RS=1）
			// 修正：Lcd_data -> lcd_write_data
			lcd_write_data(lcd, custom_chars[i][j]);
		}
	}
}

/************************************** Static function definition **************************************/

/**
 * Write a byte to the command register
 */
void lcd_write_command(Lcd_HandleTypeDef * lcd, uint8_t command)
{
	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_COMMAND_REG);		// Write to command register

	if(lcd->mode == LCD_4_BIT_MODE)
	{
		lcd_write(lcd, (command >> 4), LCD_NIB);
		lcd_write(lcd, command & 0x0F, LCD_NIB);
	}
	else
	{
		lcd_write(lcd, command, LCD_BYTE);
	}

}

/**
 * Write a byte to the data register
 */
void lcd_write_data(Lcd_HandleTypeDef * lcd, uint8_t data)
{
	HAL_GPIO_WritePin(lcd->rs_port, lcd->rs_pin, LCD_DATA_REG);			// Write to data register

	if(lcd->mode == LCD_4_BIT_MODE)
	{
		lcd_write(lcd, data >> 4, LCD_NIB);
		lcd_write(lcd, data & 0x0F, LCD_NIB);
	}
	else
	{
		lcd_write(lcd, data, LCD_BYTE);
	}

}

/**
 * Set len bits on the bus and toggle the enable line
 */
void lcd_write(Lcd_HandleTypeDef * lcd, uint8_t data, uint8_t len)
{
	// 1. 将4位数据并行发送到PC6-PC9
	for(uint8_t i = 0; i < len; i++)
	{
		HAL_GPIO_WritePin(lcd->data_port[i], lcd->data_pin[i],
			((data >> i) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}

	// 2. 产生一个精确的使能（EN）脉冲来锁存数据
	HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, GPIO_PIN_SET);

	// 产生一个短暂的延时（约几微秒）作为EN脉冲宽度。
	// 对于大多数MCU，几个NOP指令就足够了。
	__NOP();
	__NOP();
	__NOP();
	__NOP();

	HAL_GPIO_WritePin(lcd->en_port, lcd->en_pin, GPIO_PIN_RESET);
	volatile uint32_t delay_count = 15000;
	while(delay_count--);
}
