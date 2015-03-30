/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: task scheduler for first atmel1284
 *		controls ammo gauge, lcd screen, speaker, and gets input from keypad
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <avr/io.h>
#include "bgm.h"
#include "bit.h"
#include "io.h"
#include "keypad.h"
#include "scheduler.h"
#include "timer.h"
#include "ammo.h"
#include "menu.h"

task tasks[3];

const unsigned char tasksNum = 3;
const unsigned long tasksPeriodGCD = 1;
const unsigned long periodBgm = 480;
const unsigned long periodAmmo = 100;
const unsigned long periodMenu = 500;

int main()
{
	DDRA = 0xFF; PORTA = 0x00; // lcd data ports
	DDRB = 0xFF; PORTB = 0x00; // Ammo Output Display, sound, lcd control lines
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00; // 

	initUSART();

	TimerSet(tasksPeriodGCD);
	TimerOn();

	unsigned char i = 0;

	tasks[i].state = BGM_OFF;
	tasks[i].period = periodBgm;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &BGM_Tick;
	++i;
	tasks[i].state = AMMO_INIT;
	tasks[i].period = periodAmmo;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &AMMO_Tick;
	++i;
	tasks[i].state = MENU_WAIT;
	tasks[i].period = periodMenu;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &MENU_Tick;

	LCD_init();
	while(1)
	{
		unsigned char i; //scheduler code
		for (i = 0; i < tasksNum; ++i)
		{
			if ( tasks[i].elapsedTime >= tasks[i].period ) //cycle through tasks
			{
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += tasksPeriodGCD;
		}
	}
	return 0;
}