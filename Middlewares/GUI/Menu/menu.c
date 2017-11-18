#include "menu.h"
#include "ST7565/ST7565.h"
#include <stddef.h>
#include <string.h>
#include <avr\pgmspace.h>

static const __flash struct _menuitem *currMenuPtr=&menu;   //Bie¿¹ca pozycja menu
static int8_t menuindex;                                    //Numer aktualnie wybrane pozycji menu
static int8_t menufirstpos;                                 //Numer pozycji menu wyœwietlanej w górnym rzêdzie

extern const __flash uint8_t image_data_directory[];        //Symbol u¿ywany do zaznaczenia pozycji z podmenu

uint8_t Menu_GetMenuItemsNo()            //Policz ile dane menu ma pozycji
{
	const __flash struct _menuitem *tmpmenuitem=currMenuPtr;
	uint8_t index=0;

	while(tmpmenuitem)
	{
		tmpmenuitem=tmpmenuitem->next;
		index++;
	}
	return index;
}

const __flash struct _menuitem *Menu_GetMenuItem(uint8_t index)
{
	const __flash struct _menuitem *tmpmenuitem=currMenuPtr;

	while((tmpmenuitem) && (index>0))
	{
	 tmpmenuitem=tmpmenuitem->next;
	 index--;
	}
	return tmpmenuitem;
}

uint8_t Menu_GetMenuRows()
{
	return ST7565R_HEIGHT / (menu.gfx[1] + Menu_YBorder); //Wysokoœæ bitmapy
}

uint8_t Menu_GetMenuCols()
{
	return ST7565R_WIDTH / (menu.gfx[0] + Menu_XBorder);  //Szerokoœæ bitmapy
}

void Menu_Show()
{
	const __flash struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menufirstpos);
	uint8_t bmp_height=menu.gfx[1];
	uint8_t bmp_width=menu.gfx[0];
	uint8_t menuitemsno=Menu_GetMenuItemsNo();
	uint8_t currmenupos=0;

	st7565r_Clear(false); //Wyczyœæ LCD

	uint8_t xspacing=(ST7565R_WIDTH - Menu_GetMenuCols() * (bmp_width + Menu_XBorder)) / (Menu_GetMenuCols() + 1);
	uint8_t yspacing=(ST7565R_HEIGHT - Menu_GetMenuRows() * (bmp_height + Menu_YBorder)) / (Menu_GetMenuRows() + 1);

	for(uint8_t oy=0; oy < Menu_GetMenuRows(); oy++)
	{
		for(uint8_t ox=0; ox < Menu_GetMenuCols(); ox++)
		{
			if(tmpmenuitem == NULL) break;           //Nie ma wiêce pozycji menu - koñczymy pêtlê
			uint8_t tx=ox * (bmp_width + Menu_XBorder) + (ox + 1) * xspacing + Menu_XBorder/2;
			uint8_t ty=oy * (bmp_height + Menu_YBorder) + (oy + 1) * yspacing + Menu_YBorder/2;
			if(menuindex == ((menufirstpos + currmenupos) % menuitemsno))              //Czy podœwietliæ dan¹ pozycje menu?
			{
				st7565r_MoveTo(tx-2, ty-2); st7565r_LineTo(tx+bmp_width+2, ty-2, true); //_
				st7565r_LineTo(tx+bmp_width+2, ty+bmp_height+2, true);                  // |
				st7565r_LineTo(tx-2, ty+bmp_height+2, true);                            //_
				st7565r_LineTo(tx-2, ty-2, true);                                       //|
			}
			st7565r_draw_bitmap_mono(tx, ty, tmpmenuitem->gfx, false);     //Wyœwietl pozycjê menu
			if(tmpmenuitem->submenu)
		        st7565r_draw_bitmap_mono(tx+bmp_width-image_data_directory[0], ty+bmp_height-image_data_directory[1], image_data_directory, false); //Zaznacz, ¿e mamy submenu

			tmpmenuitem=tmpmenuitem->next;   //Kolejna pozycja menu
			currmenupos++;
		}
	}
	st7565r_CpyDirtyPages();   //Odœwie¿ widok na LCD
}

void Menu_SelectNext()
{
	menuindex=(menuindex + 1) % Menu_GetMenuItemsNo();     //Liczymy wszysko modulo liczba pozycji w menu
	menufirstpos=Menu_GetMenuRows() * Menu_GetMenuCols() * (menuindex / (Menu_GetMenuRows() * Menu_GetMenuCols()));
	Menu_Show();      //Wyœwietl menu
}

void Menu_SelectPrev()
{
	if(menuindex > 0) menuindex--; else menuindex=Menu_GetMenuItemsNo()-1;
	menufirstpos=Menu_GetMenuRows() * Menu_GetMenuCols() * (menuindex / (Menu_GetMenuRows() * Menu_GetMenuCols()));
	Menu_Show();     //Wyœwietl menu
}

void Menu_Back()
{
	menufirstpos=0;
	menuindex=0;
	currMenuPtr=currMenuPtr->parent;
}

void Menu_Click()
{
	const __flash struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menuindex);
	const __flash struct _menuitem *submenu=tmpmenuitem->submenu;

    menuitemfuncptr mfptr=tmpmenuitem->menuitemfunc;
	if(mfptr) (*mfptr)();
	if(submenu)
	 {
	  currMenuPtr=submenu;
	  menuindex=0;
	  menufirstpos=0;
     }
    Menu_Show();
}
