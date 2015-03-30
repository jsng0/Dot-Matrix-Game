/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: start with 5 ammo, reduces by 1 from keypad 2 and reloads with keypad 4
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#ifndef AMMO_H_
#define AMMO_H_

#include <avr/io.h>
#include "keypad.h"
#include "usart.h"

enum AMMO_STATES { AMMO_INIT, AMMO_WAIT, AMMO_SHOOT, AMMO_EMPTY};

//global variables for amount of ammo left
unsigned char ammoRemaining_g;

int AMMO_Tick(int state)
{
	const int shoot = GetKeypadKey() == '2';
	const int reload = GetKeypadKey() == '4';
	switch(state)
	{
		case AMMO_INIT:
			ammoRemaining_g = 0x1F;	
			state = AMMO_WAIT;
			break;
		case AMMO_WAIT:
			if(shoot)
			{
				state = AMMO_SHOOT;
				ammoRemaining_g = ammoRemaining_g >> 1; //remove 1 ammo
			}
			else if(reload && !shoot)
			{
				state = AMMO_INIT;
			}
			else if(!shoot && !reload)
			{
				state = AMMO_WAIT;
			}
			break;
		case AMMO_SHOOT:
			if(shoot) //only count shoot once so loop
			{
				state = AMMO_SHOOT;
			}
			else if( !shoot && (ammoRemaining_g != 0x00) ) //let go of shoot and still have ammo
			{
				state = AMMO_WAIT;
			}
			else //let go of shoot and no more ammo
			{
				state = AMMO_EMPTY;
			}
			break;
		case AMMO_EMPTY:
			if(reload) //reload is pressed
			{
				state = AMMO_INIT; //re-init the ammo and start over
			}
			else //stay in empty ammo
			{
				state = AMMO_EMPTY;
			}
			break;
		default:
			state = AMMO_INIT;
			break;
	}
	
	switch(state)
	{
		case AMMO_INIT:
			ammoRemaining_g = 0x15; //5 ammo (00011111b);
			break;
		case AMMO_WAIT:
			break;
		case AMMO_SHOOT:
			break;
		case AMMO_EMPTY:
			break;
		default:
			break;
	}
	//unsigned char out_of_ammo = (ammoRemaining_g > 0) ? 0 : 0x20; //display out of ammo led B5
	PORTB = ammoRemaining_g; //| out_of_ammo; //display ammo before leaving tick function
	return state;
}

#endif