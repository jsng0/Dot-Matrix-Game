/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: control lcd screen
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#ifndef MENU_H_
#define MENU_H_	

#include <avr/io.h>
#include "keypad.h"
#include "io.c"
#include "bit.h"
#include "usart.h"

enum MENU_STATES { MENU_WAIT, MENU_START}; //add MENU_LOSE later MENU_START

unsigned char score = 0; //move to global variables file later

int start = 0;
int reset = 0;
int left = 0;
int right = 0;
int change_menu = 1;
int change_score = 1;
const unsigned char welcome_msg[] =    "   Press (A)    ";
const unsigned char current_score[] = "High Score: 0    ";
unsigned char package = 0x00;    //holds all the masked bits
unsigned char game_begin = 0x01; //sent via USART to uc2 to end welcome animation
unsigned char send_left = 0x02;
unsigned char send_right = 0x04;
int MENU_Tick(int state)
{
	start = GetKeypadKey() == 'A'; //keypad controls 
	reset = GetKeypadKey() == 'B';
	left = GetKeypadKey() == '3';
	right = GetKeypadKey() == '1';
	
	switch(state)
	{		
		case MENU_WAIT:
			if(!start)
			{
				state = MENU_WAIT;
				if(change_menu)
				{
					LCD_DisplayString(1,welcome_msg);
					LCD_Custom(16);
					change_score = 1;
				}
				change_menu = 0;
			}
			else// if(start)
			{
				state = MENU_START;
			}
			break;
		case MENU_START:
			if(!reset)
			{
				state = MENU_START;
				if(change_score) LCD_DisplayString(1,current_score);
				LCD_Custom(16);
				change_score = 0; //have a way to notify when to update score
			}
			else if(reset)
			{
				state = MENU_WAIT;
				change_menu = 1;	
				
			}		
			break;
		default:
			break;
	}
	
	switch(state)
	{
		case MENU_WAIT:
			//package &= 0xFE; //clear the first bit
			//USART_Send(package);
			LCD_Cursor(0x00);
			break;
		case MENU_START:
			//package |= game_begin; //set the first bit
			//USART_Send(package);
			LCD_Cursor(0x00);
			
			if(left) 
			{
				package |= send_left;
				USART_Send(package); //raise the second bit
				package &= 0xFD; //raise the second bit
			}
			else if(right)
			{
				package |= send_right;
				USART_Send(package); //raise the third bit
				package &= 0xFB; //reset the third bit	
			}
				
			break;
		default:
			break;
	}
	return state;
}

#endif