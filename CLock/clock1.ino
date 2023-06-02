 #include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#include "LCDI2C.h"

#ifndef _BV      //Jets bitem
#define _BV(bit)        (1<<(bit))
#endif
#ifndef sbi     //Ustaw 1
#define sbi(reg,bit)    reg |= (_BV(bit))
#endif

#ifndef cbi     //Usatw 0
#define cbi(reg,bit)    reg &= ~(_BV(bit))
#endif

#ifndef tbi     //Zmien
#define tbi(reg,bit)    reg ^= (_BV(bit))
#endif

//========FUNKCJE=======================

void clockInit();            // ustawienie przerwania dla zegara
void interupt();             // incicjacja przerwania dla przycisków
void setALARM();             // ustawia godzine budzika
void startTIMER();
void setCLOCKandDATE();      // ustawia godzine na zegarze
void displayCLOCKandDATE(LCD_I2C lcd);  // wyswietla czas i date
void displayALARMandTIMER(LCD_I2C lcd);   // wyswietla dane do alarmu lub timer
void setBuzzer();             // wlacza buzzer
void showPosition();          // pokazuje zmieniana liczbe


//ZEGAR I DATA
int SEC = 53;      // pamiec dla sekund
int MIN = 30;      // pamiec dla minut
int HOU = 12;      // pamiec dla godzin
int DAY  = 7;      //pamiec dla dnia 
int MONTH = 6;     // pamiec miesieca
int YEAR = 2023;   // pamiec roku
int next = 0;       //zmiana pozycji

//BUDZIK

int ALARMMIN = 0; // pamiec dla minut budzika 
int ALARMHOU = 8; //pamiec dla godzin budzika


int ALARMSTSTUS = 0; //0 jest alarem jest wylaczony i 1 jelsi jest wlaczony
int choice = 0;      //słuzy do wyboru czy zmieniemy godziny czy minuty

//TIMER
int TIMERMIN = 0; // pamiec dla minut timera 
int TIMERSEC = 0; //pamiec dla sekund timera


int TIMERSTSTUS = 0; //0 jest timer jest wylaczony i 1 jelsi jest wlaczony
int TIMERSTART = 0;




//=============MAIN=====================================================================

int main(){

  //WEJSCIA
  cbi(DDRD,PD2);  // przycisk przerwanie INT0 do wyłaczania buzzera
  cbi(DDRD,PD3);  // przycisk przerwanie INT1 do ustawiania alarmu i timera
  cbi(DDRD,PD5);  // przycisk ++
  cbi(DDRD,PD6);  // przycisk --
  cbi(DDRD,PD7);  // przycisk startuje minutnik
  cbi(DDRB,PB0);  // ustawianie godziny i daty
  // WEJSCIA PULL-UP
  sbi(PORTD,PD3); 
  sbi(PORTD,PD2);
  sbi(PORTD,PD5);
  sbi(PORTD,PD6);
  sbi(PORTD,PD7);
  sbi(PORTB,PB0);
  //WYJSCIA
  sbi(DDRD, PD4); // buzzer
  cbi(PORTD,PD4); // ustawia stan niski

  //INICJUJE PRZERWANIA I ZEGAR
  clockInit();
  interupt();
  sei();    // zalacza zegar

  //EKRAN LCD
  LCD_I2C lcd = LCD_I2C(0x27); // sprawdzic port
  _delay_ms(100);
  lcd.backLightOn();

  char text1[20];
  
  lcd.goTo(0,0);
  sprintf(text1, "DZIEN DOBRY");
  lcd.writeText(text1);
  
  _delay_ms(2000);
  
  //PETLA
  while(1){
    lcd.clear();    //wyczysc ekran
    setALARM(); // jesli wybrane to ustaw budzik lub minutnik
    displayCLOCKandDATE(lcd); // wyswietla zegar
    displayALARMandTIMER(lcd);  // wyswietla alarm i timer
    startTIMER(); //jelsi wybrane wystartuj minutnik
    setBuzzer();  // jsli spełnione warunki wlacz buzzer  
  }
}

void setCLOCKandDATE(){
  if(bit_is_clear(PINB,PB0)){
      _delay_ms(30);
    if(bit_is_clear(PINB,PD0)){
      _delay_ms(20);
        next = (next+1)%6;
    } 
  }
  if(next == 1){      // zmiana godzin
    if(bit_is_clear(PIND,PD5)){
      _delay_ms(200);
      HOU = (HOU+1)%24;
    }
    else if(bit_is_clear(PIND,PD6)){
      _delay_ms(200);
      HOU--;
      if(HOU < 0){
        HOU = 23;
      }
    }
    
  }
  else if (next == 2 ){ //zmiana minut
    if(bit_is_clear(PIND,PD5) ){
      _delay_ms(200);
      MIN = (MIN+1)%60;
    }
    else if(bit_is_clear(PIND,PD6)){
      _delay_ms(200);
      MIN--;
      if(MIN < 0 ){
        MIN = 59;
      } 
    }
    
  }
  else if(next == 3){ // zmiana dnia
    if(bit_is_clear(PIND,PD5)){
      _delay_ms(200);
      DAY++;
      if(DAY > 31){
        DAY = 1;
      }
    }
    else if(bit_is_clear(PIND,PD6)){
      _delay_ms(200);
      DAY--;
      if(DAY < 1){
        DAY = 31;
      }
    }
  }
  else if(next == 4){ // zmiana miesiaca
    if(bit_is_clear(PIND,PD5)){
      _delay_ms(200);
      MONTH ++;
       if(MONTH > 12){
        MONTH = 1;
      }
    }
    else if(bit_is_clear(PIND,PD6)){
      _delay_ms(200);
      MONTH--;
      if(MONTH <1){
        MONTH = 12;
      }
    }
    
    }
  else if(next == 5 && bit_is_clear(PIND,PD5)){ // zmiana roku ++
     _delay_ms(200);
      YEAR++;
     }
  else if(next == 5 && bit_is_clear(PIND,PD6)){ // zmiana roku --
      _delay_ms(200);
      YEAR--;
      }
}


//ustawienie przerwania
void clockInit() {
      sbi(TCCR1B, WGM12);
      sbi(TCCR1B, CS12);
      OCR1A = 62500;
      sbi(TIMSK1, OCIE1A);
}


void interupt()
{
  sbi(EICRA, ISC01);
  sbi(EICRA, ISC11);
  sbi(EIMSK,INT0);
  sbi(EIMSK, INT1);
}




//======WYSWIETLANIE ZEGARA I DATY======
/*
GG:MM:SS___ST     13
DD.MM.YYYY_GG:MM  16
*/
void displayCLOCKandDATE(LCD_I2C lcd){
  char _clock[20];
  char _date[20];
  char _status[20];
  char _position[20];
  lcd.goTo(0,0);
  sprintf(_clock,"%02d:%02d:%02d", HOU, MIN, SEC);
  lcd.writeText(_clock);
  lcd.goTo(0,1);
  sprintf(_date,"%02d.%02d.%04d", DAY, MONTH, YEAR);
  lcd.writeText(_date);
  if(choice == 5){
    sprintf(_status, "H&D");
    switch(next){
      case 1:{
        sprintf(_position, "HOUR"); 
        break;
      }
      case 2:{
        sprintf(_position, "MIN"); 
        break;
      }
      case 3:{
        sprintf(_position, "DAY"); 
        break;
      }
      case 4:{
        sprintf(_position, "MONTH"); 
        break;
      }
      case 5:{
        sprintf(_position, "YEAR"); 
        break;
      }
      default:{
        sprintf(_position, " ");  
        break;
      }
    }
    lcd.goTo(11,0);
    lcd.writeText(_status);
    lcd.goTo(11,1);
    lcd.writeText(_position);
  }
}

//======WYSWITLANIE ALARMU I TIMERA======
/*
GG:MM:SS___ST     13
DD.MM.YYYY_GG:MM  16
*/
void displayALARMandTIMER(LCD_I2C lcd){
  char _alarm[20];
  char _ststus[20];
  //======SPARWDZA CZY JEST USTAWIONY BUDZIK======
  if(choice != 5){
    if(ALARMSTSTUS == 0 && TIMERSTSTUS == 0){
      sprintf(_ststus, "OFF");
      sprintf(_alarm, " ");
    }
    else if(ALARMSTSTUS == 1){
      sprintf(_ststus, "ALARM");
      sprintf(_alarm, "%02d:%02d", ALARMHOU, ALARMMIN);
      
    }
    else if(TIMERSTART == 1 || TIMERSTSTUS == 1){
      sprintf(_ststus, "TIMER");
      sprintf(_alarm, "%02d:%02d", TIMERMIN, TIMERSEC);
      
    }
    //======WYSWITLA BUDZIK LUB MINUTNIK======
    lcd.goTo(11,0);
    lcd.writeText(_ststus);
    lcd.goTo(11,1);
    lcd.writeText(_alarm);
  }
}

//======PRZERWANIE USTAWIAJACE DATE I GODZINE======
ISR(TIMER1_COMPA_vect) {
  SEC++;
  //======SEKUNDY======
  if(SEC > 59){
    SEC = 0;
    MIN++;
   //======MINUTY======
      if(MIN > 59){
        MIN = 0;
        HOU++;
        
    //======GODZINY======
          if(HOU > 23){
            HOU = 0;
            DAY++;
            
     //======DNI I MIECIACE WRAZ Z LATAMI PRZESTEPNYMI======
            if((MONTH == 4 || MONTH == 6 || MONTH == 9 || MONTH == 11) && DAY >= 31){ // miesiece 30 dni
              MONTH++;
              DAY = 1;
            }
            else if( MONTH != 2 && DAY >= 32){  // pozostele 31 dniowe poza lutym
              MONTH++;
              DAY = 1;
            }
            else if(MONTH == 2 && DAY >= 30 && ((YEAR%4 == 0 && YEAR%100 !=0) || YEAR%400 == 0) ){  //rok przestepny
              MONTH = 3;
              DAY = 1;
            }
            else if(MONTH == 2 && DAY >= 29){   // zwykly luty
              MONTH = 3;
              DAY = 1;
            }
      //======ROK======
            if(MONTH > 13){
              MONTH = 1;
              YEAR++;
              }
            }
          }    
        }
  if(TIMERSTART == 1){
    TIMERSEC--;
    if (TIMERSEC < 0){
      TIMERSEC = 59;
      TIMERMIN--;
      if(TIMERMIN < 0){
            TIMERMIN = 0;
            TIMERSEC = 0;
       }
     }
   }
}




//======USTAWIA BUDZIK I MINUTNIK======

void setALARM(){
  //======USTAWIA MINUTY BUDZIKA======
  if(choice == 1){
    if(bit_is_clear(PIND,PD5)){ //guzik do zmiany czasu 
      ALARMMIN = (ALARMMIN+1)%60;
      _delay_ms(300); 
    }
    else if(bit_is_clear(PIND,PD6)){
      ALARMMIN--;
      _delay_ms(300); 
      if(ALARMMIN <0){
        ALARMMIN = 59;
      }
    }
    ALARMSTSTUS = 1;
    TIMERSTSTUS = 0;
  }
//======USTAWIA GODZINY BUDZIKA======
  else if(choice == 2){
    if(bit_is_clear(PIND,PD5)){ //guzik do zmiany czasu 
      ALARMHOU = (ALARMHOU +1)%24;
      _delay_ms(300); 
    }
    else if(bit_is_clear(PIND,PD6)){
      ALARMHOU--;
      _delay_ms(300); 
      if(ALARMHOU <0){
        ALARMHOU = 23;
      }
    }
    ALARMSTSTUS = 1;
    TIMERSTSTUS = 0;
  }

  //======USTAWIA MINUTNIK======
  else if(choice == 3){
    if(bit_is_clear(PIND,PD5)){
      TIMERMIN++;
      _delay_ms(300);
      if(TIMERMIN > 99){
        TIMERMIN = 0;
      }
    }
    else if(bit_is_clear(PIND,PD6)){
      TIMERMIN--;
      _delay_ms(300); 
      if(ALARMMIN <0){
        ALARMMIN = 99;
      }
    }
    ALARMSTSTUS = 0;
    TIMERSTSTUS = 1;
    TIMERSTART = 0;
  }
  else if(choice == 4){
    ALARMSTSTUS = 0;
    TIMERSTSTUS = 1;
  }
  else if(choice == 5){   // ustwianie zegra i daty
    
    setCLOCKandDATE();
    ALARMSTSTUS = 0;
    TIMERSTSTUS = 0;
    TIMERSTART = 0;
  }
  else{
    ALARMSTSTUS = 0;
    TIMERSTSTUS = 0;
    TIMERSTART = 0;
    next = 0;
  }
}

//======START MINUTNIKA======

void startTIMER(){
  if(bit_is_clear(PIND,PD7)){
      TIMERSTART = 1;  
      _delay_ms(300);
    } 
}




//======WLACZA BUZZER======
void setBuzzer(){
  if(ALARMSTSTUS == 1){
    if(HOU == ALARMHOU && MIN == ALARMMIN){
      tbi(PORTD,PD4);
      _delay_ms(1000); 
      tbi(PORTD,PD4);
      _delay_ms(1000);
    }
  }
  if(TIMERSTART == 1 &&(TIMERMIN == 0 && TIMERSEC == 0)){
      tbi(PORTD,PD4);
      _delay_ms(1000); 
      tbi(PORTD,PD4);
      _delay_ms(1000);
  }
}









//======PRZERWANIE WYLACZA BUZER======

ISR(INT0_vect){
  _delay_ms(200);
  cbi(PORTD,PD4); // tu przypisac port buzzera
  ALARMSTSTUS = 0; //wyłacza budzik
  TIMERSTSTUS = 0;
  TIMERSTART = 0;
  
  choice = 0;
  next = 0;
  ALARMHOU = 8;
  ALARMMIN= 0;
}

//======PRZEWRWANIE DO USTAWIANIA BUDZIKA======
ISR(INT1_vect){
  _delay_ms(30);
  if(bit_is_clear(PIND,PD3)){
    _delay_ms(20);
      choice++;
  }
  if (choice >5){
    choice = 0;
  }
}
