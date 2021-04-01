#include <avr/interrupt.h> //Used for Independent School AVR system, for microcontroller conversion


//shift2.h allows data to be sent to two different external shift registers using the microcontroller
void transmit_data(unsigned char reg, unsigned char data)
{
	if (reg == 1)
	{
		int i;
		for (i = 0; i < 8; i++)
		{
			PORTC = 0x08;
			PORTC |= ((data >> i) & 0x01);
			PORTC |= 0x02;
		}
		PORTC |= 0x04;
		PORTC = 0x00;
	}
	else
	{
		int i;
		for (i = 0; i < 8; i++)
		{
			PORTC = 0x20;
			PORTC |= ((data >> i) & 0x01);
			PORTC |= 0x02;
		}
		PORTC |= 0x10;
		PORTC = 0x00;
	}

}