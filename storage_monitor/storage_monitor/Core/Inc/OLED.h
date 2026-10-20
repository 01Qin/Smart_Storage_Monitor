//#include "main.h"
#include <stdint.h>
#include "stm32g4xx_hal.h"


#define	i2cDisplayAdd 	0x3C<<1		//0x78 0x7A // this works 0X3C or 0X3D does not
//#define	i2c_display1_Add 	0x7a		//0x78 0x7A // this works 0X3C or 0X3D does not
#define	Display		&hi2c2, i2cDisplayAdd
//#define	Display_2		&hi2c1, i2c_display2_Add

#define SSD1306_LCDWIDTH      		128
#define SSD1306_LCDHEIGHT      		64
#define SSD1306_SETCONTRAST  		0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 		0xA5
#define SSD1306_NORMALDISPLAY 		0xA6
#define SSD1306_INVERTDISPLAY 		0xA7
#define SSD1306_DISPLAYOFF 			0xAE
#define SSD1306_DISPLAYON 			0xAF
#define SSD1306_SETDISPLAYOFFSET 	0xD3
#define SSD1306_SETCOMPINS 			0xDA
#define SSD1306_SETVCOMDETECT 		0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 	0xD5
#define SSD1306_SETPRECHARGE 		0xD9
#define SSD1306_SETMULTIPLEX 		0xA8
#define SSD1306_SETLOWCOLUMN 		0x00
#define SSD1306_SETHIGHCOLUMN 		0x10
#define SSD1306_SETSTARTLINE 		0x40
#define SSD1306_MEMORYMODE 			0x20
#define SSD1306_COLUMNADDR 			0x21
#define SSD1306_PAGEADDR   			0x22
#define SSD1306_COMSCANINC 			0xC0
#define SSD1306_COMSCANDEC 			0xC8
#define SSD1306_SEGREMAP 			0xA0
#define SSD1306_CHARGEPUMP 			0x8D
#define SSD1306_EXTERNALVCC 		0x1
#define SSD1306_SWITCHCAPVCC 		0x2

void OLED_command(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t c);
void OLED_data(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t c);
void OLED_Set_Pos(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y);
void OLED_Fill(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t bmp_data);
void OLED_CLS(I2C_HandleTypeDef *hi2c, uint8_t display_Add);
void SetStartColumn(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void SetAddressingMode(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void SetColumnAddress(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t a, uint8_t b);
void SetPageAddress(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t a, uint8_t b);
void SetStartLine(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void SetContrastControl(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Charge_Pump(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Segment_Remap(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Entire_Display(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Inverse_Display(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Multiplex_Ratio(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Display_On_Off(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void SetStartPage(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Common_Remap(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Display_Offset(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Display_Clock(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Precharge_Period(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_Common_Config(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_VCOMH(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t d);
void Set_NOP(I2C_HandleTypeDef *hi2c, uint8_t display_Add);
void OLED_Init(I2C_HandleTypeDef *hi2c, uint8_t display_Add);
void OLED_P6x8Char(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, char ch);
void OLED_P6x8Str(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, char ch[]);
void OLED_P8x16Str(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, char ch[]);
void OLED_PrintBMP(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bmp[]);
void OLED_PrintValueC(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, int8_t data);
void OLED_PrintValueUC(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, uint8_t data);
void OLED_PrintValueUC_2dig(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, uint8_t data);
void OLED_PrintValueUC_8x16(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, uint8_t data);
void OLED_PrintValueH1(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, int8_t data);
void OLED_PrintValueH1_8x16(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, int8_t data);
void OLED_PrintValueB(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, uint8_t data);
void OLED_PrintValueI(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, int16_t data);
void OLED_PrintValueI3(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, int16_t data);
void OLED_PrintValueFP(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, uint8_t data, uint8_t num);
void OLED_PrintValueF(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t x, uint8_t y, float data, uint8_t num);
void OLED_Cursor(I2C_HandleTypeDef *hi2c, uint8_t display_Add, uint8_t cursor_column, uint8_t cursor_row);
void OLED_Show_fan(I2C_HandleTypeDef *hi2c, uint8_t display_Add);
void OLED_Clear_fan(I2C_HandleTypeDef *hi2c, uint8_t display_Add);
