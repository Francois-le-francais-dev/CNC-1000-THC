#include "Arduino.h"
// Merci à Indrek !
// video : https://www.youtube.com/watch?v=m4EpTYaBBJ4&t=89s
// code : https://github.com/indrekluuk/code_examples/blob/master/InterruptPulseExample/InterruptPulseExample.ino

volatile int ocr_start = 0;

void init_TIMER1() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;  //initialize counter value to 0

  TCCR1B |= (1 << WGM12);  // CTC mode
  //TCCR1B |= (1 << CS11) | (1 << CS10);    // 64 prescaler
  //TCCR1B |= (1 << CS10);    // 1 prescaler
  TCCR1B |= (1 << CS11);  // 8 prescaler

  ocr_start = Ti * 2;  // remplacer par : ocr_start = TLong * 2;

  interrupts();
  i = 1;
  n = i;
}

void start_impuls() {
  OCR1A = ocr_start;
  TCNT1 = 0;                //initialize counter value to 0
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
}

void stop_impuls() {
  TIMSK1 &= ~(1 << OCIE1A);  // disable timer compare interrupt
}

ISR(TIMER1_COMPA_vect) {  // V04.03.04 - durée boucle 85 µs environ
  PORT_ZSTEP |= 1 << PN_ZSTEP;
  OCR1A = Ti * 2;
  // remplace "calcul_Ti()"
  Ti2 = Ti;
  if (m == 0) {        // pas en ralentissement
    if (Ti != Tmin) {  // vitesse < vitesse max : calculer le pas suivant
      n = i;
      Ti = Ti - (2 * Ti) / (4 * n + 1);  // Ti diminue => vitesse augmente
      i = i + 1;
      if (Ti < Tmin) {  // vitesse > vitesse max ?
        Ti = Tmin;      // ne pas depasser la vitesse max
        i = i - 1;      // = bloquer i et donc n pour calculer m = dernier pas atteint dans la rampe d'accélération
      }
    }
  } else {  // m != 0 : ralentissement
    n = 1 - i;
    Ti = Ti - (2 * Ti) / (4 * n + 1);  // Ti augmente => vitesse diminue
    i = i - 1;
    if (Ti > TLong) {  // vitesse < vitesse min ?
      Ti = TLong;
      i = 1;  // on est au premier pas normalement !
    }
  }
  PORT_ZSTEP &= ~(1 << PN_ZSTEP);
}
