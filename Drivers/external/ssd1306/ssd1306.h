#ifndef SSD1306_H_
#define SSD1306_H_

#include "main.h"

#define LCD_WIDTH		128
#define LCD_HEIGHT		64

// commands SSD1306 controller
#define LCD_SET_COL_HI		0x10
#define LCD_SET_COL_LO		0x00
#define LCD_SET_LINE		0x40
#define LCD_SET_CONTRAST	0x81
#define LCD_SET_SEG_REMAP0  0xA0
#define LCD_SET_SEG_REMAP1	0xA1
#define LCD_EON_OFF			0xA4
#define LCD_EON_ON			0xA5
#define LCD_DISP_NOR		0xA6
#define LCD_DISP_REV		0xA7
#define LCD_MULTIPLEX       0xA8
#define LCD_CHARGE_PUMP    	0x8D
#define LCD_PUMP_OFF    	0x10
#define LCD_PUMP_ON     	0x14
#define LCD_DISP_OFF 		0xAE
#define LCD_DISP_ON			0xAF
#define LCD_SET_PAGE		0xB0
#define LCD_SET_SCAN_FLIP	0xC0
#define LCD_SET_SCAN_NOR	0xC8
#define LCD_SET_OFFSET		0xD3
#define LCD_SET_RATIO_OSC	0xD5
#define LCD_SET_CHARGE  	0xD9
#define LCD_SET_PADS    	0xDA
#define LCD_SET_VCOM    	0xDB
#define LCD_NOP     		0xE3
#define LCD_SCROLL_RIGHT	0x26
#define LCD_SCROLL_LEFT		0x27
#define LCD_SCROLL_VR	    0x29
#define LCD_SCROLL_VL		0x2A
#define LCD_SCROLL_OFF		0x2E
#define LCD_SCROLL_ON   	0x2F
#define LCD_SCROLL_ON   	0x2F
#define LCD_VERT_SCROLL_A  	0xA3
#define LCD_MEM_ADDRESSING 	0x20
#define LCD_SET_COL_ADDR	0x21
#define LCD_SET_PAGE_ADDR	0x22

void SSD1306_Init();
void SSD1306_SetPixel(uint8_t x, uint8_t y, _Bool isSet);
void SSD1306_CpyFramebuffer();
void SSD1306_CpyDirtyPages();
void SSD1306_Clear(_Bool color);
void SSD1306_MoveTo(uint8_t x, uint8_t y);
void SSD1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, _Bool isSet);
void SSD1306_DrawHLine(uint8_t x, _Bool isSet);
void SSD1306_DrawRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, _Bool isSet);
void SSD1306_Circle(uint8_t cx, uint8_t cy , uint8_t radius, uint8_t attrs);
void SSD1306_SetText(uint8_t x, uint8_t y, const char *tekst, const uint8_t * const font[], _Bool invert);
void SSD1306_DrawBitmap(uint8_t x, uint8_t y, const uint8_t image[], _Bool invert);



#endif /*SSD1306_H_*/
