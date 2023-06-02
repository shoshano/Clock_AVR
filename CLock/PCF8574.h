/*
   PCF8574.h

    Created on: 19-08-2013
        Author: Wojtek
*/

#ifndef PCF8574_H_
#define PCF8574_H_
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "I2C.h"

class PCF8574 {
  public:
    PCF8574();
    PCF8574(uint8_t address);
    void setPin(uint8_t pin);
    void clearPin(uint8_t pin);
    void setPort(uint8_t port);

    uint8_t readPin(uint8_t pin);
    uint8_t readALLPin();

  private:
    uint8_t portState;
    uint8_t adress;
    void updatePCF();
};

#endif /* PCF8574_H_ */
