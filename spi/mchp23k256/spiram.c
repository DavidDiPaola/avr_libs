#include <avr/io.h>
#include <avr-libs/spi/spi.h>
#include <avr-libs/spi/mchp23k256/spiram.h>

//NOTE: the CS pin on the 23K256 must be brought low before every function
//  call and subsequently brought high after every funcion call. I don't do
//  that in this library so you'll have to do it yourself.

//instructions
const uint8_t __WRITE_INST = 0b00000010; //write to memory
const uint8_t __READ_INST  = 0b00000011; //read from memory
const uint8_t __WRSR_INST  = 0b00000001; //write STATUS register
const uint8_t __RDSR_INST  = 0b00000101; //read STATUS register
const uint8_t __NULL_INST  = 0b11111111; //invalid/do nothing instruction
//modes of operation
const uint8_t __MODE_MASK = 0b11000000;
const uint8_t __MODE_BITS = 6; //mode bits start at bit 6
//hold pin
const uint8_t __HOLD_MASK = 0b00000001;

//sets the mode and enables or disables the hold pin
//  uint8_t mode - the access mode. valid values are:
//                 0 - byte mode
//                 1 - sequential mode
//                 2 - page mode
//  uint8_t enhold - if 1: disables the hold pin, 0 enables
void spiram_init(uint8_t mode, uint8_t enhold){
  spi_send(__WRSR_INST); //we want to write to the status register
  spi_send( ((mode<<__MODE_BITS)&__MODE_MASK) | (enhold&__HOLD_MASK) );
}

//write a memory location
// uint16_t addr - the address to write to
// uint8_t data - the data to write
void spiram_wrbyte(uint16_t addr, uint8_t data){
  //send write instruction
  spi_send(__WRITE_INST);
  //send address
  spi_send(addr>>8); //most significant byte
  spi_send(addr); //least significant byte
  //send data
  spi_send(data);
}

//read a memory location
// uint16_t addr - the address to read from
// returns uint8_t - the data read
uint8_t spiram_rdbyte(uint16_t addr){
  uint8_t result = 0x00;

  //send read instruction
  spi_send(__READ_INST);
  //send address
  spi_send(addr>>8); //most significant byte
  spi_send(addr); //least significant byte
  //read data
  result = spi_send(__NULL_INST); //send clock pulses, get result

  return result;
}

//writes an array to an address
//  uint16_t startaddr - the address the first byte will be written to
//  const uint8_t* data - the array to be written
//  uint16_t length - the number of bytes to be written from the array
void spiram_wrseq(uint16_t startaddr, const uint8_t* data, uint16_t length){
  uint16_t i;

  //send the write instruction
  spi_send(__WRITE_INST);
  //send address
  spi_send(startaddr>>8);
  spi_send(startaddr);
  //send data
  for(i=0; i<length; i++){
    spi_send(data[i]);
  }
}

//reads a portion of memory into an array
//  uint16_t startaddr - the address the first byte will be read from
//  uint8_t* data - the array to be written to
//  uint16_t length - the number of bytes to be read from memory
void spiram_rdseq(uint16_t startaddr, uint8_t* data, uint16_t length){
  uint16_t i;

  //send the read instruction
  spi_send(__READ_INST);
  //send address
  spi_send(startaddr>>8);
  spi_send(startaddr);
  //read in data
  for(i=0; i<length; i++){
    data[i] = spi_send(__NULL_INST);
  }
}
