/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: task scheduler for second atmel1284
 *		controls led matrix via usart
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <avr/io.h>
#include "bit.h"
#include "scheduler.h"
#include "timer.h"
#include "welcome.c"

#include <avr/io.h>

task tasks[2];

const unsigned char tasksNum = 2;
const unsigned long tasksPeriodGCD = 1;
//const unsigned long periodWelcome = 7500;
//const unsigned long periodMovement = 1000;
const unsigned long periodStrafe = 80;
const unsigned long periodPA = 1;

int main()
{
	DDRA = 0xFF; PORTA = 0x00; //voltage
	DDRC = 0xFF; PORTC = 0x00; //resistances
	DDRD = 0x00; PORTD = 0x00; //usart

	initUSART();
	
	TimerSet(tasksPeriodGCD);
	TimerOn();

	unsigned char i = 0;

	/*tasks[i].state = WELCOME_INIT;
	tasks[i].period = periodWelcome;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &WELCOME_Tick;
	++i;
	tasks[i].state = MOVEMENT_INIT;
	tasks[i].period = periodMovement;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &MOVEMENT_Tick;*/
	
	tasks[i].state = PRINT_AIRPLANE_INIT;
	tasks[i].period = periodPA;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &PRINT_AIRPLANE_Tick;
	++i;
	tasks[i].state = PLANE_STRAFE_INIT;
	tasks[i].period = periodStrafe;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &PLANE_STRAFE_Tick;
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