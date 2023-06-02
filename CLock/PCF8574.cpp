/*
   PCF8574.cpp

    Created on: 19-08-2013
        Author: Wojtek
*/

#include "PCF8574.h"

#ifndef _BV
#define _BV(bit)        (1<<(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)    reg |= (_BV(bit))
#endif


#ifndef cbi
#define cbi(reg,bit)    reg &= ~(_BV(bit))
#endif

PCF8574::PCF8574() {
  portState = 0;
  adress = 0;
}

PCF8574::PCF8574(uint8_t address) {
  // TODO Auto-generated constructor stub
  this->adress = address;
  this->portState = 0;
  I2C::init();
}

/*
   Ustawia dany pin w stan wysoki
*/
void PCF8574::setPin(uint8_t pin) {
  sbi(this->portState, pin);
  I2C::beginTransmission(this->adress);
  I2C::write(this->portState);
  I2C::endTransmission();
}

/*
   Ustawia dany pin w stan niski
*/

void PCF8574::clearPin(uint8_t pin) {
  cbi(this->portState, pin);
  I2C::beginTransmission(this->adress);
  I2C::write(this->portState);
  I2C::endTransmission();
}

/*
   Ustawia caly port zgodnie z przekazana wartoscia
*/
void PCF8574::setPort(uint8_t port) {
  this->portState = port;
  I2C::write(this->portState);
}

/*
   Zwraca wartosc logiczna jaki stan logiczny wystepuje na podanym wejsciu
   zwraca 1 - stan wysoki
   zwraca 0 - stan niski
*/
uint8_t PCF8574::readPin(uint8_t pin) {
  return (I2C::readFrom(adress) & (1 << pin)) ? 1 : 0;
}

/*
   Zwraca wartosc zgodnie z wartosciami logicznymi panujacymi na porcie
*/
uint8_t PCF8574::readALLPin() {
  return I2C::readFrom(adress);
}
