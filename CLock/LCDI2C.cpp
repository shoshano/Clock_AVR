/*
   LCDI2C.cpp

    Created on: 12-02-2014
        Author: Wojtek
*/

#include "LCDI2C.h"

//-------------------------------------------------------------------------------------------------
//
// Konfiguracja sygna��w steruj�cych wy�wietlaczem.
// Mo�na zmieni� stosownie do potrzeb.
//
//-------------------------------------------------------------------------------------------------
#define RS			0
#define RW			1
#define EN			2
#define DB4			4
#define DB5			5
#define DB6			6
#define DB7			7
#define BACKLIGHT 	3

//-------------------------------------------------------------------------------------------------
//
// Instrukcje kontrolera Hitachi HD44780
//
//-------------------------------------------------------------------------------------------------

#define HD44780_CLEAR					0x01

#define HD44780_HOME					0x02

#define HD44780_ENTRY_MODE				0x04
#define HD44780_EM_SHIFT_CURSOR		0
#define HD44780_EM_SHIFT_DISPLAY	1
#define HD44780_EM_DECREMENT		0
#define HD44780_EM_INCREMENT		2

#define HD44780_DISPLAY_ONOFF			0x08
#define HD44780_DISPLAY_OFF			0
#define HD44780_DISPLAY_ON			4
#define HD44780_CURSOR_OFF			0
#define HD44780_CURSOR_ON			2
#define HD44780_CURSOR_NOBLINK		0
#define HD44780_CURSOR_BLINK		1

#define HD44780_DISPLAY_CURSOR_SHIFT	0x10
#define HD44780_SHIFT_CURSOR		0
#define HD44780_SHIFT_DISPLAY		8
#define HD44780_SHIFT_LEFT			0
#define HD44780_SHIFT_RIGHT			4

#define HD44780_FUNCTION_SET			0x20
#define HD44780_FONT5x7				0
#define HD44780_FONT5x10			4
#define HD44780_ONE_LINE			0
#define HD44780_TWO_LINE			8
#define HD44780_4_BIT				0
#define HD44780_8_BIT				16

#define HD44780_CGRAM_SET				0x40

#define HD44780_DDRAM_SET				0x80

/*
   LCD.cpp

    Created on: 12-10-2013
        Author: Wojtek
*/

//-------------------------------------------------------------------------------------------------
// Wy�wietlacz alfanumeryczny ze sterownikiem HD44780
// Sterowanie w trybie 4-bitowym bez odczytu flagi zaj�to�ci
// z dowolnym przypisaniem sygna��w steruj�cych
// Plik : HD44780.c
// Mikrokontroler : Atmel AVR
// Kompilator : avr-gcc
// Autor : Rados�aw Kwiecie�
// �r�d�o : http://radzio.dxp.pl/hd44780/
// Data : 24.03.2007
//-------------------------------------------------------------------------------------------------
LCD_I2C::LCD_I2C() {

}

//-------------------------------------------------------------------------------------------------
//
// Procedura inicjalizacji kontrolera HD44780.
//
//-------------------------------------------------------------------------------------------------
LCD_I2C::LCD_I2C(uint8_t address) {
  this->pcf8574 = PCF8574(address);
  init();
}

void LCD_I2C::init() {

  char tab1[] = { 0, 0, 14, 1, 15, 17, 15, 2 }; //�
  char tab2[] = { 0x0c, 0x04, 0x06, 0x0c, 0x04, 0x04, 0x0e, 0x00 }; //�
  char tab3[] = { 0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0e, 0x02 }; //�
  char tab4[] = { 0x01, 0x02, 0x0f, 0x10, 0x10, 0x10, 0x0f, 0x00 }; //c

  writeZnak(tab1, 0);
  _delay_ms(10);
  writeZnak(tab2, 8);
  _delay_ms(10);
  writeZnak(tab3, 16);
  _delay_ms(10);
  writeZnak(tab4, 24);
  _delay_ms(10);

  this->pcf8574.clearPin(RS); // wyzerowanie linii RS
  this->pcf8574.clearPin(EN);  // wyzerowanie linii E

  for (uint8_t i = 0; i < 3; i++) // trzykrotne powt�rzenie bloku instrukcji
  {
    this->pcf8574.setPin(EN); //  E = 1
    outNibble(0x03); // tryb 8-bitowy
    this->pcf8574.clearPin(EN); // E = 0
    _delay_ms(5); // czekaj 5ms
  }

  this->pcf8574.setPin(EN); // E = 1
  outNibble(0x02); // tryb 4-bitowy
  this->pcf8574.clearPin(EN); // E = 0

  _delay_ms(1); // czekaj 1ms
  writeCommand(
    HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
  writeCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wy��czenie wyswietlacza
  writeCommand(HD44780_CLEAR); // czyszczenie zawartos�i pamieci DDRAM
  _delay_ms(2);
  writeCommand(
    HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT); // inkrementaja adresu i przesuwanie kursora
  writeCommand(
    HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF
    | HD44780_CURSOR_NOBLINK); // w��cz LCD, bez kursora i mrugania
}

//-------------------------------------------------------------------------------------------------
//
// Funkcja wystawiaj�ca p�bajt na magistral� danych
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::outNibble(unsigned char nibbleToWrite) {
  if (nibbleToWrite & 0x01)
    this->pcf8574.setPin(DB4);
  else
    this->pcf8574.clearPin(DB4);

  if (nibbleToWrite & 0x02)
    this->pcf8574.setPin(DB5);
  else
    this->pcf8574.clearPin(DB5);

  if (nibbleToWrite & 0x04)
    this->pcf8574.setPin(DB6);
  else
    this->pcf8574.clearPin(DB6);

  if (nibbleToWrite & 0x08)
    this->pcf8574.setPin(DB7);
  else
    this->pcf8574.clearPin(DB7);

}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu bajtu do wy�wietacza (bez rozr�nienia instrukcja/dane).
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::write(unsigned char dataToWrite) {
  this->pcf8574.setPin(EN);
  outNibble(dataToWrite >> 4);
  this->pcf8574.clearPin(EN);
  this->pcf8574.setPin(EN);
  outNibble(dataToWrite);
  this->pcf8574.clearPin(EN);
  _delay_us(50);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu rozkazu do wy�wietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::writeCommand(unsigned char commandToWrite) {
  this->pcf8574.clearPin(RS);
  write(commandToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja zapisu danych do pami�ci wy�wietlacza
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::writeData(unsigned char dataToWrite) {
  this->pcf8574.setPin(RS);
  write(dataToWrite);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja wy�wietlenia napisu na wyswietlaczu.
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::writeText(char * text) {
  while (*text)
    writeData(*text++);
}


//-------------------------------------------------------------------------------------------------
//
// Funkcja ustawienia wsp�rz�dnych ekranowych
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::goTo(unsigned char x, unsigned char y) {

  unsigned char t = 0;

  if (y & 0x02)
    t |= 20;
  if (y & 0x01)
    t |= 0x40;
  writeCommand((HD44780_DDRAM_SET | x) + t);

}
//-------------------------------------------------------------------------------------------------
//
// Funkcja czyszczenia ekranu wy�wietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::clear(void) {
  writeCommand(HD44780_CLEAR);
  _delay_ms(2);
}
//-------------------------------------------------------------------------------------------------
//
// Funkcja przywr�cenia pocz�tkowych wsp�rz�dnych wy�wietlacza.
//
//-------------------------------------------------------------------------------------------------
void LCD_I2C::home(void) {
  writeCommand(HD44780_HOME);
  _delay_ms(2);
}

void LCD_I2C::writeZnak(char * znak, uint8_t address) {
  writeCommand(0x40 | address);
  for (uint8_t i = 0; i < 8; i++) {
    writeData(znak[i]);
  }
  writeCommand(0x80);
}

void LCD_I2C::backLightOn() {
  this->pcf8574.setPin(BACKLIGHT);
}

void LCD_I2C::backLightOff() {
  this->pcf8574.clearPin(BACKLIGHT);
}
