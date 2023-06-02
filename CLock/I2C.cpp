/*
   I2C.cpp

    Created on: 19-08-2013
        Author: Wojtek
*/

#include "I2C.h"

#define START 		0x08
#define START_REP 	0x10
#define MT_SLA_ACK	0x18
#define MT_DATA_ACK 	0x28
#define MR_SLA_ACK  	0x40

void I2C::init() {
  TWSR = 0;                                   // no prescaler => prescaler = 1
  TWBR = ((F_CPU / I2C_SPEED) - 16) / 2;       // change the I2C clock rate
  TWCR = 1 << TWEN;                         // enable twi module, no interrupt
}

void I2C::beginTransmission(uint8_t address) {
  I2C::rep_start(address << 1);
}

void I2C::beginTransmissionRead(uint8_t address) {
  I2C::rep_start((address << 1) | 1);
}

void I2C::endTransmission() {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void I2C::rep_start(uint8_t address) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // send REPEAT START condition
  while (!(TWCR & (1 << TWINT)))
    ;               // wait until transmission completed
  TWDR = address;                              // send device address
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;               // wail until transmission completed
}

uint8_t I2C::read(uint8_t ack) {
  TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0);
  while (!(TWCR & (1 << TWINT)))
    ;
  uint8_t r = TWDR;
  if (!ack)
    I2C::endTransmission();
  return r;
}

uint8_t I2C::readAck() {
  return I2C::read(1);
}

uint8_t I2C::readNak(void) {
  return I2C::read(0);
}

void I2C::write(uint8_t data) {
  TWDR = data;                 // send data to the previously addressed device
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

void I2C::writeToReg(uint8_t address, uint8_t registerWrite, uint8_t val) {
  beginTransmission(address);
  write(registerWrite);        // register selection
  write(val);        // value to write in register
  endTransmission();
}

uint8_t I2C::readFrom(uint8_t address) {
  I2C::rep_start((address << 1) | 1);
  return I2C::read(0);
}

uint8_t I2C::readFromReg(uint8_t address, uint8_t registerRead) {
  I2C::rep_start(address << 1);
  I2C::write(registerRead);
  I2C::rep_start((address << 1) | 1);
  return I2C::read(0);
}

/*
   return:
  	0	-	wykryto urzadzenie
  	1-2 - 	blad
*/
uint8_t I2C::checkAddress(uint8_t address) {
  uint8_t error = 0;
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // send REPEAT START condition
  //while (!(TWCR & (1 << TWINT)))
  //	;               // wait until transmission completed
  if ((TWSR & 0xF8) != START)
    error = 1;
  TWDR = address << 1;                              // send device address
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;               // wail until transmission completed
  if ((TWSR & 0xF8) != MT_SLA_ACK)
    error = 2;

  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

  return error;
}
