/*
 * menudef.c
 *
 * Created: 2013-08-20 13:05:36
 *  Author: tmf
 */

#include "misc.h"
#include "Menu/menu.h"
#include "ssd1306/ssd1306.h"
#include "FreeRTOS/FreeRTOS.h"
#include "controller/controller.h"

//Prototypy funkcji obs³ugi wybranej pozycji menu
void menu_music();
void menu_recorder();
void menu_settings();
void menu_shutdown();
void menu_music_play();

extern const uint8_t image_data_off[];
extern const uint8_t image_data_floppydisc[];
extern const uint8_t image_data_headphones[];
extern const uint8_t image_data_microphone[];
extern const uint8_t image_data_plug[];
extern const uint8_t image_data_tools[];
extern const uint8_t image_data_arrowup[];


struct _menuitem const menu;

// Music menu text items (temporary, only for tests)
struct _menuitem const s5_menu = {0, 0, "<back>", Menu_Back, &menu, 0, 0 };
struct _menuitem const s4_menu = {0, 0, "flac_ex.flac", menu_music_play, &menu, 0, &s5_menu };
struct _menuitem const s3_menu = {0, 0, "aac_ex.aac", menu_music_play, &menu, 0, &s4_menu };
struct _menuitem const s2_menu = {0, 0, "mp3_ex.mp3", menu_music_play, &menu, 0, &s3_menu };
struct _menuitem const s1_menu = {0, 0, "wav_ex.wav", menu_music_play, &menu, 0, &s2_menu };

// Main menu graphical items
struct _menuitem const menu3 = {"Shutdown", image_data_off, 0, menu_shutdown, &menu, 0, 0};
struct _menuitem const menu2 = {"Settings", image_data_tools, 0, menu_settings, &menu, 0, &menu3};
struct _menuitem const menu1 = {"Recorder", image_data_microphone, 0, menu_recorder, &menu, 0, &menu2};

const struct _menuitem const menu = {"Music", image_data_headphones, 0, 0, 0, &s1_menu, &menu1};

extern const uint8_t* const system8_array[];

void menu_music()
{
	SSD1306_Clear(false); //Wyczyœæ LCD
	SSD1306_SetText(0, 0,"Music menu", system8_array, false);
	SSD1306_CpyDirtyPages();
	delay_ms(2000);
}

void menu_recorder()
{
	SSD1306_Clear(false); //Wyczyœæ LCD
	SSD1306_SetText(0, 0,"Recorder menu", system8_array, false);
	SSD1306_CpyDirtyPages();
	delay_ms(2000);
}

void menu_settings()
{
	SSD1306_Clear(false); //Wyczyœæ LCD
	SSD1306_SetText(0, 0,"Settings menu", system8_array, false);
	SSD1306_CpyDirtyPages();
	delay_ms(2000);
}

void menu_shutdown()
{
	sleep_deep();
}

void menu_music_play()
{
	Controller_SetMenuAction(SELECT_THIS, (char*)Menu_GetCurrentMenuItem()->text);
}


