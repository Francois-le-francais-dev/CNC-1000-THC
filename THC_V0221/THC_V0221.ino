/*
   V02     adcval => long
           ajout condition dans ISR : ne marche pas
           lire_ADC en dehors de l'ISR : ne marche pas
           void auto_trigger_on () : fonction complete pour redemarrer l'ISR => ok !
           else if (ordre == sens_actuel && (m == 0)) condition rajoutee sur m => ok !!
           ledPinH / ledPinL inverses !
           lire_Serial finalise
           ecriture dans EEPROM
           correction lire_Serial
   V02.05  Suppression des commandes Arduino 'String' pour eviter les risques qu'elles comportent
              voir : https://electroniqueamateur.blogspot.com/2019/10/texte-et-arduino-1-la-classe-string.html
   V02.06  Ajout des commandes de lecture de la roue codeuse pour entrer les parametres machine
   V02.07
   V02.08  Inversion ZDIR - tempo init_THC() à 1000 ms
   V02.09  stop => pinZDIR à LOW
   V02.09b limite du nb d'echantillons augmente a 10 000 et freq. echantillonage a 153.8 kHz
            => test ok
   V02.10  digitalWrite() => PORTC, PORTD (cf IN_OUT.h)
   V02.11  correction demi-hauteur : cf fonction move_torch()
   V02.11c adapté pour simulation avec "SIMUL_Z.ino"
   V02.20  en cours ...
           -  afficher la tension mesurée / si TMax assez grand (> 800 µs)
              / ou si chrono le permet ?
           -  changer la tension de réf. pendant la coupe ?
	V02.21  - modif boucle move_torch() pour affichage tension pendant arrets moteur
          - ajout affichage version au démarrage : fonction aff_version()

  *********************************************************************************************
  Documentations sur ADC Arduino :
  https://fr.wikiversity.org/wiki/Micro_contrôleurs_AVR/La_conversion_analogique_numérique
  https://www.youtube.com/watch?v=5ky_N5-Fu6M&t=814s
  http://www.f-legrand.fr/scidoc/docmml/sciphys/arduino/adcrapide/adcrapide.html
  https://www.embedded.com/generate-stepper-motor-speed-profiles-in-real-time/
*/

/*
   PB non résolu : si la torche rencontre un trou dans la tôle elle va plonger indéfiniment ..
   et risque de heurter l'autre bord du trou !
*/

//#define SIMUL_MODE    // définir le mode simulation

#include <EEPROM.h>
#include "SevenSegmentTM1637.h"
#include <LiquidCrystal.h>

#define auto_trigger_off ADCSRA &= ~(1 << ADATE)

#define auto_trigger_is_ON ((ADCSRA & (1<<ADATE)) == (1 << ADATE))
#define auto_trigger_is_OFF ((ADCSRA & (1<<ADATE)) == 0)

#include "init_vars.h"
#include "IN_OUT.h"

#ifdef SIMUL_MODE     // Mode simulation
  #define GET_VAL_MOY lire_rien()
  #define SIMULER simul()
  #include "simul.h"
  #include "lire_rien.h"
#else                  // Mode production
  #define GET_VAL_MOY lire_ADC()
  #define SIMULER
  #include "lire_ADC.h"
#endif

#include "lire_Serial.h"
#include "lire_valeurs.h"

void aff_version(){
  char mesg[10];
  strcpy (mesg,"* THC  Version *");
  //------------XXXXXXXXXXXXXXXX  16 char
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print (mesg);
  strcpy (mesg,"*     02.21    *");
  //------------XXXXXXXXXXXXXXXX  16 char
  lcd.setCursor(0, 1);
  lcd.print (mesg);
  delay(2000);
}

void auto_trigger_on () {
  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}

void aff_tension(){ //sur TM1637
        display.clear();
        display.home();
        if (val_moy < 100){
            display.setCursor(1,1);
        }
        display.print(val_moy);
}

/*
//void aff_tension(){ // sur LCD
  char mess[10];
  strcpy (mess,"");
  aff_param (mess, val_moy);
}
*/

void move_torch() {

  if (ordre == 0) {                 // tension ok
	if (sens_actuel == 0) {   // ... et moteur arrêté
		if (Tmin > 1000) {
				ms = millis();
				if (ms > timelimit) {
					aff_tension();
					timelimit = ms + 300;
				}
		}
	}
    else if (m != 0) {              // on ralentit deja, continuer
       ralentir();
    }
    else {                                //sinon commencer a ralentir
      m = i;
      i = 1;
      ralentir();
    }
  }
  else {                      // ordre 1 ou 2
    if (i == 1) {   // debut de mouvement (premier pas)
      sens_actuel = ordre;
      //adcval = 0; D2_TOGG(); // pour correction demi-hauteur :
      //cpt=0;                 // synchro duree echantillonnage boucle ISR
      //digitalWrite(pinZDIR, !(ordre - 1)); // ordre : 2 / 1 => ordre-1 : HIGH / LOW
      switch (ordre) {
        case 1: // Bas
          digitalWrite(pinZDIR,HIGH);
          //ZDIR_ON();
          digitalWrite(ledPinL, HIGH);
          //LedL_ON();
          break;
        case 2: // Haut
          digitalWrite(pinZDIR,LOW);
          //ZDIR_OFF();
          digitalWrite(ledPinH, HIGH);
          //LedH_ON();
          break;
      }
      avancer();
    }
    else if (ordre == sens_actuel && (m == 0)) { // pas de changement de sens et pas en deceleration
      avancer();
    }
    else {          // changement de sens avec vitesse > 0
      if (m != 0) { // on ralentit deja, continuer
        ralentir();
      }
      else {        //sinon commencer a ralentir
        m = i;
        i = 1;
        ralentir();
      }
    }
  }
}

void ralentir() {
  if (Ti2 < Tstop) { // V > Vstop : calcul_Ti()
    avancer();         // inclut calcul_Ti()
  }
  else {                 // sinon ok pour arret moteur
    digitalWrite(pinZDIR, LOW);
    //ZDIR_OFF();
    digitalWrite(ledPinH, LOW);
    //LedH_OFF();
    digitalWrite(ledPinL, LOW);
    //LedL_OFF();
    Ti = TMax;
    sens_actuel = 0;
    ordre = 0;
    i = 1;
    m = 0;
  }
}

void avancer() {
  chrono = micros();
  if ((chrono - chrono_prec) > (Ti2)) { //nouveau pas
    chrono_prec = chrono;
    digitalWrite(pinZSTEP, HIGH);
    //ZSTEP_ON();
    calcul_Ti();                       // calcul du pas suivant (sert aussi de tempo!)
    digitalWrite(pinZSTEP, LOW);
    //ZSTEP_TOGG();
//    ZSTEP_OFF();
  }
}

void calcul_Ti () { // (duree fonction : 70 µs environ)

  Ti2 = Ti;

  if (m != 0 ) {
    n = i - m;
  }
  else {
    n = i;
  }

  Ti = Ti - (2 * Ti) / (4 * n + 1);
  i = i + 1;
  if (Ti > TMax) {
    Ti = TMax;
  }
  else if (Ti < Tmin) {
    Ti = Tmin;  // ne pas depasser la vitesse max, bloquer i
    i = i - 1;  // et donc n pour calculer m
  }
}

ISR(ADC_vect) {

  GET_VAL_MOY;

}


void init_thc() {

      //Serial.end(); // *** a activer pour utiliser Serial.print ***
      Serial.begin(115200);
      while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB only
      }
      Serial.println(F("THC ON\n"));  // *** a desactiver si Serial.print utilise plus bas ***
      lcd.clear();
//      display.clear();
//      delay(1000);                // début de la coupe : temporisation ...
      Serial.end();
      init_coupe = false;
      val1 = vRef;
      val2 = val1;
      adcval = 0;
      cpt = 0;
      ok = false;
      auto_trigger_on();      // enable auto trigger
      digitalWrite(ledPin, HIGH);
//      timelimit = 0;
      // stocker nouvelles valeurs EEPROM si changees
      put_Eprom ();

}

void exit_thc(){
    auto_trigger_off;       // disable auto trigger
    ordre = 0;
    while (Ti < TMax) {
        move_torch();
      }
    digitalWrite(ledPin, LOW);
    init_coupe = true;    // réinitialise la tempo de coupe
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB only
    }
    Serial.print(F("THC OFF"));
    ok = true;
    ser_aff_valeurs();

//    Serial.print("dt = ");
//    Serial.println(dt);
}

void lire_eeprom(){

      EEPROM.get(0,seuil);
      EEPROM.get(2,nech);
      EEPROM.get(4,TMax);
      Tstop = TMax / 2;
      Ti=TMax;
      EEPROM.get(6,Tmin);
      EEPROM.get(8,vRef);

}


void setup() {

  pinMode(2, OUTPUT); // pour test simul()

  pinMode(ledPin, OUTPUT);
  pinMode(ledPinL, OUTPUT);
  pinMode(ledPinH, OUTPUT);

  pinMode(pinZSTEP, OUTPUT);
  pinMode(pinZDIR, OUTPUT);
  pinMode(pinCoolant, INPUT_PULLUP);

  pinMode (bouton_SW,INPUT_PULLUP);
  pinMode (bouton_CLK,INPUT_PULLUP);
  pinMode (bouton_DT,INPUT_PULLUP);

  digitalWrite(pinZSTEP, LOW);
  digitalWrite(ledPin, LOW);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  display.begin();            // initializes the TM1637 display
  display.setBacklight(100);  // set the brightness to 100 %

  aff_version();

  // Source : EFY PC OsciloScope
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz
  //ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);    // 8 prescaler for 153.8 KHz
  

  //  ADCSRA |= (1 << ADATE); // enable auto trigger
  //  ADCSRA &= ~(1 << ADATE); // disable auto trigger

  // auto trigger désactivé par défaut


//  Premiere utilisation de l'EEPROM mettre "1" (mettre "0" après)
//if (1) {
if (0) {
    EEPROM.put(0,seuil);
    EEPROM.put(2,nech);
    EEPROM.put(4,TMax);
    EEPROM.put(6,Tmin);
    EEPROM.put(8,vRef);
}

  lire_eeprom();
  val1 = vRef;
  val2 = val1;
  chrono_prec = micros();
  display.print(vRef);
  exit_thc();
  ser_aff_valeurs();
  delay(100);

//  aLastState = digitalRead(bouton_CLK); // initialise la roue codeuse

// initialisation de la tempo d'affichage de tension
  ms = millis();
  timelimit = ms + 200;
}


void loop() {

  //************* THC ON *******************

//  if (digitalRead (pinCoolant) == HIGH) {
  if (etat_COOLANT()) {

//    timelimit = max(dt, micros() - ms); // duree boucle max : 204 µs (hors init)

    if auto_trigger_is_OFF { // si auto trigger inactif

      init_thc();

    }

//   ms = micros();

    SIMULER;

    if (ok == true) { // nouvelle valeur moyenne disponible
      //D2_TOGG();
      ok = false;
      val2 = val_moy;
//        Serial.println(val2); // *** ne pas oublier de modifier "Serial.end" ci-dessus ***
//                                 duree mesuree de Serial.print : 130 à 180 µs (@115200)
        if (val2 > vRef + seuil) {
          ordre = 1; // descendre
        }
        else if (val2 < vRef - seuil) {
          ordre = 2; // monter
        }
        else {
          ordre = 0; // tension OK
        }
      val1 = val2;
      //D2_TOGG();
    }
    move_torch();
}

//************* THC OFF *******************

  else {

    if auto_trigger_is_ON { // si auto trigger actif
      exit_thc();
      //help_serial();
      //Serial.print("THC OFF\n");
      }
    lire_valeurs();       // lecture des parametres avec la roue codeuse
    //lire_Serial();        // lecture des commandes sur Serial
  }

}
