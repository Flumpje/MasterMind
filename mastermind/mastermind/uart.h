/*
 * uart.h
 *
 * Created: 15-1-2019 18:54:26
 *  Author: w.meijer
 */ 


#ifndef UART_H_
#define UART_H_



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


#endif /* UART_H_ */