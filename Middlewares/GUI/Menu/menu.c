#include "menu.h"
#include "ssd1306/ssd1306.h"
#include <stddef.h>
#include <string.h>

extern const uint8_t image_data_directory[];        		//Symbol u¿ywany do zaznaczenia pozycji z podmenu
extern const uint8_t* const system8_array[];

static const struct _menuitem *currMenuPtr=&menu;   		//Bie¿¹ca pozycja menu
static int8_t menuindex;                                    //Numer aktualnie wybrane pozycji menu
static int8_t menufirstpos;                                 //Numer pozycji menu wyœwietlanej w górnym rzêdzie
static const uint8_t* const* menu_font = system8_array;

uint8_t Menu_GetMenuItemsNo()            //Policz ile dane menu ma pozycji
{
	const struct _menuitem *tmpmenuitem=currMenuPtr;
	uint8_t index=0;

	while(tmpmenuitem)
	{
		tmpmenuitem=tmpmenuitem->next;
		index++;
	}
	return index;
}

const struct _menuitem *Menu_GetMenuItem(uint8_t index)
{
	const struct _menuitem *tmpmenuitem=currMenuPtr;

	while((tmpmenuitem) && (index>0))
	{
	 tmpmenuitem=tmpmenuitem->next;
	 index--;
	}
	return tmpmenuitem;
}

uint8_t Menu_GetMenuRows()
{
	uint8_t rows = 0;

	if(currMenuPtr->gfx)
		rows =  LCD_HEIGHT / (currMenuPtr->gfx[1] + Menu_YBorder); //Wysokoœæ bitmapy
	else if(currMenuPtr->text)
		rows =  LCD_HEIGHT/(uintptr_t)menu_font[0];			// Wysokoœc cznionki

	return rows;
}

uint8_t Menu_GetMenuCols()
{
	return LCD_WIDTH / (menu.gfx[0] + Menu_XBorder);  //Szerokoœæ bitmapy
}

void Menu_ShowGfx()
{
	const struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menufirstpos);
	uint8_t bmp_height=menu.gfx[1];
	uint8_t bmp_width=menu.gfx[0];
	uint8_t menuitemsno=Menu_GetMenuItemsNo();
	uint8_t currmenupos=0;

	SSD1306_Clear(false); //Wyczyœæ LCD

	uint8_t xspacing=(LCD_WIDTH - Menu_GetMenuCols() * (bmp_width + Menu_XBorder)) / (Menu_GetMenuCols() + 1);
	uint8_t yspacing=(LCD_HEIGHT - Menu_GetMenuRows() * (bmp_height + Menu_YBorder)) / (Menu_GetMenuRows() + 1);

	for(uint8_t oy=0; oy < Menu_GetMenuRows(); oy++)
	{
		for(uint8_t ox=0; ox < Menu_GetMenuCols(); ox++)
		{
			if(tmpmenuitem == NULL) break;           //Nie ma wiêce pozycji menu - koñczymy pêtlê
			uint8_t tx=ox * (bmp_width + Menu_XBorder) + (ox + 1) * xspacing + Menu_XBorder/2;
			uint8_t ty=oy * (bmp_height + Menu_YBorder) + (oy + 1) * yspacing + Menu_YBorder/2;
			if(menuindex == ((menufirstpos + currmenupos) % menuitemsno))              //Czy podœwietliæ dan¹ pozycje menu?
			{
				SSD1306_DrawRect(tx-2, ty-2, bmp_width+3, bmp_height+3, false);
				SSD1306_SetText(LCD_WIDTH/2 - (strlen(Menu_GetMenuItem(currmenupos)->name)*6)/2, 54,
						Menu_GetMenuItem(currmenupos)->name, system8_array,false);
			}
			SSD1306_DrawBitmap(tx, ty, tmpmenuitem->gfx, false);     //Wyœwietl pozycjê menu
			if(tmpmenuitem->submenu)
				SSD1306_DrawBitmap(tx+bmp_width-image_data_directory[0], ty+bmp_height-image_data_directory[1], image_data_directory, false); //Zaznacz, ¿e mamy submenu

			tmpmenuitem=tmpmenuitem->next;   //Kolejna pozycja menu
			currmenupos++;
		}
	}
	SSD1306_CpyDirtyPages();   //Odœwie¿ widok na LCD
}

void Menu_ShowTxt()
{
	const struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menufirstpos);
	uint8_t font_height=(uintptr_t)menu_font[0];
	uint8_t menuitemsno=Menu_GetMenuItemsNo();

	SSD1306_Clear(false); //Wyczyœæ LCD

	for(uint8_t i=0; i < Menu_GetMenuRows(); i++)
	{
		_Bool invert=menuindex == ((menufirstpos + i) % menuitemsno);              //Czy podœwietliæ dan¹ pozycje menu
		SSD1306_SetText(0, i * font_height, tmpmenuitem->text, menu_font, invert); //Wyœwietl pozycjê menu
		if(tmpmenuitem->submenu)
		  SSD1306_SetText(LCD_WIDTH - 3 * font_height, i * font_height, ">>>", menu_font, invert); //Zaznacz, ¿e mamy submenu
		tmpmenuitem=tmpmenuitem->next;
		if(tmpmenuitem == NULL)  //Koniec listy
		{
			if(Menu_GetMenuItemsNo() > Menu_GetMenuRows()) tmpmenuitem=currMenuPtr; //Zawijamy listê jeœli jest d³u¿sza ni¿ liczba wyœwietlanych pozycji
			   else break;   //lub koñczymy, ¿eby unikn¹æ powtarzania elementów
		}
	}
	SSD1306_CpyDirtyPages();   //Odœwie¿ widok na LCD
}

void Menu_Show()
{
	if(currMenuPtr->gfx)
		Menu_ShowGfx();
	else if(currMenuPtr->text)
		Menu_ShowTxt();
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
	const struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menuindex);
	const struct _menuitem *submenu=tmpmenuitem->submenu;

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
