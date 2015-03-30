/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: contains function for custom character creation and print 
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.h"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))
          
/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTA		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTB	// port connected to pins 4 and 6 of LCD disp.
#define RS 5			// pin number of uC connected to pin 4 of LCD disp.
#define E 7			// pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
   LCD_WriteCommand(0x01);
}

void LCD_init(void) {

    //wait for 100 ms.
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);						 
}

void LCD_WriteCommand (unsigned char Command) {
   CLR_BIT(CONTROL_BUS,RS);
   DATA_BUS = Command;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
   SET_BIT(CONTROL_BUS,RS);
   DATA_BUS = Data;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(1);
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
   LCD_ClearScreen();
   unsigned char c = column;
   while(*string) {
      LCD_Cursor(c++);
      LCD_WriteData(*string++);
   }
}

void LCD_Cursor(unsigned char column) {
   if ( column < 17 ) {
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);
   }
}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
    int i,j;
    for(i=0;i<miliSec;i++)
    for(j=0;j<775;j++)
  {
   asm("nop");
  }
} 

void LCD_Make_Plane()
{
	LCD_WriteCommand(0x40);
	LCD_WriteData(0X00);
	LCD_WriteData(0X00);
	LCD_WriteData(0X04);
	LCD_WriteData(0X16);
	LCD_WriteData(0X0F);
	LCD_WriteData(0X16);
	LCD_WriteData(0X04);
	LCD_WriteData(0X00);
	LCD_WriteCommand(0x02);
}

void LCD_Custom(unsigned char col)
{
	LCD_Make_Plane();
	unsigned char airplane[] = {0x00,0x00,0x04,0x16,0x0F,0x16,0x04,0x00};
	unsigned char index;
	LCD_WriteCommand(0x80 + col - 1); //display on first row at specified col
	for(index = 0; index < 8; ++index)
	{
		LCD_WriteData(airplane[index]);
	}
	/*unsigned char* airplane_loc = 0x40;
	while(*airplane_loc)
	{
		LCD_WriteCommand(0x80 + col - 1);
		LCD_WriteData(*airplane_loc++);
	}*/
}