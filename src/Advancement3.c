
/*	Author: Ethan Bayer
 *  Partner(s) Name: NA
 *	Lab Section: 23
 *	Assignment: Lab #14  Advancement #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://www.youtube.com/watch?v=SgVQKr02mYE&list=PLePqx8Mc55_7e_rFEPCPLzIr4YNqKs4MF&index=32
 *
 *
 */
#include <avr/io.h> //Used for Independent School System.
#include "../headers/timer.h"
#include "../headers/bit.h"
#include "../headers/shift2.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct task
{
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct)(int);
} task;

unsigned char PadHit(unsigned char pad, unsigned char ball)
{
	unsigned char ballOne = (ball << 1) | 0x01;
	unsigned char ballTwo = (ball << 2) | 0x03;
	unsigned char set;
	if ((pad | ballTwo) == ballTwo)
	{
		set = 1;
	}
	else if ((pad | ballOne) == ballOne)
	{
		set = 2;
	}
	else if ((pad | ball) == ball)
	{
		set = 3;
	}
	
	return set;
}

unsigned char PlayerPad_row = 0xF1;
unsigned char PlayerPad_col = 0x01;
enum PlayerPad_States { PlayerPad_Start, OnPress, OffPress };
int PlayerPad_Tick(int state)
{
	unsigned char button = ~PINA & 0x03;

	switch (state)
	{
		case PlayerPad_Start:
			state = OnPress;
			break;
		case OnPress:
			if ((button & 0x01) == 0x01)
			{
				if (PlayerPad_row != 0xF8)
				{
					PlayerPad_row = (PlayerPad_row >> 1) | 0x80;
				}
				state = OffPress;
			}
			else if ((button & 0x02) == 0x02)
			{
				if (PlayerPad_row != 0xE3)
				{
					PlayerPad_row = (PlayerPad_row << 1) | 0x01;
				}
				state = OffPress;
			}
			else
			{
				state = OnPress;
			}
			break;
		case OffPress:
			if (button == 0x00)
			{
				state = OnPress;
			}
			else
			{
				state = OffPress;
			}
			break;
		default:
			state = PlayerPad_Start;
			break;
	}
	
	return state;
}

unsigned char GameType = 1;

unsigned char ballRow = 0xFB;
unsigned char ballCol = 0x08;
unsigned char moveType = 2;		// 1 is diagonally up, 2 is straight, 3 is diagonally down

/*
 * Ai System:
 * If the random number is a 1, the Ai moves up.
 * If the random number is a 2, the Ai moves down.
 * If the random number is a 3, the Ai stays still.
 * If the random number is either 4 or 5 it will follow the ball.
 */
unsigned char AiPad_row = 0xF1;
unsigned char AiPad_center = 0xFB;
unsigned char AiPad_col = 0x80;
unsigned char randMove;
unsigned char cnt;
enum AiPad_States { AiPad_Start, Choose, Move };
int AiPad_Tick(int state)
{
	switch (state)
	{
		case AiPad_Start:
			randMove = 0;
			srand(time(NULL));
			state = Choose;
			break;
		case Choose:
			randMove = rand() % 5 + 1;
			cnt = 0;
			state = Move;
			break;
		case Move:
			if (GameType == 1)
			{
				if (randMove == 1)
				{
					if (AiPad_row != 0xF8)
					{
						AiPad_row = (AiPad_row >> 1) | 0x80;
						AiPad_center = (AiPad_center >> 1) | 0x80;
					}
				}
				else if (randMove == 2)
				{
					if (AiPad_row != 0xE3)
					{
						AiPad_row = (AiPad_row << 1) | 0x01;
						AiPad_center = (AiPad_center << 1) | 0x01;
					}
				}
				else if (randMove == 4 || randMove == 5)
				{
					if (AiPad_center < ballRow)
					{
						if (AiPad_row != 0xF8)
						{
							AiPad_row = (AiPad_row >> 1) | 0x80;
							AiPad_center = (AiPad_center >> 1) | 0x80;
						}
					}
					else if (AiPad_center > ballRow)
					{
						if (AiPad_row != 0xE3)
						{
							AiPad_row = (AiPad_row << 1) | 0x01;
							AiPad_center = (AiPad_center << 1) | 0x01;
						}
					}
				}
			}
			else
			{
				if (AiPad_center < ballRow)
				{
					if (AiPad_row != 0xF8)
					{
						AiPad_row = (AiPad_row >> 1) | 0x80;
						AiPad_center = (AiPad_center >> 1) | 0x80;
					}
				}
				else if (moveType == 1 && ballCol > 0x02)
				{
					if (AiPad_row != 0xF8)
					{
						AiPad_row = (AiPad_row >> 1) | 0x80;
						AiPad_center = (AiPad_center >> 1) | 0x80;
					}
				}
				else if (moveType == 3 && ballCol > 0x02)
				{
					if (AiPad_row != 0xE3)
					{
						AiPad_row = (AiPad_row << 1) | 0x01;
						AiPad_center = (AiPad_center << 1) | 0x01;
					}
				}
				else if (AiPad_center > ballRow)
				{
					if (AiPad_row != 0xE3)
					{
						AiPad_row = (AiPad_row << 1) | 0x01;
						AiPad_center = (AiPad_center << 1) | 0x01;
					}
				}
			}
			state = Choose;
			break;
		default:
			state = AiPad_Start;
			break;
	}
	
	return state;
}
	
	
//unsigned char ballRow;
//unsigned char ballCol;
unsigned char leftOrRight;	// 1 is right, 2 is left
//unsigned char moveType;		// 1 is diagonally up, 2 is straight, 3 is diagonally down
unsigned char padDetection;
unsigned char DiagonalIssue;
enum Ball_States { Ball_Start, movement, score };
int Ball_Tick(int state)
{
	switch (state)
	{
		case Ball_Start:
			ballRow = 0xFB;
			ballCol = 0x08;
			leftOrRight = 1;
			moveType = 2;
			state = movement;
			break;
		case movement:
			if (moveType == 2 && leftOrRight == 1)	//Straight right
			{
				if (ballCol != 0x02)
				{
					ballCol >>= 1;
					state = movement;
				}
				else if (ballCol == 0x02 && ((~PlayerPad_row & ~ballRow) == ~ballRow))
				{
					padDetection = PadHit(PlayerPad_row, ballRow);
					if (padDetection == 1)
					{
						moveType = 1;
					}
					else if (padDetection == 2)
					{
						moveType = 2;
					}
					else
					{
						moveType = 3;
					}
					leftOrRight = 2;
					state = movement;
				}
				else
				{
					ballCol >>= 1;
					state = score;
				}
			}
			else if (moveType == 2 && leftOrRight == 2) //Straight left
			{
				if (ballCol != 0x40)
				{
					ballCol <<= 1;
					state = movement;
				}
				else if (ballCol == 0x40 && ((~AiPad_row & ~ballRow) == ~ballRow))
				{
					padDetection = PadHit(AiPad_row, ballRow);
					if (padDetection == 1)
					{
						moveType = 1;
					}
					else if (padDetection == 2)
					{
						moveType = 2;
					}
					else
					{
						moveType = 3;
					}
					leftOrRight = 1;
					state = movement;
				}
				else
				{
					ballCol <<= 1;
					state = score;
				}
			}
			else if (moveType == 1 && leftOrRight == 1)	//Diagonally up right
			{
				DiagonalIssue = ~ballRow >> 1;
				if (ballCol != 0x02 && ballRow != 0xFE)
				{
					ballCol >>= 1;
					ballRow = (ballRow >> 1) | 0x80;
					state = movement;
				}
				else if (ballCol == 0x02 && ((~PlayerPad_row & ~ballRow) == ~ballRow))
				{
					padDetection = PadHit(PlayerPad_row, ballRow);
					if (padDetection == 1)
					{
						moveType = 1;
					}
					else if (padDetection == 2)
					{
						moveType = 2;
					}
					else
					{
						moveType = 3;
					}
					leftOrRight = 2;
					state = movement;
				}
				else if (ballCol == 0x02 && ((~PlayerPad_row & DiagonalIssue) == DiagonalIssue))
				{
					moveType = 3;
					leftOrRight = 2;
					state = movement;
				}
				else if (ballRow == 0xFE && ballCol != 0x02)
				{
					moveType = 3;
					state = movement;
				}
				else
				{
					ballCol >>= 1;
					if (ballRow != 0xFE)
					{
						ballRow = (ballRow >> 1) | 0x80;
					}
					state = score;
				}
			}
			else if (moveType == 1 && leftOrRight == 2)	//Diagonally up left
			{
				DiagonalIssue = ~ballRow >> 1;
				if (ballCol != 0x40 && ballRow != 0xFE)
				{
					ballCol <<= 1;
					ballRow = (ballRow >> 1) | 0x80;
					state = movement;
				}
				else if (ballCol == 0x40 && ((~AiPad_row & ~ballRow) == ~ballRow))
				{
					padDetection = PadHit(AiPad_row, ballRow);
					if (padDetection == 1)
					{
						moveType = 1;
					}
					else if (padDetection == 2)
					{
						moveType = 2;
					}
					else
					{
						moveType = 3;
					}
					leftOrRight = 1;
					state = movement;
				}
				else if (ballCol == 0x40 && ((~AiPad_row & DiagonalIssue) == DiagonalIssue))
				{
					moveType = 3;
					leftOrRight = 1;
					state = movement;
				}
				else if (ballRow == 0xFE && ballCol != 0x40)
				{
					moveType = 3;
					state = movement;
				}
				else
				{
					ballCol <<= 1;
					if (ballRow != 0xFE)
					{
						ballRow = (ballRow >> 1) | 0x80;
					}
					state = score;
				}
			}
			else if (moveType == 3 && leftOrRight == 1)	//Diagonally down right
			{
				DiagonalIssue = ~ballRow << 1;
				if (ballCol != 0x02 && ballRow != 0xEF)
				{
					ballCol >>= 1;
					ballRow = (ballRow << 1) | 0x01;
					state = movement;
				}
				else if (ballCol == 0x02 && ((~PlayerPad_row & ~ballRow) == ~ballRow))
				{
					padDetection = PadHit(PlayerPad_row, ballRow);
					if (padDetection == 1)
					{
						moveType = 1;
					}
					else if (padDetection == 2)
					{
						moveType = 2;
					}
					else
					{
						moveType = 3;
					}
					leftOrRight = 2;
					state = movement;
				}
				else if (ballCol == 0x02 && ((~PlayerPad_row & DiagonalIssue) == DiagonalIssue))
				{
					moveType = 1;
					leftOrRight = 2;
					state = movement;
				}
				else if (ballRow == 0xEF && ballCol != 0x02)
				{
					moveType = 1;
					state = movement;
				}
				else
				{
					ballCol >>= 1;
					if (ballRow != 0xEF)
					{
						ballRow = (ballRow << 1) | 0x01;
					}
					state = score;
				}
			}
			else if (moveType == 3 && leftOrRight == 2)	//Diagonally down left
			{
				DiagonalIssue = ~ballRow << 1;
				if (ballCol != 0x40 && ballRow != 0xEF)
				{
					ballCol <<= 1;
					ballRow = (ballRow << 1) | 0x01;
					state = movement;
				}
				else if (ballCol == 0x40 && ((~AiPad_row & ~ballRow) == ~ballRow))
				{
					padDetection = PadHit(AiPad_row, ballRow);
					if (padDetection == 1)
					{
						moveType = 1;
					}
					else if (padDetection == 2)
					{
						moveType = 2;
					}
					else
					{
						moveType = 3;
					}
					leftOrRight = 1;
					state = movement;
				}
				else if (ballCol == 0x02 && ((~AiPad_row & DiagonalIssue) == DiagonalIssue))
				{
					moveType = 1;
					leftOrRight = 1;
					state = movement;
				}
				else if (ballRow == 0xEF && ballCol != 0x40)
				{
					moveType = 1;
					state = movement;
				}
				else
				{
					ballCol <<= 1;
					if (ballRow != 0xEF)
					{
						ballRow = (ballRow << 1) | 0x01;
					}
					state = score;
				}
			}
			break;
		case score:	//Score system to start the ball back at the center to serve. 		
			if (leftOrRight == 1)
			{
				ballCol = 0x10;
				ballRow = 0xFB;
				moveType = 2;
				leftOrRight = 2;
			}
			else if (leftOrRight == 2)
			{
				ballCol = 0x08;
				ballRow = 0xFB;
				moveType = 2;
				leftOrRight = 1;
			}
			state = movement;
			break;
		default:
			state = Ball_Start;
			break;
	}
	
	return state;
}

//SynchSM which determines Ai to play against and also allows the second player to take over
enum Game_States { Game_Start, WaitGame, GamePress };
int Game_Tick(int state)
{
	unsigned char button = ~PINA & 0x08;
	
	switch (state)
	{
		case Game_Start:
			state = WaitGame;
			break;
		case WaitGame:
			if ((button & 0x08) == 0x08)
			{
				if (GameType == 1)
				{
					GameType = 2;
				}
				else
				{
					GameType = 1;
				}
				state = GamePress;
			}
			else
			{
				state = WaitGame;
			}
			break;
		case GamePress:
			if (button == 0x00)
			{
				state = WaitGame;
			}
			else
			{
				state = GamePress;
			}
			break;
		default:
			state = Game_Start;
			break;
	}
	
	switch (state)
	{
		case Game_Start:
			break;
		case WaitGame:
			break;
		case GamePress:
			break;
		default:
			break;
	}
	
	PORTB = GameType;
	
	return state;
}

//Waits for the reset button to be pressed, then resets all values
enum Reset_States { Reset_Start, WaitReset, Reset };
int Reset_Tick(int state)
{
	unsigned char button = ~PINA & 0x04;

	switch (state)
	{
		case Reset_Start:
			state = WaitReset;
			break;
		case WaitReset:
			if ((button & 0x04) == 0x04)
			{
				state = Reset;
			}
			else
			{
				state = WaitReset;
			}
			break;
		case Reset:
			PlayerPad_row = 0xF1;
			PlayerPad_col = 0x01;
			AiPad_row = 0xF1;
			AiPad_center = 0xFB;
			AiPad_col = 0x80;
			ballRow = 0xFB;
			ballCol = 0x08;
			leftOrRight = 1;
			moveType = 2;

			state = WaitReset;
			break;
		default:
			state = Reset_Start;
			break;
	}
	return state;
}

enum Display_States { Dstart, Display1, Display2, Display3 };
int Display_Tick(int state)
{
	switch (state)
	{
		case Dstart:
			state = Display1;
			break;
		case Display1:
			PORTD = PlayerPad_row;
			//transmit_data(2, PlayerPad_row);
			PORTC = PlayerPad_col;
			//transmit_data(1, PlayerPad_col);
			state = Display2;
			break;
		case Display2:
			PORTD = AiPad_row;
			//transmit_data(2, AiPad_row);
			PORTC = AiPad_col;
			//transmit_data(1, AiPad_col);
			state = Display3;
			break;
		case Display3:
			PORTD = ballRow;
			//transmit_data(2, ballRow);
			PORTC = ballCol;
			//transmit_data(1, ballCol);
			state = Display1;
			break;
		default:
			state = Dstart;
			break;
	}
	return state;
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	static task task1, task2, task3, task4, task5, task6;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	const char start = -1;
	
	/*
	 * Large periods are due to issue with microcontroller making more reasonable 
	 * periods too fast for the system to work as intended (such as lights blinking too
	 * fast), so much larger ones were required for my system. 
	 * The SynchSM's which use buttons can have smaller periods since their
	 * lights are not using a generated sequence.
	 */

	task1.state = start;
	task1.period = 1000;
	task1.elapsedTime = task1.period;
	task1.TickFct = &PlayerPad_Tick;
	
	task2.state = start;
	task2.period = 150000;
	task2.elapsedTime = task2.period;
	task2.TickFct = &AiPad_Tick;

	task3.state = start;
	task3.period = 120000;
	task3.elapsedTime = task3.period;
	task3.TickFct = &Ball_Tick;
	
	task4.state = start;
	task4.period = 1000;
	task4.elapsedTime = task4.period;
	task4.TickFct = &Display_Tick;

	task5.state = start;
	task5.period = 1000;
	task5.elapsedTime = task5.period;
	task5.TickFct = &Reset_Tick;

	task6.state = start;
	task6.period = 50000;
	task6.elapsedTime = task6.period;
	task6.TickFct = &Game_Tick;
	
	TimerSet(100);
	TimerOn();
	
	//A2D_init();
	
	unsigned short i;
	while (1)
	{
		for (i = 0; i < numTasks; i++)
		{
			if (tasks[i]->elapsedTime == tasks[i]->period)
			{
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 100;
		}
		
		while (!TimerFlag)
		{
			TimerFlag = 0;
		}
	}
	return 0;
}