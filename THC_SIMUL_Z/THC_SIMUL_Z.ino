/*
 *  Lecture des sorties moteur Z : PULSE & DIR sur broches 4 & 7
 *  pour Arduino Uno
 *  utilise les interruptions externes pour lire la broche 2
 *  
*/

// Entrees - sorties
// *** ATTENTION *** A4 et A5 utilisees par MCP4725 pour I2C

const byte PULSE_Z_PIN   = 2;
const byte DIR_Z_PIN     = 3;
const byte Fonction_PIN  = 4;
const byte SWITCH_PIN    = 5;
const byte COOLANT_PIN   = 6;

const byte Reglage_PIN   = A0;  // Potentiometre

volatile uint8_t mov     = 0;
volatile long pos_Z      = 0;

uint32_t Vout   = 0;
int32_t Vdelta  = 0;

bool etat_COOLANT       = HIGH;
uint32_t delay_switch   = 0;
bool waiting            = LOW;
byte retard_switch      = 500; // délai (non bloquant !) pour switch en ms

byte coef_shift, coef_shift2, tempo;
uint16_t offset;

int reg, reg2, nb_diff  = 0;

#include "sinus_DAC.h"


void _pulse_Z() {
      mov = 1;
      if (PIND & 1 << DIR_Z_PIN) pos_Z--;
      else pos_Z++;
}


void mov_Z() {

    reglage();

    if (mov) {
      mov = 0;
      float Z_mm = pos_Z*0.01875;
      float Z_DAC = pos_Z*2.4; // 2.4 unite_DAC / STEP
      Vdelta = Z_DAC;
      if (!etat_COOLANT) Vdelta = 0;
//      Serial.println(Z_mm);
//      Serial.println(Vdelta);
//      delay(1); // limiter le nombre de sorties de Serial
    }
    if (millis() > delay_switch) {
      waiting = LOW;
    }
    if (!waiting) {
  	  if (!digitalRead(SWITCH_PIN)) {
        waiting = HIGH;
        delay_switch = millis() + retard_switch;
    		etat_COOLANT = !etat_COOLANT;
    		digitalWrite (COOLANT_PIN, etat_COOLANT);
    		Vdelta = 0;
  	  }
    }

}

void reglage(){

  reg2 = analogRead(Reglage_PIN);
  
  if (reg2 != reg) nb_diff++;
  else if (nb_diff > 1) nb_diff--;
  
  if (nb_diff > 100  ) {
	  nb_diff = 0;
	  reg = reg2;
	  coef_shift2 = map(reg, 0, 1023, 0, 8);
    if (coef_shift2 != coef_shift) {
      coef_shift = coef_shift2;
      Serial.println(coef_shift);
    }
  }
  
}



void setup()
{
  Serial.begin(115200);
  pinMode( PULSE_Z_PIN, INPUT );
  pinMode( DIR_Z_PIN, INPUT );
  pinMode( Fonction_PIN, INPUT_PULLUP);
  pinMode( COOLANT_PIN, OUTPUT );
  pinMode( SWITCH_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PULSE_Z_PIN), _pulse_Z, RISING);
  
  setup_DAC();

  // coef_shift : coef diviseur pour amplitude maxi de la tension
  // valeurs calculees avec VCC = 5V (entre 4.8 et 5.3 possible en realite)
  // déplacement : 0.125 mm/Varc soit 6.25 mm/Vthc (ou 5 mm/Vthc si 0.1 mm/Varc)
  //
  //  0 => 4095 = 5     V :: 31.2  mm
  //  1 => 2048 = 2.5   V :: 15.6  mm
  //  2 => 1024 = 1.25  V ::  7.8  mm
  //  3 =>  512 = 0.625 V ::  3.9  mm
  //  4 =>  256 = 0.312 V ::  1.9  mm
  //  5 =>  128 = 0.156 V ::  1.0  mm
  //  6 =>   64 = 0.078 V ::  0.5  mm
  //  7 =>   32 = 0.039 V ::  0.25 mm
  //  8 =>   16 = 0.019 V ::  0.12 mm
  //  9 =>    8 = 0.009 V ::  0.06 mm
  // 10 =>    4 = 0.005 V ::  0.03 mm
  // 11 =>    2 = 0.002 V ::  0.01 mm

  coef_shift  =  map(analogRead(Reglage_PIN), 0, 255, 0, 11);

  offset      = 630; // tension mini (VCC = 4095)
                        // valeur VCC pour USB PC XPS-8930 : 5,08 V
                        // 1258 pour 1.56V <=> 78V au plasma (pour VCC = 5,08)

  tempo       = 5;      // tempo pour diminuer la fréquence du signal de sortie 
                        // corniere : 1.2s (tempo = 120) pour monter de 20mm à 1000 mm/min (DAC = +2560)
  Vout        = 0;

}

void loop() {

  uint32_t i=0;
  int k  = 1 ; // unite de pas pour boucle triangle
                // durée cycle selon k
                //   1 :: 1255 ms (maximum)
                //   2 ::  625 ms
                //   3 ::  420 ms
                //   4 ::  313 ms
                //   5 ::  250 ms
                //  10 ::  125 ms
                //  20 ::   63 ms
                //  50 ::   25 ms
                // 100 ::   13 ms

  if (k < 1)  k=1;  // faut pas deconner !

  //Serial.println(i);
  if (!digitalRead (Fonction_PIN)) {  // Sinus
      for (i = 0; i < 512; i = i + k) {
  		  for (uint16_t dt = 0; dt < tempo; dt++) {
    		  mov_Z();  
    		  Vout = Vdelta + (pgm_read_word(&(DACLookup_FullSine_9Bit[i])) >> coef_shift)+ offset;
    		  dac.setVoltage(Vout, false);
    		  }
      }
  }
  else {                           // Triangle
    for (i = k; i > 1 ; i = i+k)
    {
        mov_Z();

        Vout = Vdelta + offset + (i >> coef_shift);
        dac.setVoltage(Vout, false);
        if (i > (4095-k)) k=-k;
        //Serial.println(i);
        //delay(10);
        //delayMicroseconds(tempo);
    }
  }

} //FIN
