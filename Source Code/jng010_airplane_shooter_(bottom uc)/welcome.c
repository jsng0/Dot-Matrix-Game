/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: controls led matrix
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#ifndef WELCOME_H_
#define WELCOME_H_

#include <avr/io.h>
#include "bit.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h> //for rand

//srand(time(NULL));

/////////////////////////////////
unsigned char drop_down_rows[] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE}; //toggle hex values for resistance
unsigned char drop_down_cols[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; //toggle hex values for voltage
unsigned char strafe_cols[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}; //axis of movement for airplane								 

unsigned char outputVolt = 0x00; //PORTA
unsigned char outputRes = 0x00; //PORTC
unsigned char row_index = 0;
unsigned char col_index = 0;

unsigned char first_sm_done = 0x00;

//variables for airplane movement
unsigned char airplane_row = 7;
unsigned char airplane_col = 0;

//game control variables via USART
unsigned char get_package = 0x00; //flag carrier from first uc
unsigned char game_start = 0x01;	 //sent via USART from keypadkey == 'A'
unsigned char strafe_left = 0x02;    //sent via USART from keypadkey == '3'
unsigned char strafe_right = 0x04;   //sent via USART from keypadkey == '1'
unsigned char got_hit = 0x08;

unsigned char control_col = 7; //used to interrogate each column
unsigned char control_row = 0;

//used for led matrix control
unsigned char per_col = 0x00;
unsigned char hex_data = 0x00;
unsigned char drop_col = 0x00; //random col generated from rand()
unsigned char drop_row = 0x00;
unsigned short rand_limiter = 0; //pseudo clock for generate random
unsigned short drop_limiter = 0; //pseudo clock for bullet drop


/////////////////////////////////////
                            //col 0 1 2 3 4 5 6 7
unsigned char airplane[8][8] = { {0,0,0,0,0,0,0,0}, //row 0
								 {0,0,0,0,0,0,0,0}, //row 1
								 {0,0,0,0,0,0,0,0}, //row 2
								 {0,0,0,0,0,0,0,0}, //row 3
							     {0,0,0,0,0,0,0,0}, //row 4
								 {0,0,0,0,0,0,0,0}, //row 5
								 {0,0,0,0,0,0,0,0}, //row 6
								 {5,0,0,0,0,0,0,0} }; //row 7

enum PRINT_AIRPLANE_STATES {PRINT_AIRPLANE_INIT, PRINT_AIRPLANE_OUT, PRINT_AIRPLANE_NEXT};

int PRINT_AIRPLANE_Tick(int state)
{
	switch(state)
	{
		case PRINT_AIRPLANE_INIT:
			drop_col = rand() % 8; //pick a pseudo random column
			airplane[0][drop_col] = 0x07; //put in bullets
			airplane[airplane_row][airplane_col] = 0x05; //put in airplane
			//PORTA = 0x00; PORTC = 0x00;
			
			state = PRINT_AIRPLANE_NEXT;
			break;
		case PRINT_AIRPLANE_NEXT:
			state = PRINT_AIRPLANE_OUT;
			break;
		case PRINT_AIRPLANE_OUT:
			++rand_limiter;
			++drop_limiter;
			state = PRINT_AIRPLANE_NEXT;
			break;
	}
	switch(state)
	{
		case PRINT_AIRPLANE_INIT:
			break;
		case PRINT_AIRPLANE_NEXT:
			
			for(per_col = 0; per_col < 8; ++per_col) //generate row hex
			{
				if(airplane[control_row][per_col] == 0x05)
				{
					hex_data = SetBit(hex_data,per_col,1); //make 1
				}
				else if(airplane[control_row][per_col] == 0x00)
				{
					hex_data = SetBit(hex_data,per_col,0); //make 0
				}
				else if(airplane[control_row][per_col] == 0x07)
				{
					hex_data = SetBit(hex_data,per_col,1); //make 1
				}
			}
			
			break;
		case PRINT_AIRPLANE_OUT:
			//DDRA = hex_data;
			PORTA = hex_data;
			//DDRC = ~drop_down_rows[drop_row];
			PORTC = drop_down_rows[control_row];
			hex_data = 0x00;
			
			/*if(airplane[drop_row][airplane_col] == 0x07) //drop down the bullet
			{
				if(drop_limiter >= 50){
				airplane[drop_row][airplane_col] = 0x00;
				++drop_row; //move the row down by one
				airplane[drop_row][airplane_col] = 0x07;
				--drop_row; //move back to original row
				drop_limiter = 0;
				}
			}*/
			
			for(unsigned char r = 0; r < 8; ++r)
			{
				for(unsigned char c = 0; c < 8; ++c)
				{
					if(airplane[drop_row][airplane_col] == 0x07) //drop down the bullet
					{
						if(drop_limiter >= 50){
						airplane[drop_row][airplane_col] = 0x00;
						++drop_row; //move the row down by one				
						airplane[drop_row][airplane_col] = 0x07;
						--drop_row; //move back to original row
						drop_limiter = 0;
						}
					}
				}
			}
			
			
			if(drop_row == 7)
			{
				drop_row = 0;			
				if(rand_limiter >= 500) //generate new col for bullet
				{
					drop_col = rand() % 8; //get a new column
					airplane[0][drop_col] = 0x07; //put in bullets
					rand_limiter = 0;
				}
			}
			else
			{
				++drop_row;
			}
			
			if (control_row == 7) //if reach max row
			{
				control_row = 0;		
			}
			else
			{
				++control_row;
			}
			break;
		default:
			break;
	}
	return state;
}

enum PLANE_STRAFE_STATES {PLANE_STRAFE_INIT, PLANE_STRAFE_INPUT};

int PLANE_STRAFE_Tick(int state) 
{
	get_package = USART_Receive();
	
	switch(state) 
	{
		case PLANE_STRAFE_INIT:
			state = PLANE_STRAFE_INPUT;
			break;
		case PLANE_STRAFE_INPUT:
			PORTA = hex_data;
			//DDRC = 0xFF;
			PORTC = 0xFE; //print on the last row
			state = PLANE_STRAFE_INPUT;
			break;
		default:
			state = PRINT_AIRPLANE_INIT;
			break;
	}
	
	switch(state)
	{
		case PLANE_STRAFE_INIT:
			break;
		case PLANE_STRAFE_INPUT:
			/*for(unsigned char r = 0; r < 8; ++r)
			{
				for(unsigned char c = 0; c < 8; ++c)
				{
					if(airplane[drop_row][airplane_col] == 0x07) //drop down the bullet
					{
						if(drop_limiter >= 10){
						airplane[drop_row][airplane_col] = 0x00;
						++drop_row; //move the row down by one				
						airplane[drop_row][airplane_col] = 0x07;
						--drop_row; //move back to original row
						drop_limiter = 0;
						}
					}
				}
			}*/
			/*if(airplane[drop_row][airplane_col] == 0x07) //drop down the bullet
			{
				if(drop_limiter >= 1){
				airplane[drop_row][airplane_col] = 0x00;
				++drop_row; //move the row down by one				
				airplane[drop_row][airplane_col] = 0x07;
				--drop_row; //move back to original row
				}
			}*/
			switch(get_package)
			{
				case 0x04: //move left
					if(airplane_col > 0)
					{
						airplane[airplane_row][airplane_col] = 0x00;
						--airplane_col;
						airplane[airplane_row][airplane_col] = 0x05; //move the plane
						
						if(airplane[airplane_row][airplane_col] == 0x07) //if hit an object
						{
							//hit a bullet, send lose game flag 
							//get_package |= got_hit;
						}
					}
					break;
				case 0x02: //move right
					if(airplane_col < 7)
					{
						airplane[airplane_row][airplane_col] = 0x00;
						++airplane_col;
						airplane[airplane_row][airplane_col] = 0x05; //move the plane
						
						if(airplane[airplane_row][airplane_col] == 0x07) //if hit an object
						{
							//hit a bullet, send lose game flag
							//get_package |= got_hit;
						}
					}
					break;
				default:
					//airplane[airplane_row][airplane_col] = 0x05;
					break;
			}
			break;
	}
	return state;
}

//wasted time and broken dreams
/*
enum WELCOME_STATES { WELCOME_INIT, WELCOME_ROWS_DOWN, WELCOME_ROWS_UP, WELCOME_END}; 

int WELCOME_Tick(int state) //welcome splash screen
{	
	get_package = USART_Receive();
	
	switch(state)
	{
		case WELCOME_INIT:
			state = WELCOME_ROWS_DOWN;
			
			break;
		case WELCOME_ROWS_DOWN:
			//state = (game_start == 0x01) ? WELCOME_END : WELCOME_ROWS_DOWN;
			if(GetBit(get_package,0) == 1)
			{
				state = WELCOME_END;
				PORTA = 0x00; PORTC = 0x00;
				//first_sm_done = 0x01;
			}
			else if(GetBit(get_package,0) == 0)
			{
				state = WELCOME_ROWS_DOWN;
			}
			break;
		case WELCOME_ROWS_UP:
			//state = (game_start == 0x01) ? WELCOME_END : WELCOME_ROWS_UP;
			if(GetBit(get_package,0) == 1)
			{
				state = WELCOME_END;
				PORTA = 0x00; PORTC = 0x00;
				//first_sm_done = 0x01;
			}
			else if(GetBit(get_package,0) == 0)
			{
				state = WELCOME_ROWS_UP;
			}
			break;
		case WELCOME_END:
			//state = (game_start == 0x01) ? WELCOME_END : WELCOME_INIT; //death line here because 0x01,0x03, or 0x04
			if(GetBit(get_package,0) == 1)
			{
				state = WELCOME_END;
				first_sm_done = 0x01;
			}
			else if(GetBit(get_package,0) == 0)
			{
				state = WELCOME_INIT;
				PORTC = 0xFF; PORTA = 0xFF;
			}
			break;
		default:
			break;
	}
	
	switch(state)
	{
		case WELCOME_INIT:
			
			break;
		case WELCOME_ROWS_DOWN:
			if(row_index < 8)
			{
				outputRes = drop_down_rows[row_index];
				++row_index;
			}
			else if(row_index == 8)
			{
				--row_index;
				state = WELCOME_ROWS_UP;
			}
			PORTC = outputRes;
			break;
		case WELCOME_ROWS_UP:
			if(row_index > 0)
			{
				outputRes = drop_down_rows[row_index];
				--row_index;
			}
			else if (row_index == 0)
			{
				state = WELCOME_ROWS_DOWN;
			}
			PORTC = outputRes;
			break;
		case WELCOME_END:
			break;	
		default:
			break;
	}
	return state;
}
*/

/*
enum DROP_DOWN_STATES {DROP_INIT, DROP_SET, DROP_OUT};

int DROP_DOWN_Tick(int state)
{
	switch(state)
	{
		case DROP_INIT:
			
			col_holder = drop_col;
			state = DROP_SET:
			break;
		case DROP_SET:
			
			state = DROP_OUT:
			break;
		case DROP_OUT:
			state = DROP_SET;
			break;
		default:
			state = DROP_INIT;
			break;
	}
	
	switch(state)
	{
		case DROP_INIT:
			break;
		case DROP_SET:
			
			for(drop_row = 0; drop_row < 7; ++drop_row)
			{
				
			}
			break;
		case DROP_OUT:

			break;
		default:
			break;
	}	
}*/

/*
enum MOVEMENT_STATES {MOVEMENT_INIT, MOVEMENT_WAIT, MOVEMENT_LEFT, MOVEMENT_RIGHT, };
int MOVEMENT_Tick(int state)
{
	get_package = USART_Receive();
	
	switch(state)
	{
		case MOVEMENT_INIT:
			PORTA = 0x00; PORTC = 0xFE; //all rows turned off
			state = MOVEMENT_WAIT;
			break;
		case MOVEMENT_WAIT:
			if(get_package == 0x02) state = MOVEMENT_LEFT;
			else if(get_package == 0x04) state = MOVEMENT_RIGHT;
			else state = MOVEMENT_WAIT;
			
			break;	
		case MOVEMENT_LEFT:
			state = MOVEMENT_WAIT;
			break;
		case MOVEMENT_RIGHT:
			state = MOVEMENT_WAIT;
			break;
		default:
			break;
	}
	
	switch(state)
	{
		case MOVEMENT_INIT:
			//PORTA = 0x00; PORTC = 0xFE;
			break;
		case MOVEMENT_WAIT:
			PORTA = strafe_cols[control_col];
			break;
		case MOVEMENT_LEFT:
			if(control_col > 0) //airplane is in row bounds
			{
				--control_col;
			}
			break;
		case  MOVEMENT_RIGHT:
			if(control_col < 7) //airplane is in row bounds
			{
				++control_col;
			}
			break;
		default:
			break;
	}
	
	return state;
}
*/
#endif