/*
 * file_browser.c
 *
 *  Created on: 22.10.2017
 *      Author: Kwarc
 */



// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "FatFs/ff.h"
#include "file_browser.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
const char *FB_GetFileExtension(char *filename)
{
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    dot++;
    // Convert to lowercase
    for(uint8_t i = 0; dot[i]; i++)
    {
    	dot[i] = tolower(dot[i]);
    }
    return dot;
}

_Bool FB_GetItemsCount(char* path, uint8_t* items_count)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;

    *items_count = 0;
    res = f_opendir(&dir, path);                       	   /* Open the directory */
    if (res == FR_OK)
    {
        for (;;)
        {
            res = f_readdir(&dir, &fno);                    /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;   /* Break on error or end of dir */
            (*items_count)++;                     			/* It is a file. */
        }

        f_closedir(&dir);
    }

    return (res == FR_OK) ? true : false;
}

_Bool FB_CreateItemsList(char* path, struct fb_item** list, uint8_t* list_size)
{
	FRESULT res;
    DIR dir;
    FILINFO fno;
	uint8_t items_count, idx;

	if(FB_GetItemsCount(path, &items_count))
	{
		*list = (struct fb_item*)malloc(sizeof(struct fb_item) * items_count);
		if(!*list)
			return false;
		memset(*list, 0, sizeof(struct fb_item) * items_count);
	}
	else
		return false;

    res = f_opendir(&dir, path);                       	   /* Open the directory */
    if (res == FR_OK)
    {
    	idx = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno);                    /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;   /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR)	                    /* It is a directory */
            {
            	(*list)[idx].is_dir = true;
            }
            else				                     		/* It is a file. */
            {
            	(*list)[idx].is_dir = false;
            }

            (*list)[idx].name = (char*)malloc(strlen(fno.fname) * sizeof(char) + 1);
            if(!(*list)[idx].name) return false;
            strcpy((*list)[idx].name, fno.fname);
            idx++;
        }

        f_closedir(&dir);
        *list_size = items_count;
    }

    return (res == FR_OK) ? true : false;

}

_Bool FB_DeleteItemsList(struct fb_item** list, uint8_t list_size)
{
	if(!*list)
		return false;

	for(uint8_t idx = 0; idx < list_size; idx++)
	{
		if((*list)[idx].name) free((*list)[idx].name);
	}

	free(*list);

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
