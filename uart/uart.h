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

#ifndef __UART_H
#define __UART_H

#include <avr/pgmspace.h>

//This code relies on F_CPU being defined as the CPU clockrate in Hertz.
//The example Makefile supplied with this library does this.
// e.g.: #define F_CPU 8000000

//initialize a uart
// uint8_t uart - which uart to initialize
void uart_init(uint8_t uart, unsigned long baudrate);

//send a byte
// uint8_t uart - which uart to send on
// char data - the data to be sent
void uart_send(uint8_t uart, char data);

//waits until a byte is received and returns it
// uint8_t uart - which uart to listen on
// returns char - the data received
char uart_get(uint8_t uart);

//sends characters from a null-terminated string
//does NOT send the null character
//  uint8_t uart - which uart to send on
//  const char* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_print(uint8_t uart, const char* data, uint16_t maxlen);

//uart_prints a null-terminated string followed by CR and LF chars
//  uint8_t uart - which uart to send on
//  const char* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_println(uint8_t uart, const char* data, uint16_t maxlen);

//sends characters from a null-terminated string in progmem
//does NOT send the null character
//  uint8_t uart - which uart to send on
//  const prog_uchar* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_prgprint(uint8_t uart, const prog_uchar* data, uint16_t maxlen);

//uart_prints a null-terminated string in progmem followed by CR and LF chars
//  uint8_t uart - which uart to send on
//  const prog_uchar* data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_prgprintln(uint8_t uart, const prog_uchar* data, uint16_t maxlen);

//receives up to maxlen characters, putting them into a string
//  uint8_t uart - which uart to listen on
//  char* data - the string to be written to
//  uint16_t maxlen - the number of characters to be sent
//  uint8_t echo - if 0, characters received will not be echoed
void uart_getln(uint8_t uart, char* data, uint16_t maxlen, uint8_t echo);

#endif
