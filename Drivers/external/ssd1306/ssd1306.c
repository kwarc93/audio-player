
#include "ssd1306.h"
#include "gpio/gpio.h"
#include "misc.h"

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief GPIO definitions and routines for software SPI-Interface (hardware dependent)
 */
#define SSD1306_VCC_PIN		(0)
#define SSD1306_VCC_PORT	(GPIOB)
#define SSD1306_VCC_LOW()	(SSD1306_VCC_PORT->BSRR = 1 << (SSD1306_VCC_PIN + 16))
#define SSD1306_VCC_HIGH()	(SSD1306_VCC_PORT->BSRR = 1 << SSD1306_VCC_PIN)

#define SSD1306_GND_PIN		(12)
#define SSD1306_GND_PORT	(GPIOB)
#define SSD1306_GND_LOW()	(SSD1306_GND_PORT->BSRR = 1 << (SSD1306_GND_PIN + 16))
#define SSD1306_GND_HIGH()	(SSD1306_GND_PORT->BSRR = 1 << SSD1306_GND_PIN)

#define SSD1306_CLK_PIN	 	(14)
#define SSD1306_CLK_PORT	(GPIOB)
#define SSD1306_CLK_LOW()	(SSD1306_CLK_PORT->BSRR = 1 << (SSD1306_CLK_PIN + 16))
#define SSD1306_CLK_HIGH()	(SSD1306_CLK_PORT->BSRR = 1 << SSD1306_CLK_PIN)

#define SSD1306_MOSI_PIN	(8)
#define SSD1306_MOSI_PORT	(GPIOD)
#define SSD1306_MOSI_LOW()	(SSD1306_MOSI_PORT->BSRR = 1 << (SSD1306_MOSI_PIN + 16))
#define SSD1306_MOSI_HIGH()	(SSD1306_MOSI_PORT->BSRR = 1 << SSD1306_MOSI_PIN)

#define SSD1306_CS_PIN	  	(10)
#define SSD1306_CS_PORT		(GPIOD)
#define SSD1306_CS_LOW()	(SSD1306_CS_PORT->BSRR = 1 << SSD1306_CS_PIN << 16)
#define SSD1306_CS_HIGH()	(SSD1306_CS_PORT->BSRR = 1 << SSD1306_CS_PIN)

#define SSD1306_DC_PIN	  	(12)
#define SSD1306_DC_PORT		(GPIOD)
#define SSD1306_DC_LOW()	(SSD1306_DC_PORT->BSRR = 1 << (SSD1306_DC_PIN + 16))
#define SSD1306_DC_HIGH()	(SSD1306_DC_PORT->BSRR = 1 << SSD1306_DC_PIN)

#define SSD1306_RST_PIN	 	(14)
#define SSD1306_RST_PORT	(GPIOD)
#define SSD1306_RST_LOW()	(SSD1306_RST_PORT->BSRR = 1 << (SSD1306_RST_PIN + 16))
#define SSD1306_RST_HIGH()	(SSD1306_RST_PORT->BSRR = 1 << SSD1306_RST_PIN)

static void softspi_init()
{
	/* Enable GPIO clocks */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN
			| RCC_AHB2ENR_GPIODEN;
	__DSB();

	// Supply pins
	GPIO_PinConfig( SSD1306_GND_PORT, SSD1306_GND_PIN, GPIO_OUT_PP_2MHz );
	SSD1306_GND_LOW();
	GPIO_PinConfig( SSD1306_VCC_PORT, SSD1306_VCC_PIN, GPIO_OUT_PP_2MHz );
	SSD1306_VCC_HIGH();

	// SPI interface pins
	GPIO_PinConfig( SSD1306_CLK_PORT, SSD1306_CLK_PIN, GPIO_OUT_PP_100MHz );
	GPIO_PinConfig( SSD1306_MOSI_PORT, SSD1306_MOSI_PIN, GPIO_OUT_PP_100MHz );
	GPIO_PinConfig( SSD1306_CS_PORT, SSD1306_CS_PIN, GPIO_OUT_PP_100MHz );
	GPIO_PinConfig( SSD1306_DC_PORT, SSD1306_DC_PIN, GPIO_OUT_PP_100MHz );
	GPIO_PinConfig( SSD1306_RST_PORT, SSD1306_RST_PIN, GPIO_OUT_PP_100MHz );

}

static inline void softspi_write_bit( uint8_t byte, uint8_t mask )
{
	SSD1306_CLK_LOW();
	if( byte & mask )
		SSD1306_MOSI_HIGH();
	else
		SSD1306_MOSI_LOW();
	SSD1306_CLK_HIGH();
	nop();
}

static void softspi_write_byte( uint8_t byte )
{
	/* transmit msb first, sample at clock falling edge */

	softspi_write_bit( byte, (1 << 7) );
	softspi_write_bit( byte, (1 << 6) );
	softspi_write_bit( byte, (1 << 5) );
	softspi_write_bit( byte, (1 << 4) );
	softspi_write_bit( byte, (1 << 3) );
	softspi_write_bit( byte, (1 << 2) );
	softspi_write_bit( byte, (1 << 1) );
	softspi_write_bit( byte, (1 << 0) );
}

/*
 * @brief SSD1306 LCD driver routines
 * Display resolution: 128x64
 */

// Private variables
enum lcd_buffer
{
	DIRTY, CLEAN
};
enum ssd1306_instruction
{
	CMD, DATA
};

// @brief Video buffer in RAM (the last byte defines whether page must be redrawn)
static uint8_t lcd_buffer[LCD_HEIGHT / 8][LCD_WIDTH + 1];
static uint8_t lcd_x, lcd_y;

// Private functions

static void set_instruction( enum ssd1306_instruction dc, uint8_t byte )
{
	if( dc )
		SSD1306_DC_HIGH();
	else
		SSD1306_DC_LOW();

	SSD1306_CS_LOW();
	softspi_write_byte( byte );
	SSD1306_CS_HIGH();
}

static void clr_vram()
{
	uint8_t height = LCD_HEIGHT;
	uint8_t width = LCD_WIDTH;
	while( height-- )
	{
		lcd_x = 0;
		lcd_y = height;
		while( width-- )
			set_instruction( DATA, 0x00 );
		width = LCD_WIDTH;
	}
}

static inline void set_page_address( uint8_t address )
{
	address &= 0x0F;
	set_instruction( CMD, LCD_SET_PAGE | address );
}

static inline void set_column_address( uint8_t address )
{
	address &= 0x7F;
	set_instruction( CMD, LCD_SET_COL_HI | (address >> 4) );
	set_instruction( CMD, LCD_SET_COL_LO | (address & 0x0F) );
}

void write_data_at( uint8_t x, uint8_t y, uint8_t data )
{
	set_page_address( y );
	set_column_address( x );
	set_instruction( DATA, data );
}

// Public functions

void SSD1306_Init()
{

	softspi_init();
	delay_ms( 10 );

	SSD1306_RST_LOW();
	delay_ms( 10 );
	SSD1306_RST_HIGH();
	delay_ms( 10 );

	set_instruction( CMD, 0xAE ); // display off
	set_instruction( CMD, 0xD5 ); // clock
	set_instruction( CMD, 0x81 ); // upper nibble is rate, lower nibble is divisor
	set_instruction( CMD, 0xA8 ); // mux ratio
	set_instruction( CMD, 0x3F ); // rtfm
	set_instruction( CMD, 0xD3 ); // display offset
	set_instruction( CMD, 0x00 ); // rtfm
	set_instruction( CMD, 0x00 );
	set_instruction( CMD, 0x8D ); // charge pump
	set_instruction( CMD, 0x14 ); // enable
	set_instruction( CMD, 0x20 ); // memory addr mode
	set_instruction( CMD, 0x00 ); // horizontal
	set_instruction( CMD, 0xA1 ); // segment remap
	set_instruction( CMD, 0xA5 ); // display on
	set_instruction( CMD, 0xC8 ); // com scan direction
	set_instruction( CMD, 0xDA ); // com hardware cfg
	set_instruction( CMD, 0x12 ); // alt com cfg
	set_instruction( CMD, 0x81 ); // contrast aka current
	set_instruction( CMD, 0x7F ); // 128 is midpoint
	set_instruction( CMD, 0xD9 ); // precharge
	set_instruction( CMD, 0x11 ); // rtfm
	set_instruction( CMD, 0xDB ); // vcomh deselect level
	set_instruction( CMD, 0x20 ); // rtfm
	set_instruction( CMD, 0xA6 ); // non-inverted
	set_instruction( CMD, 0xA4 ); // display scan on
	set_instruction( CMD, 0xAF ); // drivers on

	clr_vram();

}

void SSD1306_SetPixel( uint8_t x, uint8_t y, _Bool isSet )
{
	lcd_x = x;
	lcd_y = y;
	if( isSet )
		lcd_buffer[y >> 3][x] |= (1 << (y % 8));
	else
		lcd_buffer[y >> 3][x] &= ~(1 << (y % 8));
	lcd_buffer[y >> 3][LCD_WIDTH] = DIRTY;
}

void SSD1306_CpyFramebuffer()
{
	uint8_t *ptr = (uint8_t*) lcd_buffer;
	for( uint8_t y = 0; y < (LCD_HEIGHT >> 3); y++ )
	{
		set_page_address( y );
		set_column_address( 0 );
		for( uint8_t x = 0; x < LCD_WIDTH; x++ )
			set_instruction( DATA, *ptr++ );
		ptr++; //Synchronizacja ze wzgl�du na bajt Dirty w buforze
	}
}

void SSD1306_CpyDirtyPages()
{
	uint8_t *ptr = (uint8_t*) lcd_buffer;
	for( uint8_t y = 0; y < (LCD_HEIGHT >> 3); y++ )
	{
		if( ptr[LCD_WIDTH] == DIRTY )  //Wysy�amy dane tylko je�li dana strona by�a zmieniona
		{
			set_page_address( y );
			set_column_address( 0 );
			for( uint8_t x = 0; x < LCD_WIDTH; x++ )
				set_instruction( DATA, *ptr++ );
			*ptr++ = CLEAN;                //Skasuj flag� Dirty
		}
		else
			ptr += LCD_WIDTH + 1;
	}
}

void SSD1306_Clear( _Bool color )
{
	uint8_t pix = 0;
	if( color )
		pix = 0xff;
	uint8_t *ptr = (uint8_t*) lcd_buffer;
	for( uint8_t y = 0; y < (LCD_HEIGHT >> 3); y++ )
	{
		for( uint16_t i = 0; i < LCD_WIDTH; i++ )
			*ptr++ = pix;
		*ptr++ = DIRTY; //Bajt dirty
	}
}

void SSD1306_MoveTo( uint8_t x, uint8_t y )
{
	lcd_x = x;
	lcd_y = y;
}

void SSD1306_DrawLine( uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, _Bool isSet )
{
	int dy = y1 - lcd_y;
	int dx = x1 - lcd_x;
	int8_t stepx, stepy;

	SSD1306_MoveTo( x0, y0 );

	if( dy < 0 )
	{
		dy = -dy;
		stepy = -1;
	}
	else
		stepy = 1;
	if( dx < 0 )
	{
		dx = -dx;
		stepx = -1;
	}
	else
		stepx = 1;
	dy <<= 1;
	dx <<= 1;

	SSD1306_SetPixel( lcd_x, lcd_y, isSet );
	if( dx > dy )
	{
		int fraction = dy - (dx >> 1);  // same as 2*dy - dx
		while( lcd_x != x1 )
		{
			if( fraction >= 0 )
			{
				lcd_y += stepy;
				fraction -= dx;          // same as fraction -= 2*dx
			}
			lcd_x += stepx;
			fraction += dy;              // same as fraction -= 2*dy
			SSD1306_SetPixel( lcd_x, lcd_y, isSet );
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		while( lcd_y != y1 )
		{
			if( fraction >= 0 )
			{
				lcd_x += stepx;
				fraction -= dy;
			}
			lcd_y += stepy;
			fraction += dx;
			SSD1306_SetPixel( lcd_x, lcd_y, isSet );
		}
	}
}

void SSD1306_DrawHLine( uint8_t x, _Bool isSet )
{
	x += lcd_x;
	for( uint8_t x1 = lcd_x; x1 < x; x1++ )
		SSD1306_SetPixel( x1, lcd_y, isSet );
}

void SSD1306_DrawRect( uint8_t x, uint8_t y, uint8_t width, uint8_t height, _Bool fill )
{
	uint8_t w, h;

	if( fill == true )
	{
		for( h = y; h != y + height; h++ )
		{
			for( w = x; w != x + width; w++ )
			{
				SSD1306_SetPixel( w, h, true );
			}
		}
	}
	else
	{
		for( w = x; w <= x + width; w++ )
		{
			SSD1306_SetPixel( w, y, true );
			SSD1306_SetPixel( w, y + height, true );
		}
		for( h = y + 1; h < y + height; h++ )
		{
			SSD1306_SetPixel( x, h, true );
			SSD1306_SetPixel( x + width, h, true );
		}
	}

}

void SSD1306_Circle( uint8_t cx, uint8_t cy, uint8_t radius, uint8_t attrs )
{
	int8_t x, y, xchange, ychange, radiusError;
	x = radius;
	y = 0;
	xchange = 1 - 2 * radius;
	ychange = 1;
	radiusError = 0;
	_Bool isSet = attrs & 1;

	while( x >= y )
	{
		if( (attrs & 2) == 0 )
		{
			SSD1306_SetPixel( cx + x, cy + y, isSet );
			SSD1306_SetPixel( cx - x, cy + y, isSet );
			SSD1306_SetPixel( cx - x, cy - y, isSet );
			SSD1306_SetPixel( cx + x, cy - y, isSet );
			SSD1306_SetPixel( cx + y, cy + x, isSet );
			SSD1306_SetPixel( cx - y, cy + x, isSet );
			SSD1306_SetPixel( cx - y, cy - x, isSet );
			SSD1306_SetPixel( cx + y, cy - x, isSet );
		}
		else
		{
			SSD1306_MoveTo( cx - x, cy + y );
			SSD1306_DrawHLine( 2 * x, isSet );
			SSD1306_MoveTo( cx - x, cy - y );
			SSD1306_DrawHLine( 2 * x, isSet );
			SSD1306_MoveTo( cx - y, cy + x );
			SSD1306_DrawHLine( 2 * y, isSet );
			SSD1306_MoveTo( cx - y, cy - x );
			SSD1306_DrawHLine( 2 * y, isSet );
		}

		y++;
		radiusError += ychange;
		ychange += 2;
		if( 2 * radiusError + xchange > 0 )
		{
			x--;
			radiusError += xchange;
			xchange += 2;
		}
	}
}

void SSD1306_SetText( uint8_t x, uint8_t y, const char *tekst, const uint8_t * const font[],
		_Bool invert )
{
	lcd_y = y;
	lcd_x = x;
	uint8_t rows = (uintptr_t) font[0]; //Pobierz wysoko�� fontu
	y += rows - 1;
	char ch;

	while( (ch = *tekst++) )  //Wy�wietl kolejne znaki a� do ko�ca tekstu (znaku NUL)
	{
		const uint8_t *znak = font[ch - 30]; //Adres pocz�tku opisu znaku
		uint8_t col = *znak++;                     //Szeroko�� znaku w pikselach
		uint8_t page = 0, coldesc = 0, colmask = 0;

		for( uint8_t ox = 0; ox < col; ox++ )        //Wy�wietlamy kolejne kolumny tworz�ce znak
		{
			uint8_t dispmask = 1 << (y % 8);
			for( uint8_t oy = 0; oy < rows; oy++ )   //Narysuj jedn� kolumn� znaku
			{
				if( colmask == 0 )
				{
					colmask = 0x80;
					coldesc = *znak++;             //Pobierz bajt opisu znaku
				}
				page = ((y - oy) >> 3) & 0b111;    //Zabezpieczenie przed zapisem poza bufor
				lcd_buffer[page][lcd_x] &= ~dispmask;
				if( coldesc & colmask )
					lcd_buffer[page][lcd_x] |= dispmask;
				if( invert )
					lcd_buffer[page][lcd_x] ^= dispmask;  //Dokonaj inwersji obrazu
				colmask >>= 1;
				dispmask >>= 1;
				if( dispmask == 0 )   //Przekraczamy stron� - nale�y zapisa� kompletny bajt
				{
					lcd_buffer[page][LCD_WIDTH] = DIRTY;
					dispmask = 0x80;
				}
			}
			if( dispmask != 0x80 )
				lcd_buffer[page][LCD_WIDTH] = DIRTY;
			lcd_x++;
			if( lcd_x == LCD_WIDTH )
				return; //Wychodzimy za LCD
		}
	}
}

void SSD1306_DrawBitmap( uint8_t x, uint8_t y, const uint8_t image[], _Bool invert )
{
	uint8_t page, imagedata = 0;
	uint8_t rows = image[1];                  //Pobierz szeroko�� bitmapy
	uint8_t cols = image[0];                  //Pobierz wysoko�� bitmapy
	const uint8_t *data = &image[2];  //Wska�nik do danych obrazu

	uint8_t datamask = 0x00;

	for( uint8_t ox = x; ox < (x + cols); ox++ )   //Rysujemy kolumny
	{
		page = (y >> 3) & 0b111;                 //Zabezpieczenie przed zapisem poza bufor
		uint8_t dispmask = 1 << (y & 0b0111);      //Maska pozycji od kt�rej rysujemy bitmap�

		for( uint8_t oy = y; oy < (y + rows); oy++ )
		{
			if( dispmask == 0 )        //Zapisujemy kolejne 8 bit�w
			{
				lcd_buffer[page][LCD_WIDTH] = DIRTY;
				dispmask = 0x01;
				page++;
			}
			if( datamask == 0 )        //Pobieramy kolejny bajt danych bitmapy
			{
				datamask = 0x80;
				imagedata = *data++;
			}

			lcd_buffer[page][ox] &= ~dispmask;
			if( imagedata & datamask )
				lcd_buffer[page][ox] |= dispmask;
			if( invert )
				lcd_buffer[page][ox] ^= dispmask;  //Dokonaj inwersji obrazu
			dispmask <<= 1;
			datamask >>= 1;
		}
	}
}
