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

#ifndef __SPIRAM_H
#define __SPIRAM_H

//NOTE: the CS pin on the 23K256 must be brought low before every function
//  call and subsequently brought high after every funcion call. I don't do
//  that in this library so you'll have to do it yourself.

//sets the mode and enables or disables the hold pin
//  uint8_t mode - the access mode. valid values are:
//                 0 - byte mode
//                 1 - sequential mode
//                 2 - page mode
//  uint8_t enhold - if 1: disables the hold pin, 0 enables
void spiram_init(uint8_t mode, uint8_t enhold);

//write a memory location
// uint16_t addr - the address to write to
// uint8_t data - the data to write
void spiram_wrbyte(uint16_t addr, uint8_t data);

//read a memory location
// uint16_t addr - the address to read from
// returns uint8_t - the data read
uint8_t spiram_rdbyte(uint16_t addr);

//writes an array to an address
//  uint16_t startaddr - the address the first byte will be written to
//  const uint8_t* data - the array to be written
//  uint16_t length - the number of bytes to be written from the array
void spiram_wrseq(uint16_t startaddr, const uint8_t* data, uint16_t length);

//reads a portion of memory into an array
//  uint16_t startaddr - the address the first byte will be read from
//  uint8_t* data - the array to be written to
//  uint16_t length - the number of bytes to be read from memory
void spiram_rdseq(uint16_t startaddr, uint8_t* data, uint16_t length);

#endif
