/*
 * main.c 
 * 
 * Description : Example usage of the mastermind library functions
 *
 * Company : I&Q
 * Author : Hugo Arends
 * Date : July 2017
 */ 
#define F_CPU 16000000UL // 16Mhz
#include <avr/io.h>
#include <time.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "mastermind.h"
#include "global_var.h"
#include "uart.h"

#define LED_INIT   DDRB  |=  (1<<DDB5)
#define LED_ON     PORTB |=  (1<<PORTB5)
#define LED_OFF    PORTB &= ~(1<<PORTB5)
#define LED_TOGGLE PINB  |=  (1<<PINB3)
#define SWITCH_PRESSED	!(PINB & (1<<PINB7))

void generateCode(void);

mm_result_t mm_result;
//char secret_code[MM_DIGITS];
//char user_code[MM_DIGITS];

char secret_code[4];
char user_code[4];

ISR(TIMER0_OVF_vect)
{
	count++;
}

ISR(PCINT0_vect)
{
	LED_TOGGLE;
	if (SWITCH_PRESSED)
	{
		generateCode();
		set_secret_code(secret_code);
		attempt = 12;
		wait = 0;
		LED_OFF;
	}
}

int main(void)
{
	int start = 1;
	
	// initialize the uart
	InitUART(MYUBRR);
        
    // Initialize the LED    
    LED_INIT;
    LED_OFF;    
	
	// Set interrupt in pin pb7
	PCMSK0 |= (1<<PCINT7);	// Pin Change Enable
	PCICR |= (1<<PCIE0);	// Pin Change Interrupt Enable 0
	sei(); // Global Interrupt Enable
	
	// set timer for random counter.
	TCCR0B |= (1<<CS00);
	TCCR0B |= (1<<CS02);
	TIMSK0 |= (1<<TOIE1);
	
	DDRB |= (1<<DDB3);		// Pin b3 is output
	DDRB &= ~(1<<DDB7);		// pin b7 is input
    
	secret_code[0] = '1';
	secret_code[1] = '1';
	secret_code[2] = '2';
	secret_code[3] = '2';
	secret_code[4] = NULL;
    // Just as an example, the user code is set statically
    // TODO: Get the code from the user
    user_code[0] = '1';
    user_code[1] = '2';
    user_code[2] = '3';
    user_code[3] = '4';
	user_code[4] = NULL;
    
    while (1) 
    {
		char byte2send = '0';
		
		srand(count);
		/*
		if (start)
		{
			TransmitString("The game has begun\r\n");
			_delay_ms(100);
			start = 0;
		}
		
		*/
		
		
		// debug info
		user_code[4] = NULL;
		TransmitString(secret_code);
		TransmitString(user_code);
		
		// Check the user code against the secret code
		mm_result = check_secret_code(user_code);
		
		byte2send = mm_result.correct_num_and_pos + '0';
		TransmitByte(byte2send);
		byte2send = mm_result.correct_num + '0';
		TransmitByte(byte2send);
		_delay_ms(100);
		
		if (attempt)
		{
			if(mm_result.correct_num_and_pos == 4)
			{
				LED_ON;
				_delay_ms(100);
				TransmitString("");
				TransmitString("\r\nYou are my Mastermind\r\n");
				_delay_ms(100);
				TransmitString("Press the button to reset the game\r\n");
				_delay_ms(100);
				wait = 1;
				while(wait);

			} else
			{
				TransmitString("Enter your code\r\n");
				attempt--;
				
				ReceiveString(user_code);
				
			}	
		} else
		{
			TransmitString("You have lost\r\n");
			TransmitString("Press the switch to reset the game\r\n");
			_delay_ms(100);
			//TransmitString("Press the button to reset the game\r\n");
			//_delay_ms(100);
			wait = 1;
			while(wait);
		}
		
    }
}
void generateCode(void)
{
	secret_code[0] = '1' + rand()%7;
	secret_code[1] = '1' + rand()%7;
	secret_code[2] = '1' + rand()%7;
	secret_code[3] = '1' + rand()%7;
	secret_code[4] = NULL;
	
	TransmitString("\r\nThe game has been reset\r\n");
	_delay_ms(100);
	
	return;
}
