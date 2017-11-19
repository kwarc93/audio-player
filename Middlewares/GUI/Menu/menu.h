#ifndef _MENU_H
#define _MENU_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*menuitemfuncptr)();

struct _menuitem
{
	const char* name;
	const uint8_t *gfx;
	const char * const text;
	menuitemfuncptr menuitemfunc;
	const struct _menuitem *parent;
	const struct _menuitem *submenu;
	const struct _menuitem *next;
};

const struct _menuitem *Menu_GetCurrentMenuItem(void);
void Menu_Show();
void Menu_SelectNext();
void Menu_SelectPrev();
void Menu_Click();
void Menu_Back();

extern struct _menuitem const menu;		//Struktura menu

#define Menu_YBorder 4                  //Ile miejsca zostawiæ wokó³ piktogramu - decyduje o ich rozstrzeleniu
#define Menu_XBorder 4

#endif
