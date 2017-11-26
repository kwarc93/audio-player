#include "menu.h"
#include "ssd1306/ssd1306.h"
#include <stddef.h>
#include <string.h>

#define GFX_Y_BORDER 4
#define GFX_X_BORDER 4

extern const uint8_t image_data_directory[];
extern const uint8_t* const system8_array[];

static const struct _menuitem *currMenuPtr;   		    // Current menu position
static int8_t menuindex;                                    // Number of actual menu position
static int8_t menufirstpos;                                 // Number of first menu position
static const uint8_t* const* menu_font = system8_array;

static uint8_t Menu_GetMenuItemsNo()
{
  const struct _menuitem *tmpmenuitem=currMenuPtr;
  uint8_t index=0;

  // Get number of current menu items
  while(tmpmenuitem)
    {
      tmpmenuitem=tmpmenuitem->next;
      index++;
    }
  return index;
}

static const struct _menuitem *Menu_GetMenuItem(uint8_t index)
{
  const struct _menuitem *tmpmenuitem=currMenuPtr;

  while((tmpmenuitem) && (index>0))
    {
      tmpmenuitem=tmpmenuitem->next;
      index--;
    }
  return tmpmenuitem;
}

static uint8_t Menu_GetMenuRows()
{
  uint8_t rows = 0;

  if(currMenuPtr->gfx)
    rows =  LCD_HEIGHT / (currMenuPtr->gfx[1] + GFX_Y_BORDER); // Bitmap height
  else if(currMenuPtr->text)
    rows =  LCD_HEIGHT / (uintptr_t)menu_font[0];	// Font height

  return rows;
}

static uint8_t Menu_GetMenuCols()
{
  return LCD_WIDTH / (currMenuPtr->gfx[0] + GFX_X_BORDER);  //Bitmap width
}

static void Menu_ShowGfx()
{
  const struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menufirstpos);
  uint8_t bmp_height=tmpmenuitem->gfx[1];
  uint8_t bmp_width=tmpmenuitem->gfx[0];
  uint8_t menuitemsno=Menu_GetMenuItemsNo();
  uint8_t currmenupos=0;

  SSD1306_Clear(false);

  uint8_t xspacing=(LCD_WIDTH - Menu_GetMenuCols() * (bmp_width + GFX_X_BORDER)) / (Menu_GetMenuCols() + 1);
  uint8_t yspacing=(LCD_HEIGHT - Menu_GetMenuRows() * (bmp_height + GFX_Y_BORDER)) / (Menu_GetMenuRows() + 1);

  for(uint8_t oy=0; oy < Menu_GetMenuRows(); oy++)
    {
      for(uint8_t ox=0; ox < Menu_GetMenuCols(); ox++)
	{
	  if(tmpmenuitem == NULL) break; // If there is no more items - break

	  uint8_t tx=ox * (bmp_width + GFX_X_BORDER) + (ox + 1) * xspacing + GFX_X_BORDER/2;
	  uint8_t ty=oy * (bmp_height + GFX_Y_BORDER) + (oy + 1) * yspacing + GFX_Y_BORDER/2;

	  if(menuindex == ((menufirstpos + currmenupos) % menuitemsno))
	    {
	      // Highlight current position
	      SSD1306_DrawRect(tx-2, ty-2, bmp_width+3, bmp_height+3, false);
	      SSD1306_SetText(LCD_WIDTH/2 - (strlen(Menu_GetMenuItem(currmenupos)->name)*6)/2, 54,
			      Menu_GetMenuItem(currmenupos)->name, system8_array,false);
	    }

	  // Show menu position
	  SSD1306_DrawBitmap(tx, ty, tmpmenuitem->gfx, false);

	  // Indicates that this item has submenu
	  //if(tmpmenuitem->submenu)
	  //	SSD1306_DrawBitmap(tx+bmp_width-image_data_directory[0], ty+bmp_height-image_data_directory[1], image_data_directory, false);

	  tmpmenuitem=tmpmenuitem->next;   // Next menu position
	  currmenupos++;
	}
    }
  SSD1306_CpyDirtyPages(); // Refresh LCD
}

static void Menu_ShowTxt()
{
  const struct _menuitem *tmpmenuitem=Menu_GetMenuItem(menufirstpos);
  uint8_t font_height=(uintptr_t)menu_font[0];
  uint8_t menuitemsno=Menu_GetMenuItemsNo();

  SSD1306_Clear(false);

  for(uint8_t i=0; i < Menu_GetMenuRows(); i++)
    {
      _Bool invert = menuindex == ((menufirstpos + i) % menuitemsno); // Highlight current position?
      SSD1306_SetText(0, i * font_height, tmpmenuitem->text, menu_font, invert);

      // Indicates that this item has submenu
      if(tmpmenuitem->submenu)
	SSD1306_SetText(LCD_WIDTH - 3 * font_height, i * font_height, ">>>", menu_font, invert);

      tmpmenuitem=tmpmenuitem->next;
      if(tmpmenuitem == NULL)
	{
	  //End of list
	  if(Menu_GetMenuItemsNo() > Menu_GetMenuRows()) tmpmenuitem=currMenuPtr; // Roll up list
	  else break; // else end to aviod items repetition
	}
    }
  SSD1306_CpyDirtyPages(); // Refresh LCD
}

void Menu_Init(struct _menuitem* main_menu)
{
  currMenuPtr = main_menu;
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
  if(currMenuPtr->gfx)
    {
      menuindex=(menuindex + 1) % Menu_GetMenuItemsNo();     //Liczymy wszysko modulo liczba pozycji w menu
      menufirstpos=Menu_GetMenuRows() * Menu_GetMenuCols() * (menuindex / (Menu_GetMenuRows() * Menu_GetMenuCols()));
    }
  else if(currMenuPtr->text)
    {
      uint8_t no=Menu_GetMenuItemsNo();
      menuindex++;
      if(no > Menu_GetMenuRows())        //Czy liczba pozycji menu jest wi�ksza ni� liczba wy�wietlanych pozycji?
	{
	  int8_t dist;               //Odleg�o�� pomi�dzy pierwsz� wy�wietlan� pozycj�, a pozycj� pod�wietlon�
	  if(menuindex < menufirstpos) dist=no - menufirstpos + menuindex; //Jest zale�na od tego, kt�a z pozycji jest wi�ksza
	  else dist=menuindex-menufirstpos;
	  if(dist >= Menu_GetMenuRows()) menufirstpos++;  //Koniec ekranu, trzeba przewija�
	}
      menuindex%=no;     //Liczymy wszysko modulo liczba pozycji w menu
      menufirstpos%=no;
    }
  Menu_Show();      //Wy�wietl menu
}

void Menu_SelectPrev()
{
  if(currMenuPtr->gfx)
    {
      if(menuindex > 0) menuindex--; else menuindex=Menu_GetMenuItemsNo()-1;
      menufirstpos=Menu_GetMenuRows() * Menu_GetMenuCols() * (menuindex / (Menu_GetMenuRows() * Menu_GetMenuCols()));
    }
  else if(currMenuPtr->text)
    {
      if(menuindex > 0)
	{
	  if(menuindex == menufirstpos) menufirstpos--;
	  menuindex--;               //Poprzedni element
	}
      else
	{
	  if(menufirstpos == 0)
	    {
	      menuindex=Menu_GetMenuItemsNo()-1;  //Zawijamy menu
	      if(Menu_GetMenuItemsNo()>Menu_GetMenuRows()) menufirstpos=menuindex;  //Je�li mamy mniej pozycji menu ni� linii na LCD to nie zmieniamy numeru pierwszej pozycji menu
	    } else menuindex=Menu_GetMenuItemsNo()-1;
	}
    }
  Menu_Show();     //Wy�wietl menu
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

struct _menuitem *Menu_GetCurrentMenuItem(void)
{
  return (struct _menuitem *)Menu_GetMenuItem(menuindex);
}

void Menu_SetCurrentMenuItem(struct _menuitem* menu_item)
{
  currMenuPtr = menu_item;
  menuindex=0;
  menufirstpos=0;
}
