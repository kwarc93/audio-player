#include "misc.h"
#include "Menu/menu.h"
#include "ssd1306/ssd1306.h"
#include "FreeRTOS/FreeRTOS.h"
#include "controller/controller.h"
#include "filebrowser/file_browser.h"

#include <stdlib.h>
#include <string.h>

extern const uint8_t* const system8_array[];

static struct _menuitem* menu_music;
static struct fb_item* dir_items;
static uint8_t dir_items_cnt;

static void menu_music_create();
static void menu_music_back();
static void menu_music_play();
static void menu_music_delete();

static void menu_recorder();
static void menu_settings();
static void menu_shutdown();

extern const uint8_t image_data_off[];
extern const uint8_t image_data_headphones[];
extern const uint8_t image_data_microphone[];
extern const uint8_t image_data_tools[];
extern const uint8_t image_data_arrowup[];

// Current graphical menu
struct _menuitem menu;

// Forward menu declarations
static struct _menuitem const settings_menu0;

// Constant main menu graphical items
static struct _menuitem const menu3 =
{ "Shutdown", image_data_off, 0, menu_shutdown, &menu, 0, 0 };
static struct _menuitem const menu2 =
{ "Settings", image_data_tools, 0, 0, &menu, &settings_menu0, &menu3 };
static struct _menuitem const menu1 =
{ "Recorder", image_data_microphone, 0, menu_recorder, &menu, 0, &menu2 };
static struct _menuitem const menu0 =
{ "Music", image_data_headphones, 0, menu_music_create, 0, 0, &menu1 };

// Constant settings submenu
static struct _menuitem const settings_menu2 =
{ "<back>", 0, "<back>", Menu_Back, &settings_menu0, 0, 0 };
static struct _menuitem const settings_menu1 =
{ "LCD contrast", 0, "LCD contrast", 0, &settings_menu0, 0, &settings_menu2 };
static struct _menuitem const settings_menu0 =
{ "Equalizer", 0, "Equalizer", 0, &menu, 0, &settings_menu1 };

static void menu_music_create()
{
	FB_EnterToDir( (char*) Menu_GetCurrentMenuItem()->text );

	menu_music_delete();

	FB_DeleteItemsList( &dir_items, dir_items_cnt );

	if( !FB_CreateItemsList( NULL, &dir_items, &dir_items_cnt ) )
		return;

	menu_music = (struct _menuitem*) malloc( sizeof(struct _menuitem) * (dir_items_cnt + 1) );
	if( !menu_music )
		return;

	memset( menu_music, 0, sizeof(struct _menuitem) * (dir_items_cnt + 1) );

	// Copy dir items names to menu items list
	for( uint8_t idx = 0; idx < dir_items_cnt; idx++ )
	{
		if( dir_items[idx].is_dir )
		{
			menu_music[idx].menuitemfunc = menu_music_create;
			menu_music[idx].name = dir_items[idx].name;
		}
		else
		{
			menu_music[idx].menuitemfunc = menu_music_play;
			menu_music[idx].name = NULL;
		}

		menu_music[idx].next = &menu_music[idx + 1];
		menu_music[idx].submenu = NULL;
		menu_music[idx].parent = menu_music;
		menu_music[idx].text = dir_items[idx].name;
	}

	menu_music[dir_items_cnt].menuitemfunc = menu_music_back;
	menu_music[dir_items_cnt].parent = menu_music;
	menu_music[dir_items_cnt].submenu = NULL;
	menu_music[dir_items_cnt].next = NULL;
	menu_music[dir_items_cnt].text = "<back>";

	Menu_SetCurrentMenuItem( menu_music );
}

static void menu_music_delete()
{
	if( menu_music )
		free( menu_music );
	menu_music = NULL;
}

static void menu_music_back()
{
	if( !strcmp( FB_GetCurrentPath(), "/" ) )
	{
		menu_music_delete();
		menu = menu0;
		Menu_SetCurrentMenuItem( &menu );
	}
	else
	{
		FB_ExitFromCurrentDir();
		menu_music_create();
		Menu_Back();
	}
}

static void menu_recorder()
{
	SSD1306_Clear( false );
	SSD1306_SetText( 0, 0, "Recorder menu", system8_array, false );
	SSD1306_CpyDirtyPages();
	delay_ms( 2000 );
}

static void menu_settings()
{
	SSD1306_Clear( false );
	SSD1306_SetText( 0, 0, "Settings menu", system8_array, false );
	SSD1306_CpyDirtyPages();
	delay_ms( 2000 );
}

static void menu_shutdown()
{
	sleep_deep();
}

static void menu_music_play()
{
	Controller_SetMenuAction( SELECT_THIS, (char*) Menu_GetCurrentMenuItem()->text );
}

void MenuManager_CreateMenu( void )
{
	menu = menu0;
	Menu_Init( &menu );
}

