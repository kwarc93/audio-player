#ifndef _MENU_H
#define _MENU_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*menuitemfuncptr)();

struct _menuitem
{
	const char* name;
	const uint8_t* gfx;
	char* text;
	menuitemfuncptr menuitemfunc;
	const struct _menuitem* parent;
	const struct _menuitem* submenu;
	const struct _menuitem* next;
};

struct _menuitem *Menu_GetCurrentMenuItem(void);
void Menu_SetCurrentMenuItem(struct _menuitem* menu_item);

void Menu_Init(struct _menuitem* main_menu);
void Menu_Show();
void Menu_SelectNext();
void Menu_SelectPrev();
void Menu_Click();
void Menu_Back();

extern struct _menuitem menu;		//Struktura menu

#define Menu_YBorder 4                  //Ile miejsca zostawi� wok� piktogramu - decyduje o ich rozstrzeleniu
#define Menu_XBorder 4

#endif
