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

#define LED_INIT   DDRB  |=  (1<<DDB5)
#define LED_ON     PORTB |=  (1<<PORTB5)
#define LED_OFF    PORTB &= ~(1<<PORTB5)
#define LED_TOGGLE PINB  |=  (1<<PINB5)
#define SWITCH_PRESSED	!(PINB & (1<<PINB7))






#define FOSC 16000000 // Clock speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
/* UART Buffer Defines */
#define UART_RX_BUFFER_SIZE 32 /* 2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_SIZE 32
#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
#error RX buffer size is not a power of 2
#endif
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)
#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
#error TX buffer size is not a power of 2
#endif
/* Static Variables */
static char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile char UART_RxHead;
static volatile char UART_RxTail;
static char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile char UART_TxHead;
static volatile char UART_TxTail;
/* Prototypes */
void InitUART(unsigned int ubrr_val);
char ReceiveByte(void);
void TransmitByte(char data);
void ReceiveString(char *str);
void TransmitString(char *str);








void generateCode(void);

mm_result_t mm_result;
//char secret_code[MM_DIGITS];
//char user_code[MM_DIGITS];

char secret_code[4];
char user_code[4];


ISR(PCINT0_vect)
{
	LED_TOGGLE;
	if (SWITCH_PRESSED)
	{
		generateCode();
	}
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
	
    // Set the secret code
    // TODO: Just one secret code is not very challenging
    
	secret_code[0] = 1;
    secret_code[1] = 1;
    secret_code[2] = 2;
    secret_code[3] = 2;
    
	// set count for random number
	int count = 0;
    
    // Just as an example, the user code is set statically
    // TODO: Get the code from the user
    user_code[0] = 1;
    user_code[1] = 2;
    user_code[2] = 3;
    user_code[3] = 4;
    
    // Check the user code against the secret code
    mm_result = check_secret_code(user_code);
    
    // Turn on the LED if the user code matches the secret code
    // TODO: Implement more meaningful feedback for the user
    if(mm_result.correct_num_and_pos == 4)
    {
      LED_ON;
    }    
    
    // Stop executing
    // TODO: Give the user 12 opportunities
    // TODO: Start over once a game has finished
    while (1) 
    {
		count++;
		srand(count);
		
		
    }
}
void generateCode(void)
{
	secret_code[0] = '1' + rand()%7;
	secret_code[1] = '1' + rand()%7;
	secret_code[2] = '1' + rand()%7;
	secret_code[3] = '1' + rand()%7;
	secret_code[4] = '\n';
	TransmitString(secret_code);
	secret_code[4] = NULL;
	
	return;
}







/* Initialize UART */
void InitUART(unsigned int ubrr_val)
{
	char x;
	/* Set the baud rate */
	UBRR0H = (unsigned char)(ubrr_val>>8);
	UBRR0L = (unsigned char)ubrr_val;
	/* Enable UART receiver and transmitter */
	UCSR0B = ((1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0));
	/* Flush receive buffer */
	x = 0;
	UART_RxTail = x;
	UART_RxHead = x;
	UART_TxTail = x;
	UART_TxHead = x;
}
/* Interrupt handlers */
ISR(USART_RX_vect)
{
	char data;
	unsigned char tmphead;
	/* Read the received data */
	data = UDR0;
	/* Calculate buffer index */
	tmphead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxHead = tmphead;
	if (tmphead == UART_RxTail) {
		/* ERROR! Receive buffer overflow */
	}
	/* Store received data in buffer */
	UART_RxBuf[tmphead] = data;
}
ISR(USART_UDRE_vect)
{
	unsigned char tmptail;
	/* Check if all data is transmitted */
	if (UART_TxHead != UART_TxTail) {
		/* Calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		/* Store new index */
		UART_TxTail = tmptail;
		/* Start transmission */
		UDR0 = UART_TxBuf[tmptail];
		} else {
		/* Disable UDRE interrupt */
		UCSR0B &= ~(1<<UDRIE0);
	}
}
char ReceiveByte(void)
{
	unsigned char tmptail;
	/* Wait for incoming data */
	while (UART_RxHead == UART_RxTail);
	/* Calculate buffer index */
	tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
	/* Store new index */
	UART_RxTail = tmptail;
	/* Return data */
	return UART_RxBuf[tmptail];
}
void TransmitByte(char data)
{
	unsigned char tmphead;
	/* Calculate buffer index */
	tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
	/* Wait for free space in buffer */
	while (tmphead == UART_TxTail);
	/* Store data in buffer */
	UART_TxBuf[tmphead] = data;
	/* Store new index */
	UART_TxHead = tmphead;
	/* Enable UDRE interrupt */
	UCSR0B |= (1<<UDRIE0);
}
/*
* This function gets a string of characters from the USART.
* The string is placed in the array pointed to by str.
*
* - This function uses the function ReceiveByte() to get a byte
* from the UART.
* - If the received byte is equal to '\n' (Line Feed),
* the function returns.
* - The array is terminated with ´\0´.
*/
void ReceiveString(char *str)
{
	uint8_t t = 0;
	while ((str[t] = ReceiveByte()) != '\n')
	{
		t++;
	}
	str[t++] = '\n';
	str[t] = '\0';
}
/*
* Transmits a string of characters to the USART.
* The string must be terminated with '\0'.
*
* - This function uses the function TransmitByte() to
* transmit a byte via the UART
* - Bytes are transmitted until the terminator
* character '\0' is detected. Then the function returns.
*/
void TransmitString(char *str)
{
	while(*str)
	{
		TransmitByte(*str++);
	}
}
