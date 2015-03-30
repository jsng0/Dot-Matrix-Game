/* Name & E-mail: Jordon Ng, jng010@ucr.edu
 * Lab Section: 221
 * Assignment: Lab 10
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */#ifndef BGM_H_
#define BGM_H_

#include <avr/pgmspace.h>
#include "bit.h"

#define A5 880.00 //flat notes 
#define B5 466.16
#define C5 523.25
#define D5 587.33
#define E5 659.26
#define F5 698.46
#define G5 783.99

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound

void set_PWM(double frequency)
{
	//static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.

	//if (frequency != current_frequency)
	{
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		
		TCNT3 = 0; // resets counter
		//current_frequency = frequency; // Updates the current frequency
		TCNT3 = OCR3A <= TCNT3 ? TCNT3-OCR3A : TCNT3;
	}
}

void PWM_on()
{
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	//sTCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}


void PWM_off()
{
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

const double bgmNotes[] =	{ G5, E5, G5, G5, E5, G5, A5, G5, F5, E5,
							  D5, E5, F5, E5, F5, G5, C5, C5, C5, C5,
							  C5, D5, E5, F5, G5, G5, D5, D5, F5, E5,
							  D5, C5, G5, E5, G5, G5, E5, G5, A5, G5,
							  F5, E5, D5, E5, F5, E5, F5, G5, C5, C5,
							  C5, C5, C5, D5, E5, F5, G5, G5, D5, D5,
							  F5, E5, D5, C5 };


const unsigned char noteLength[] = {	8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
										8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
										8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
										8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
										8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
										8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
										8, 8, 8, 8 };

const unsigned char silentLength[] = {	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
										2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
										2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
										2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
										2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
										2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
										2, 2, 2, 8 };

enum BGM_States { BGM_INIT, BGM_PLAY, BGM_OFF};
unsigned char noteIndex = 0, noteCount = 0;

int BGM_Tick(int state)
{
	//unsigned char game_start = ~getBit(PINB,7);
	//game_start = 1; //test
	switch(state) 
	{
		case BGM_INIT:
			PWM_on();
			noteCount = 0;
			noteIndex = 0;
			/*if(!game_start)
			{ 
				state = BGM_INIT;
				PWM_off();
				break;
			}*/
			state = BGM_PLAY;
			break;
		case BGM_PLAY:
			if(noteCount >= noteLength[noteIndex]) //index has went through the array
			{
				set_PWM(0);
				noteCount = 0; //reset the index
				state=BGM_OFF;
				break;
			}
			set_PWM(bgmNotes[noteIndex]);
			break;
		case BGM_OFF:
			if(noteCount >= silentLength[noteIndex])
			{
				noteIndex = (noteIndex >= 63) ? 0 : noteIndex + 1; //iterate for number of notes in song
				noteCount=0;
				set_PWM(bgmNotes[noteIndex]);
				state=BGM_PLAY;
			}
			break;
		default:
			state = BGM_INIT;
			break;
		
	}
	
	switch(state) {
		case BGM_INIT:
			noteCount = 0;
			noteIndex = 0;
			break;
		case BGM_PLAY:
			set_PWM(bgmNotes[noteIndex]);
			++noteCount;
			break;
		case BGM_OFF:
			++noteCount;
			break;
		default:
			break;
	}
	return state;
}

#endif