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
void resetGame(void);
void printCode(char code[4]);
int *getCode();

mm_result_t mm_result;
//char secret_code[MM_DIGITS];
//char user_code[MM_DIGITS];

char secret_code[4];
char user_code[4];


ISR(PCINT0_vect)
{
	resetGame();
}

int main(void)
{
	// initialize the uart
	InitUART(MYUBRR);
        
    // Initialize the LED    
    LED_INIT;
    LED_OFF;    
	
	// Set interrupt in pin pb7
	PCMSK0 |= (1<<PCINT7);	// Pin Change Enable
	PCICR |= (1<<PCIE0);	// Pin Change Interrupt Enable 0
	sei(); // Global Interrupt Enable
	
	DDRB |= (1<<DDB3);		// Pin b3 is output
	DDRB &= ~(1<<DDB7);		// pin b7 is input
    
	// set count for random number
	int count = 0;
    
	secret_code[0] = 1;
	secret_code[1] = 1;
	secret_code[2] = 2;
	secret_code[3] = 2;
	//secret_code[4] = NULL;
    // Just as an example, the user code is set statically
    // TODO: Get the code from the user
    user_code[0] = 1;
    user_code[1] = 2;
    user_code[2] = 3;
    user_code[3] = 4;
	//user_code[4] = NULL;
    
    // Check the user code against the secret code
    mm_result = check_secret_code(user_code);
    /*
	if(mm_result.correct_num_and_pos == 4)
	{
		LED_ON;
	}
	*/
    // Stop executing
    // TODO: Give the user 12 opportunities
    // TODO: Start over once a game has finished
    while (1) 
    {
		char byte2send = '0';
		count++;
		srand(count);
		
		//TransmitString(secret_code);
		_delay_ms(50);
		//TransmitString(user_code);
		
		printCode(secret_code);
		printCode(user_code);
		
		
		byte2send = mm_result.correct_num_and_pos + '0';
		TransmitByte(byte2send);
		byte2send = mm_result.correct_num + '0';
		TransmitByte(byte2send);
		
		_delay_ms(100);
		mm_result = check_secret_code(user_code);
		
		if (attempt)
		{
			if(mm_result.correct_num_and_pos == 4)
			{
				LED_ON;
				TransmitString("You are my Mastermind\r\n");
				_delay_ms(100);
				TransmitString("Press the switch to reset the game\r\n");
				//TransmitString("Or press enter to refresh\r\n");
				//resetGame();
				
			} else
			{
				TransmitString("Enter your code\r\n");
				
				//ReceiveString(user_code);
				*user_code = getCode();
				attempt--;
			}	
		} else
		{
			TransmitString("You have lost\r\n");
			TransmitString("Press the switch to reset the game");
		}
		
		mm_result = check_secret_code(user_code);
		
		//_delay_ms(100);
		
    }
}
void generateCode(void)
{
	secret_code[0] = 1 + rand()%6;
	secret_code[1] = 1 + rand()%6;
	secret_code[2] = 1 + rand()%6;
	secret_code[3] = 1 + rand()%6;
	//TransmitString(secret_code);
	//secret_code[4] = NULL;
	
	return;
}


void resetGame()
{
	LED_TOGGLE;
	if (SWITCH_PRESSED)
	{
		generateCode();
		set_secret_code(secret_code);
		attempt = 12;
		mm_result.correct_num = 0;
		mm_result.correct_num_and_pos = 0;
		LED_OFF;
	}
}

void printCode(char code[4])
{
	char i;
	for (i = 0; i < 4; i++)
	{
		TransmitByte('0' + code[i]);
		code[i] = 0;
	}
}

int *getCode()
{
	char i;
	char array[4];
	ReceiveString(array);
	for (i = 0; i < 4; i++)
	{
		array[i] =+ '0';
	}
	return array;
}