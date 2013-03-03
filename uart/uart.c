#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"

//This code relies on F_CPU being defined as the CPU clockrate in Hertz.
//The example Makefile supplied with this library does this.
// e.g.: #define F_CPU 8000000

#ifndef __AVR_ATmega1284P__
#error "This UART library does not support your AVR, please modify uart.c"
#endif

//initialize a uart
// uint8_t uart - which uart to initialize
void uart_init(uint8_t uart, unsigned long baudrate){
  baudrate = (F_CPU/16/baudrate-1); //massage the baud rate

  //USART 0
  if(uart == 0){
    //Set baud rate
    UBRR0H = (uint8_t)(baudrate>>8);
    UBRR0L = (uint8_t)baudrate;

    //Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    //NOTE: some devices require the URSEL bit to be set in this step
    //Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
  }

  //USART 1
  else if(uart == 1){
    //Set baud rate
    UBRR1H = (uint8_t)(baudrate>>8);
    UBRR1L = (uint8_t)baudrate;

    //Enable receiver and transmitter
    UCSR1B = (1<<RXEN1)|(1<<TXEN1);

    //NOTE: some devices require the URSEL bit to be set in this step
    //Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR1C = (0<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10);
  }
}

//send a byte
// uint8_t uart - which uart to send on
// char data - the data to be sent
inline void uart_send(uint8_t uart, char data){
  if(uart == 0){
    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0) {};

    // Transmit data
    UDR0 = data;
  }
  else if(uart == 1){
    // Wait if a byte is being transmitted
    while((UCSR1A&(1<<UDRE1)) == 0) {};

    // Transmit data
    UDR1 = data;
  }
}

//waits until a byte is received and returns it
// uint8_t uart - which uart to listen on
// returns char - the data received
inline char uart_get(uint8_t uart){
  char result = '\0';

  if(uart == 0){
    //wait until a byte has been received
    while((UCSR0A&(1<<RXC0)) == 0) {};

    //get received data
    result = UDR0;
  }
  else if(uart == 1){
    //wait until a byte has been received
    while((UCSR1A&(1<<RXC1)) == 0) {};

    //get received data
    result = UDR1;
  }

  return result;
}

//prints a nibble (4 bits) in hexadecimal
//  uint8_t uart - which uart to send on
//  uint8_t nibble - the nibble to print (only 4 lowest bits used)
void uart_print4(uint8_t uart, uint8_t nibble){
  nibble &= 0b00001111;

  if( nibble <= 0x09 ){
    uart_send(uart, (nibble + 0x30));
  } else {
    uart_send(uart, ((nibble - 0x0A) + 0x41));
  }
}

//prints a byte (8 bits) in hexadecimal
//  uint8_t uart - which uart to send on
//  uint8_t val - the byte to print
void uart_print8(uint8_t uart, uint8_t val){
  uart_print4(uart, val >> 4);
  uart_print4(uart, val);
}

//prints a "word" (16 bits) in hexadecimal
//  uint8_t uart - which uart to send on
//  uint16_t val - the "word" to print
void uart_print16(uint8_t uart, uint16_t val){
  uart_print8(uart, val >> 8);
  uart_print8(uart, val);
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
//  const char* PROGMEM data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_print_p(uint8_t uart, const char* PROGMEM data, uint16_t maxlen){
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

//uart_print_ps a null-terminated string in progmem followed by CR and LF chars
//  uint8_t uart - which uart to send on
//  const char* PROGMEM data - the string to be sent
//  uint16_t maxlen - the maximum number of characters to be sent
void uart_println_p(uint8_t uart, const char* PROGMEM data, uint16_t maxlen){
  //print the string
  uart_print_p(uart, data, maxlen);
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
