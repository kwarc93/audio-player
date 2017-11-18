/*
 * menudef.c
 *
 * Created: 2013-08-20 13:05:36
 *  Author: tmf
 */

#include "menu.h"
#include <util/delay.h>
#include <ST7565/ST7565.h>
#include <avr\pgmspace.h>

//Prototypy funkcji obs³ugi wybranej pozycji menu
void menufunc1();
void menufunc2();
void menufunc3();
void menufunc4();
void menufunc5();
void menufunc6();
void menufunc7();

#define PGM_STR(X) ((const __flash char[]) { X })


extern const __flash uint8_t image_data_battery[];
extern const __flash uint8_t image_data_bulb[];
extern const __flash uint8_t image_data_calculator[];
extern const __flash uint8_t image_data_floppydisc[];
extern const __flash uint8_t image_data_headphones[];
extern const __flash uint8_t image_data_laptop[];
extern const __flash uint8_t image_data_microphone[];
extern const __flash uint8_t image_data_photography[];
extern const __flash uint8_t image_data_plug[];
extern const __flash uint8_t image_data_radio[];
extern const __flash uint8_t image_data_tools[];
extern const __flash uint8_t image_data_arrowup[];


struct _menuitem const __flash menu;
struct _menuitem const __flash menuA1;
struct _menuitem const __flash menuB1;

struct _menuitem const __flash menuB3 = {image_data_arrowup, Menu_Back, &menuB1, 0, 0};
struct _menuitem const __flash menuB2 = {image_data_photography, menufunc7, &menuB1, 0, &menuB3};
struct _menuitem const __flash menuB1 = {image_data_radio, menufunc6, &menuA1, 0, &menuB2};

struct _menuitem const __flash menuA4 = {image_data_arrowup, Menu_Back, &menuA1, 0, 0};
struct _menuitem const __flash menuA3 = {image_data_microphone, menufunc5, &menuA1, 0, &menuA4};
struct _menuitem const __flash menuA2 = {image_data_laptop, 0, &menuA1, &menuB1, &menuA3};
struct _menuitem const __flash menuA1 = {image_data_headphones, menufunc4, &menu, 0, &menuA2};

struct _menuitem const __flash menu4 = {image_data_tools, 0, &menu, 0, 0};
struct _menuitem const __flash menu3 = {image_data_floppydisc, menufunc3, &menu, 0, &menu4};
struct _menuitem const __flash menu2 = {image_data_bulb, menufunc2, &menu, 0, &menu3};
struct _menuitem const __flash menu1 = {image_data_battery, 0, &menu, &menuA1, &menu2};

const __flash struct _menuitem const __flash menu = {image_data_calculator, menufunc1, 0, 0, &menu1};


extern const __flash uint8_t* const __flash system16_array[];
extern const __flash uint8_t* const __flash system12_array[];
extern const __flash uint8_t* const __flash system8_array[];

void menufunc1()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Menu_pic 1!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

void menufunc2()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Menu_pic 3!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

void menufunc3()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Menu_pic 4!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

void menufunc4()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Podmenu A1!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

void menufunc5()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Podmenu A3!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

void menufunc6()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Podmenu B1!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

void menufunc7()
{
	st7565r_Clear(false); //Wyczyœæ LCD
	st7565r_SetText(0, 0, PSTR("Podmenu B2!"), system8_array, false);
	st7565r_CpyDirtyPages();
	_delay_ms(2000);
}

