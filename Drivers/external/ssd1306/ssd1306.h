#ifndef SSD1306_H_
#define SSD1306_H_

#include "main.h"

#define lcd_width		128
#define lcd_height		64
#define lcd_height_b	8

#define space_char	1		// space between chars

#define BUFFER 1024

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
void clean_area(unsigned char,unsigned char,unsigned char,unsigned char);
void write_string(unsigned char,unsigned char,const char*);
void write_char(unsigned char,unsigned char,unsigned char);
void send_data_array(const char*,unsigned char);
void set_cursor(unsigned char,unsigned char);
void clr_VRAM(void);
void lcdCharBlk36(uint8_t x, uint8_t y, char s);
void lcdStringBlk36(uint8_t x, uint8_t y, char * s);
void lcdCharLite24(uint8_t x, uint8_t y, char s);
void lcdStringLite24(uint8_t x, uint8_t y, char * s);
void lcd_bitmap(const uint8_t *gData);
void set_pixel(uint8_t x, uint8_t y);
void lcd_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void show_buff(void);
void write_display(uint8_t data);
void clr_buff(void);

extern const unsigned char AGH_LOGO[];

#endif /*SSD1306_H_*/
