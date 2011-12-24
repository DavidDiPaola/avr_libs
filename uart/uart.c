/***
Copyright (C) 2011 David DiPaola

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
***/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"

//This code relies on F_CPU being defined as the CPU clockrate in Hertz.
//The example Makefile supplied with this library does this.
// e.g.: #define F_CPU 8000000

//this is by no means a complete list, add targets as necessary
#define USART1_SUPPORTED (defined(__AVR_ATmega164P__)||defined(__AVR_ATmega324P__)||defined(__AVR_ATmega644P__)||defined(__AVR_ATmega1284P__))

//initialize a uart
// uint8_t uart - which uart to initialize
void uart_init(uint8_t uart, unsigned long baudrate){
  baudrate = ((F_CPU/(baudrate<<4))-1); //massage the baud rate

  if( uart == 0 ){
    // Set baud rate
    UBRR0L = (uint8_t)baudrate;
    UBRR0H = (uint8_t)(baudrate>>8);
    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  }

  #if USART1_SUPPORTED
  else {
    // Set baud rate
    UBRR1L = (uint8_t)baudrate;
    UBRR1H = (uint8_t)(baudrate>>8);
    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR1C = (0<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10);
    // Enable receiver and transmitter
    UCSR1B = (1<<RXEN1)|(1<<TXEN1);
  }
  #endif
}

//send a byte
// uint8_t uart - which uart to send on
// char data - the data to be sent
void uart_send(uint8_t uart, char data){
  if(uart == 0){
    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0) {};
    // Transmit data
    UDR0 = data;
  }

  #if USART1_SUPPORTED
  else {
    // Wait if a byte is being transmitted
    while((UCSR1A&(1<<UDRE1)) == 0) {};
    // Transmit data
    UDR1 = data;
  }
  #endif
}

//waits until a byte is received and returns it
// uint8_t uart - which uart to listen on
// returns char - the data received
char uart_get(uint8_t uart){
  char result = 0;

  if(uart == 0){
    //wait until a byte has been received
    while((UCSR0A&(1<<RXC0)) == 0) {};
    //get received data
    result = UDR0;
  }

  #if USART1_SUPPORTED
  else {
    //wait until a byte has been received
    while((UCSR1A&(1<<RXC1)) == 0) {};
    //get received data
	result = UDR1;
  }
  #endif

  return result;
}

//sends characters from a null-terminated string
//does NOT send the null character
//  uint8_t uart - which uart to send on
//  const char* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_print(uint8_t uart, const char* data, uint16_t maxlen){
  uint16_t i=0;

  //iterate through the string until a null-terminator is found or the maximum
  // string length is reached
  while( (data[i] != '\0') && (i < maxlen) ){
    uart_send(uart, data[i]); //send a character
    i++; //increment to the next potential character
  }
}

//uart_prints a null-terminated string followed by CR and LF chars
//  uint8_t uart - which uart to send on
//  const char* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_println(uint8_t uart, const char* data, uint16_t maxlen){
  //print the string
  uart_print(uart, data, maxlen);
  //print the newline characters
  uart_send(uart, '\n');
  uart_send(uart, '\r');
}

//sends characters from a null-terminated string in progmem
//does NOT send the null character
//  uint8_t uart - which uart to send on
//  const prog_uchar* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_prgprint(uint8_t uart, const prog_uchar* data, uint16_t maxlen){
  uint16_t i=0;
  char c = pgm_read_byte_near(data); //prime the loop

  //iterate through the string until a null-terminator is found or the maximum
  // string length is reached
  while( (c != '\0') && (i < maxlen) ){
    uart_send(uart, c); //send a character
    i++; //increment to the next potential character
    c = pgm_read_byte_near(data + i);
  }
}

//uart_prints a null-terminated string in progmem followed by CR and LF chars
//  uint8_t uart - which uart to send on
//  const prog_uchar* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_prgprintln(uint8_t uart, const prog_uchar* data, uint16_t maxlen){
  //print the string
  uart_prgprint(uart, data, maxlen);
  //print the newline characters
  uart_send(uart, '\n');
  uart_send(uart, '\r');
}


//receives up to maxlen characters, putting them into a string
//  uint8_t uart - which uart to listen on
//  char* data - the string to be written to
//  uint16_t maxlen - the number of characters to be sent
//  uint8_t echo - if 0, characters received will not be echoed
void uart_getln(uint8_t uart, char* data, uint16_t maxlen, uint8_t echo){
  uint16_t i = 0;
  char temp;  //was set to 'a'

  //prime
  temp = uart_get(uart);
  //echo
  if( echo ){
    uart_send(uart, temp);
  }

  while( (temp != '\0') &&   //NULL
         (temp != '\n') &&   //LF
         (temp != '\r') &&   //CR
         (i < (maxlen-1)) ){

    //check for backspace
    if( (temp == 0x7F) ||
        (temp == '\b') ){
      if( i>0 ){
        i--;
      }
      //echo the backspace
      if( echo ){
        uart_send(uart, temp);
      }
    } else {
      data[i] = temp;
      i++;
    }

    temp = uart_get(uart);
    //echo the character
    if( echo ){
      uart_send(uart, temp);
    }
  }

  //echo the newline
  if( echo ){
    uart_send(uart, '\n');
    uart_send(uart, '\r');
  }

  data[i] = '\0';
}
