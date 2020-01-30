void ZSTEP_TOGG() {
    PORTD ^= 1 << pinZSTEP;
} 

void ZSTEP_ON() {
    PORTD |= 1 << pinZSTEP;
} 

void ZSTEP_OFF() {
    PORTD &= ~(1 << pinZSTEP);
} 

void ZDIR_TOGG() {
    PORTD ^= 1 << pinZDIR;
} 

void ZDIR_ON() {
    //digitalWrite(pinZDIR,HIGH);
    PORTD |= 1 << pinZDIR;
}

void ZDIR_OFF() {
    //digitalWrite(pinZDIR,LOW);
    PORTD &= ~(1 << pinZDIR);
}

void LedL_ON() {
    PORTC |= 1 << ledPinL_B;
}

void LedL_OFF() {
    PORTC &= ~(1 << ledPinL_B);
}

void LedH_ON() {
    PORTC |= 1 << ledPinH_B;
}

void LedH_OFF() {
    PORTC &= ~(1 << ledPinH_B);
}

// lectures

byte etat_COOLANT(){
  return (PINC & 1 << pinCoolant_B);
}



/* pour mesures temps et debug */

void D2_TOGG(){
  PORTD ^= 1 << 2; // pin D2 inversee
}

void D2_OFF(){
  PORTD &= ~(1 << 2); // pin D2 à LOW
}

void D2_ON(){
  PORTD |= 1 << 2; // pin D2 à HIGH
}
