#include "Arduino.h"
/*
  Ports différents sur MEGA !!

	https://docs.arduino.cc/retired/hacking/software/PortManipulation/

Each port is controlled by three registers, which are also defined variables in the arduino language.
 The DDR register, determines whether the pin is an INPUT or OUTPUT.
  The PORT register controls whether the pin is HIGH or LOW, and
   the PIN register reads the state of INPUT pins set to input with pinMode().

DDR and PORT registers may be both written to, and read.
PIN registers correspond to the state of inputs and may only be read.

PORTD maps to Arduino MEGA digital pins 18 to 21

DDRD - The Port D Data Direction Register - read/write

PORTD - The Port D Data Register - read/write

PIND - The Port D Input Pins Register - read only

*/

void TORCHE_OFF(){
    digitalWrite(relais_coupe_Pin, HIGH);  // HIGH = arrêt relais torche
}

void TORCHE_ON(){
    digitalWrite(relais_coupe_Pin, LOW);  // LOW = marche relais torche
}


void START_ARC_OFF() {
  digitalWrite(relais_start_arc_Pin, HIGH);
}


void START_ARC_ON() {
  digitalWrite(relais_start_arc_Pin, LOW);
}


// ZSTEP
void ZSTEP_TOGG() {
  PORT_ZSTEP ^= 1 << PN_ZSTEP;
}


void ZSTEP_ON() {
  PORT_ZSTEP |= 1 << PN_ZSTEP;
}

void ZSTEP_OFF() {
  PORT_ZSTEP &= ~(1 << PN_ZSTEP);
}

// DIR Z : 1 = DESCENTE / 0 = MONTEE
void ZDIR_ON() {
  PORT_ZDIR |= 1 << PN_ZDIR;
}

void ZDIR_OFF() {
  PORT_ZDIR &= ~(1 << PN_ZDIR);
}

// ******* Leds *******

void LedL_ON() {
  // PORTA
  PORT_ledL |= 1 << PN_ledL;  // led Bas ON
  // PORTA |= 1 << PORTA2;     // led Bas ON
}

void LedL_OFF() {
  // PORTA
  PORT_ledL &= ~(1 << PN_ledL);  // led Bas OFF
}

void LedH_ON() {
  PORT_ledH |= 1 << PN_ledH;  // led Haut ON
}

void LedH_OFF() {
  PORT_ledH &= ~(1 << PN_ledH);  // led Haut OFF
}

void LedHH_ON() {
  PORT_ledHH |= 1 << PN_ledHH;  // led HH ON
}

void LedHH_OFF() {
  PORT_ledHH &= ~(1 << PN_ledHH);  // led HH OFF
}

void LedOK_ON() {
  PORT_ledOK |= 1 << PN_ledOK;  // led Ok ON
}

void LedOK_OFF() {
  PORT_ledOK &= ~(1 << PN_ledOK);  // led Ok OFF
}


// 241014 - sortie PAUSE GRBL
// 241024 *** MEGA ***
// --------------------------
void led_THC_ON() {
  // PORT_led_THC |= (1 << PN_led_THC);
  digitalWrite(ledPin_THC, HIGH);
}

void led_THC_OFF() {
  // PORT_led_THC &= ~(1 << PN_led_THC);  // PORTB7 sur MEGA
  digitalWrite(ledPin_THC, LOW);
}

void GRBL_pause() {
  digitalWrite(GRBL_pause_Pin, LOW);  // sortie niveau BAS
  display_Varc.clear();
  lcd.clear();
  delay(50);                           // tempo pour GRBL (DEBOUNCE 40 ms dans config.h)
  digitalWrite(GRBL_pause_Pin, HIGH);  // relacher
}

void voyant_defaut_OFF()
{
  digitalWrite(pinDefaut_THC, HIGH);
}

void voyant_defaut_ON()
{
  digitalWrite(pinDefaut_THC, LOW);
}



//--------------------------



// lectures

/* inutilisé
byte etat_COOLANT() {
  return (PIN_Coolant && (1 << PN_Coolant));  //
}
*/


/* pour mesures temps et debug */


void DEBUG_TOGG() {
  if (Pin_debug_Pin && (1 << PN_debug_Pin)) {  // DEBUG = HIGH
    PORT_debug_Pin &= ~(1 << PN_debug_Pin);    // pin DEBUG à LOW
  } else {
    PORT_debug_Pin |= 1 << PN_debug_Pin;  // pin DEBUG à HIGH
  }
}


void DEBUG_OFF() {
  PORT_debug_Pin &= ~(1 << PN_debug_Pin);  // pin DEBUG à LOW
}

void DEBUG_ON() {
  // digitalWrite(23,HIGH);
  PORT_debug_Pin |= 1 << PN_debug_Pin;  // pin DEBUG à HIGH
}
