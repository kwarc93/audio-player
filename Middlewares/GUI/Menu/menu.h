#ifndef _MENU_H
#define _MENU_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*menuitemfuncptr)();

struct _menuitem
{
	const char* name;			// Name of item
	const uint8_t* gfx;			// Pointer to item bitmap
	char* text;				// Pointer to item text
	menuitemfuncptr menuitemfunc;		// Callback function (on click)
	const struct _menuitem* parent; 	// Parent item
	const struct _menuitem* submenu;	// Submenu item
	const struct _menuitem* next;		// Next item
};

const struct _menuitem *Menu_GetCurrentMenuItem(void);
void Menu_SetCurrentMenuItem(struct _menuitem* menu_item);

void Menu_Init(struct _menuitem* main_menu);
void Menu_Show();
void Menu_SelectNext();
void Menu_SelectPrev();
void Menu_Click();
void Menu_Back();

#endif
