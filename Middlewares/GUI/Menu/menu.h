#ifndef _MENU_H
#define _MENU_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*menuitemfuncptr)();

struct _menuitem
{
	const __flash uint8_t *gfx;        //WskaŸnik do opisu bitmapy
	menuitemfuncptr menuitemfunc;
	const __flash struct _menuitem *parent;
	const __flash struct _menuitem *submenu;
	const __flash struct _menuitem *next;
};

void Menu_Show();
void Menu_SelectNext();
void Menu_SelectPrev();
void Menu_Click();
void Menu_Back();

extern struct _menuitem const __flash menu;                 //Struktura menu

#define Menu_YBorder 4                  //Ile miejsca zostawiæ wokó³ piktogramu - decyduje o ich rozstrzeleniu
#define Menu_XBorder 4
#endif
