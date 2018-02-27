/*
 * file_browser.h
 *
 *  Created on: 22.10.2017
 *      Author: Kwarc
 */

#ifndef APP_FILE_BROWSER_H_
#define APP_FILE_BROWSER_H_

struct fb_item
{
	char* name;
	_Bool is_dir;
};

_Bool FB_EnterToDir( char* path );
_Bool FB_ExitFromCurrentDir( void );
const char* FB_GetCurrentPath( void );
const char* FB_GetFileExtension( char *filename );
_Bool FB_GetItemsCount( char* path, uint8_t* items_count );
_Bool FB_CreateItemsList( char* path, struct fb_item** list, uint8_t* list_size );
_Bool FB_DeleteItemsList( struct fb_item** list, uint8_t list_size );

#endif /* APP_FILE_BROWSER_H_ */
