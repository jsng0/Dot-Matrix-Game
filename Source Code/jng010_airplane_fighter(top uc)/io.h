/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: header for custom character
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#ifndef __io_h__
#define __io_h__

void LCD_init();
void LCD_ClearScreen(void);
void LCD_WriteCommand (unsigned char Command);
void LCD_Cursor (unsigned char column);
void LCD_DisplayString(unsigned char column ,const unsigned char *string);
void delay_ms(int miliSec);

void LCD_Custom(unsigned char location);
void LCD_Make_Plane();
#endif

