#include "Arduino.h"
/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

void aff_version() {

  LedOK_ON();
  LedH_ON();
  LedL_ON();

  char mesg[20];
  strcpy(mesg, "* THC  Version *");
  //------------XXXXXXXXXXXXXXXX  16 char
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mesg);
  strcpy(mesg, firm_ver);
  //------------XXXXXXXXXXXXXXXX  16 char
  lcd.setCursor(0, 1);
  lcd.print(mesg);
  delay(2000);

  LedOK_OFF();
  LedH_OFF();
  LedL_OFF();
}


 void aff_pause() {

  LedOK_ON();
  LedH_ON();
  LedL_ON();

  char mesg[20];
  // Ligne 1
  strcpy(mesg, "XX PAUSE GRBL XX");
  //------------XXXXXXXXXXXXXXXX  16 char
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mesg);
  // Ligne 2  
  strcpy(mesg, "****************");
  //------------XXXXXXXXXXXXXXXX  16 char
  lcd.setCursor(0, 1);
  lcd.print(mesg);

  delay(300);

  LedOK_OFF();
  LedH_OFF();
  LedL_OFF();
}

bool aff_varL1(char message[20], unsigned int &param) {

    unsigned int val_param = param;  
    //lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print (message);

    //lcd.setCursor(0, 1);
    lcd.print (val_param);

    return (true);
}

bool aff_varL2(char message[20], unsigned int &param) {

    unsigned int val_param = param;  
    //lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print (message);

    //lcd.setCursor(0, 1);
    lcd.print (val_param);

    return (true);
}

bool lcd_msg(char message[20]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print (message);
    delay(1000);
    return (true);
}

bool debug_msg(char message[20]){}

/*bool debug_msg(char message[20]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print (message);
    delay(800);
    return (true);
}*/
