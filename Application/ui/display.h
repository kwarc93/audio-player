/*
 * display.h
 *
 *  Created on: 24.09.2017
 *      Author: Kwarc
 */

#ifndef UI_DISPLAY_H_
#define UI_DISPLAY_H_

#define LCD_STACK_SIZE 512

void Display_StartTasks(unsigned portBASE_TYPE uxPriority);
void Display_SendText(char* txt);
void Display_SendBarLevel(uint8_t lvl);


#endif /* UI_DISPLAY_H_ */
