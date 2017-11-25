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
#include "filebrowser/file_browser.h"

#include <stdlib.h>
#include <string.h>

extern const uint8_t* const system8_array[];

struct _menuitem* menu_items;
struct _menuitem* menu_items_parent;
struct fb_item* dir_items;
uint8_t dir_items_cnt;

//Prototypy funkcji obs�ugi wybranej pozycji menu
void menu_music();
void menu_recorder();
void menu_settings();
void menu_shutdown();
void menu_music_play();
void menu_back();

extern const uint8_t image_data_off[];
extern const uint8_t image_data_headphones[];
extern const uint8_t image_data_microphone[];
extern const uint8_t image_data_tools[];
extern const uint8_t image_data_arrowup[];

// Main graphical menu
struct _menuitem  menu;


// Main menu graphical items
struct _menuitem const menu3 = {"Shutdown", image_data_off, 0, menu_shutdown, &menu, 0, 0};
struct _menuitem const menu2 = {"Settings", image_data_tools, 0, menu_settings, &menu, 0, &menu3};
struct _menuitem const menu1 = {"Recorder", image_data_microphone, 0, menu_recorder, &menu, 0, &menu2};

struct _menuitem  menu = {"Music", image_data_headphones, 0, menu_music, 0, 0, &menu1};

void menu_music()
{
  struct _menuitem* current_menu =  Menu_GetCurrentMenuItem();

  FB_EnterToDir((char*)current_menu->text);

  FB_DeleteItemsList(&dir_items, dir_items_cnt);

  if(!FB_CreateItemsList(NULL, &dir_items, &dir_items_cnt))
    return;

  if(menu_items)
    free(menu_items);

  menu_items = (struct _menuitem*)malloc(sizeof(struct _menuitem) * (dir_items_cnt + 1));
  if(!menu_items)
    return;
  memset(menu_items, 0, sizeof(struct _menuitem) * (dir_items_cnt + 1));

  // Copy dir items names to menu items list
  for(uint8_t idx = 0; idx < dir_items_cnt; idx++)
    {
      if(!dir_items[idx].is_dir)
	menu_items[idx].menuitemfunc = menu_music_play;
      else
	{
	menu_items[idx].menuitemfunc = menu_music;
	menu_items[idx].name = dir_items[idx].name;
	}

      menu_items[idx].next = &menu_items[idx + 1];

      if(idx == 0)
	menu_items[idx].parent = current_menu;
      else
	menu_items[idx].parent = menu_items;

      menu_items[idx].text = dir_items[idx].name;
    }

  menu_items[dir_items_cnt].menuitemfunc = menu_back;
  menu_items[dir_items_cnt].parent = menu_items;
  menu_items[dir_items_cnt].next = NULL;
  menu_items[dir_items_cnt].text = "<back>";

  // Now it has submenu
  current_menu->submenu = menu_items;
}

void menu_recorder()
{
  SSD1306_Clear(false); //Wyczy�� LCD
  SSD1306_SetText(0, 0,"Recorder menu", system8_array, false);
  SSD1306_CpyDirtyPages();
  delay_ms(2000);
}

void menu_settings()
{
  SSD1306_Clear(false); //Wyczy�� LCD
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

void menu_back()
{
  struct _menuitem* current_menu =  Menu_GetCurrentMenuItem();

  if(!strcmp(FB_GetCurrentPath(), "/"))
    {
      current_menu->parent = &menu;
      current_menu->submenu = NULL;
    }
  else
    {
      FB_ExitFromCurrentDir();
      menu_music();
    }

  Menu_Back();
}


