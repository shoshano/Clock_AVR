/*
   I2C.h

    Created on: 19-08-2013
        Author: Wojtek
*/

#ifndef I2C_H_
#define I2C_H_
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>


/**********************************    I2C speed   ************************************/
#define I2C_SPEED 100000L     //100kHz normal mode
//#define I2C_SPEED 400000L   //400kHz fast mode
class I2C {
  private:
    static void rep_start(uint8_t adress);
    static void stop(void);

    static uint8_t read(uint8_t ack);

  public:
    static void init();
    static void beginTransmission(uint8_t address);
    static void beginTransmissionRead(uint8_t address);
    static void endTransmission();

    static uint8_t readAck();
    static uint8_t readNak(void);			//Konczy odczytywanie
    static void write(uint8_t data);

    static uint8_t readFrom(uint8_t address);
    static uint8_t readFromReg(uint8_t address, uint8_t registerR);

    static void writeToReg(uint8_t address, uint8_t registerW, uint8_t val);

    static uint8_t checkAddress(uint8_t address);

};

/*
   Read
  I2C::beginTransmission(EEPROM_ADRESS);
  I2C::write(addressRead);
  I2C::endTransmission();
  I2C::beginTransmissionRead(EEPROM_ADRESSs)
  val1=I2C::readAck();
  val2=I2C::readAck();
  val3= I2C::readNak();			//Ko�czy transmisje




   Write
  I2C::beginTransmission(EEPROM_ADRESS);
  I2C::write(addressWrite;
  I2C::write(date >> 8);
  I2C::write(date & 0xff);
  I2C::endTransmission();

*/

/* Kod do skanowania podlaczonych urzadzen
  char text[16];
  for (uint8_t i = 0; i < 127; i++) {
  if (I2C::checkAddress(i) == 0) {
  sprintf(text, "%d\r\n", i);
  UART0::sendText(text);
  }
  _delay_ms(5);
  }
*/

#endif /* I2C_H_ */
