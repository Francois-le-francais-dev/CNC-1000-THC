// THC pour Arduino MEGA 2560

#define firm_ver "*  04.03.065   *"
//                XXXXXXXXXXXXXXXX  = 16 char

/* voir fichier REVISIONS.txt dans le dossier superieur pour historique des modifications.

  V04.03.064 - 19/01/2025
            - retard THC pour stabilisation de l'arc : durée fixée à 700 ms après mesures de vitesses
              cf "calculs THC.ods"

  V04.03.065 - 16/02/2025
            - entrées Coupe_Pin (M3) / Coolant_Pin (M8) sur broches logiques => actif niveau HAUT
            - réaffectation des broches pour nouveau boitier THC avec connecteur DB25
            - affichage de la tension permanent, même au repos


  *** prévu ***
            - (?) stockage de la valeur cumulée des déplacements Z et restitution z0 initial dans THC OFF()
            - (?) affichage de la valeur de correction Z en continu
            - (?) anti-plongée : arrêt THC sur seuil de tension max. (vide)

  *********************************************************************************************

   PB non résolu : si la torche rencontre un trou dans la tôle elle va plonger indéfiniment ..
   et risque de heurter l'autre bord du trou !
   Solution 1 - (fait) réduire beaucoup la vitesse de descente (comme sur un amortisseur de voiture) :
                cf move_torch() / TMax_haut, TMax_bas, coef_bas : accélération réduite en descente.
   Solution 2 - si la tension augmente au-delà d'un seuil "max" alors arrêter la correction jusqu'au
                retour à un seuil "normal"= mode "anti-plongée"

*/

//#define SIMUL_MODE    // définir le mode simulation

#include <EEPROM.h>
#include <SevenSegmentTM1637.h>
#include <LiquidCrystal.h>

#include "init_vars.h"
#include "IN_OUT.h"
#include "LCD_Print.h"
#include "impuls_TIMER.h"


#ifdef SIMUL_MODE  // Mode simulation
//#define GET_VAL_MOY lire_rien()
#define SIMULER simul()
#include "simul.h"
#include "lire_rien.h"
#else  // Mode production
//#define GET_VAL_MOY lire_ADC()
#define SIMULER
#include "lire_ADC.h"
#endif

#include "lire_mode.h"

// Communication I2C pour LOGGER
//#include "SlaveWriter.h"

void put_Eprom() {
  /* 
  EEPROM.put(): This function uses EEPROM.update() to perform the write,
                  so does not rewrites the value if it didn't change. 
  parametre   adresse EEPROM
   seuil      0,1
   nech       2,3
   TMax       4,5
   Tmin       6,7
   vRef       8,9
  */

  EEPROM.put(0, seuil);
  EEPROM.put(2, nech);
  EEPROM.put(4, TMax);
  EEPROM.put(6, Tmin);
  EEPROM.put(8, vRef);
}


void aff_tension() {  //sur TM1637
  ms = millis();
  if (ms > timelimit) {
    timelimit = ms + 100;  // délai pour nouvel affichage tension
    display_Varc.clear();
    display_Varc.home();
    if (vArc < 100) {
      display_Varc.setCursor(1, 1);
    }
    display_Varc.print(vArc);
  }
}


void move_torch() {

  if (ordre != 0) {  // monter / descendre
    LedOK_OFF();
    if (!arrete) {
      if (ordre != sens_actuel) {  // pas même sens
        arreter();
      } else {
        m = 0;
      }
    } else {  // arrêté
      demarrer();
    }
  } else {  // ordre = 0 : arrêter (tension ok)
    LedOK_ON();
    if (!arrete) {
      arreter();
    }
  }
}

void demarrer() {
  sens_actuel = ordre;
  switch (ordre) {
    case 1:  // (vArc > vMax) Descendre
      TLong = TMax_Bas;
      Ti = TLong;
      Ti2 = Ti;
      Tstop = TLong / 2;
      LedOK_OFF();
      LedH_OFF();
      LedL_ON();
      ZDIR_ON();
      init_TIMER1();
      start_impuls();  // démarre les interruptions : impulsions + calcul_Ti()
      arrete = false;
      break;
    case 2:  // (vArc < vMin) Monter
      TLong = TMax_Haut;
      Ti = TLong;
      Ti2 = Ti;
      Tstop = TLong / 2;
      LedOK_OFF();
      LedH_ON();
      LedL_OFF();
      ZDIR_OFF();
      init_TIMER1();
      start_impuls();
      arrete = false;
      break;
    default:
      break;
  }
}

void arreter() {
  if (m == 0) {
    m = 1;
  }
  // if (!(Ti2 < Tstop)) {  // OK pour arret moteur + modif LEDs / ZDIR
  if (1) {          // Arret force car pb depassements consigne pendant le ralentissement + instabilites
    stop_impuls();  // STOP interruptions TIMER1 !
    Ti = TLong;
    Ti2 = Ti;
    sens_actuel = 0;
    i = 1;
    m = 0;
    LedOK_ON();
    LedH_OFF();
    LedL_OFF();
    ZDIR_OFF();
    arrete = true;
  }
}

void init_thc() {
  detachInterrupt(digitalPinToInterrupt(INZ_PULSE));
  //aff_tension();
  vMax = vRef + seuil;
  vMin = vRef - seuil;
  vArc = vRef;
  adcval = 0;
  cpt = 0;
  ok = false;
  // Initialisation moteur
  Ti = TMax;
  Ti2 = Ti;
  sens_actuel = 0;
  ordre = 0;
  i = 1;
  m = 0;
  LedOK_ON();
  LedH_OFF();
  LedL_OFF();
  ZDIR_OFF();
  put_Eprom();  // stocker nouvelles valeurs EEPROM si changees
  init_codeuse();
  thc_is_OFF = false;
  led_THC_ON();  // Temoin THC ON
}

void exit_thc() {
  TORCHE_OFF();  // arrêt torche
  //envoi_logger_fin();         // transmet les donnees au LOGGER
  ordre = 0;
  while (!arrete) {
    arreter();
  }
  //delay(300);
  //stop_impuls();
  //******************************************//
  // restitution position z d'origine
  //******************************************//
  LedOK_OFF();
  LedH_OFF();
  LedL_OFF();
  ZDIR_OFF();
  start_modes = true;
  display_Varc.clear();
  thc_is_OFF = true;
  led_THC_OFF();  // V4.0
  // Recopie des mouvements Z
  attachInterrupt(digitalPinToInterrupt(INZ_PULSE), pulsez_out, RISING);
}

void THC_ON() {
  // ************* THC ON ******************* //
  if (thc_is_OFF) {  // si THC inactif
    init_thc();
  }
  SIMULER;
  if (ok == true) {  // nouvelle valeur disponible : val_tot
    // DEBUG_TOGG();    // V04.03.04.51c : durée boucle (nech = 100) : 18 ms
    ok = false;
    vArc_new = val_tot / nech;
    aff_tension();
    // if (vArc_new != vArc) {  // inutile et empêche la prise en compte de
    //  nouvelle consigne de tension si vArc ne change pas
    vArc = vArc_new;
    if (vArc > vMax) {
      ordre = 1;  // descendre
    } else if (vArc < vMin) {
      ordre = 2;  // monter
    } else {
      ordre = 0;  // tension OK
    }
    // } // fin test
  }
  if (mode_calme) {
    correction_demi_seuil();
  }
  lire_modes();  // lecture des parametres avec la roue codeuse
  lire_ADC();    // lecture directe par analogRead()
  move_torch();
}

void THC_OFF() {
  // ************* THC OFF ******************* //
  if (!thc_is_OFF) {  // si THC actif
    exit_thc();
  }
  lire_modes();  // lecture des parametres avec la roue codeuse
  put_Eprom();   // stocker nouvelles valeurs EEPROM si changees
}

void lire_eeprom() {

  EEPROM.get(0, seuil);
  EEPROM.get(2, nech);
  EEPROM.get(4, TMax);
  EEPROM.get(6, Tmin);
  EEPROM.get(8, vRef);
  Tstop = TMax / 2;
  Ti = TMax;
  TMax_Haut = TMax;
  TMax_Bas = TMax * coef_bas;
  accel = 23562000000 / TMax / TMax;
  vitesse = 11781 / Tmin;  // mm/sec
}

void correction_demi_seuil() {  // tests : voir "pas_moteurZ.ino"
  int p = 0;
  if (seuil != 6) {
    p = seuil * 5;
  } else p = 3;  // pour seuil "0,5" ;o)
                 //    move_torch();
  while (p > 0) {
    move_torch();
    p--;
  }
}

#include "test_moteur.h"

void pulsez_out() {
  // recopier DIR Z
  bool etat_dir_z = digitalRead(INZ_DIR);
  digitalWrite(pinZDIR, etat_dir_z);
  // if (PIN_INZ_DIR && (1 << PN_INZ_DIR)) {
  //   PORT_ZDIR |= 1 << PN_ZDIR;
  // } else PORT_ZDIR &= ~(1 << PN_ZDIR);
  //sortie sur STEP Z
  PORT_ZSTEP |= 1 << PN_ZSTEP;
  for (volatile int ipulse = 0; ipulse < 10; ipulse++) { ; }  // 12 µs environ pour "ipulse < 10"
  PORT_ZSTEP &= ~(1 << PN_ZSTEP);
}

// void dirz_out() {
//   if (PIN_INZ_DIR && (1 << PN_INZ_DIR)) {
//     PORT_ZDIR |= 1 << PN_ZDIR;
//   } else PORT_ZDIR &= ~(1 << PN_ZDIR);
// }



void setup() {

  lcd.begin(16, 2);  // set up the LCD's number of columns and rows:
  debug_msg("Setup commence..");

  pinMode(pinZDIR, OUTPUT);
  pinMode(bouton_SW, INPUT_PULLUP);
  pinMode(pinZSTEP, OUTPUT);
  pinMode(bouton_DT, INPUT_PULLUP);
  pinMode(bouton_CLK, INPUT_PULLUP);
  pinMode(ledPin_THC, OUTPUT);
  pinMode(ledPinH, OUTPUT);
  pinMode(ledPinHH, OUTPUT);
  pinMode(ledPinOK, OUTPUT);
  // pinMode(Coupe_Pin, INPUT_PULLUP);
  pinMode(Coupe_Pin, INPUT);
  pinMode(AC_PSL_Pin, INPUT_PULLUP);
  pinMode(ARC_OK_Pin, INPUT);
  pinMode(SWT_Pin, INPUT_PULLUP);
  pinMode(GRBL_pause_Pin, OUTPUT);
  pinMode(GRBL_start_Pin, OUTPUT);
  pinMode(ledPinL, OUTPUT);
  pinMode(SW_PAUSE_Pin, INPUT_PULLUP);  // Bouton PAUSE GRBL
  pinMode(SW_START_Pin, INPUT_PULLUP);  // Bouton START GRBL
  pinMode(relais_coupe_Pin, OUTPUT);    // commande de relais de la torche
  // pinMode(Coolant_Pin, INPUT_PULLUP);  // actif niveau BAS
  pinMode(Coolant_Pin, INPUT);  // actif niveau HAUT
  pinMode(debug_Pin, OUTPUT);   // DEBUG pin
  pinMode(relais_start_arc_Pin, OUTPUT);
  pinMode(pinDefaut_THC, OUTPUT);
  // attachInterrupt(digitalPinToInterrupt(INZ_PULSE), pulsez_out, RISING);
  // attachInterrupt(digitalPinToInterrupt(INZ_DIR), dirz_out, CHANGE);

  voyant_defaut_OFF();  // arrêt voyant défaut THC
  START_ARC_OFF();      // arrêt relais START_ARC
  TORCHE_OFF();         // arrêt relais torche

  //setup_slave_writer(); // initialisation I2C pour LOGGER
  ZSTEP_OFF();  //digitalWrite(pinZSTEP, LOW);
  ZDIR_OFF();
  led_THC_OFF();  //digitalWrite(led_THC, LOW);

  digitalWrite(GRBL_pause_Pin, HIGH);  // GRBL PAUSE OFF (Feed hold off)
  digitalWrite(GRBL_start_Pin, HIGH);  // GRBL START/RESUME OFF
  START_ARC_OFF();
  display_Varc.begin();            // afficheur 1 - initializes the TM1637 display
  display_Varc.setBacklight(100);  // afficheur 1 - set the brightness to 100 %

  display_Vref.begin();  // afficheur 2
  display_Vref.setBacklight(100);

  aff_version();  // sur LCD

  // Initialisation de l'ADC
  // setup_ADC();

  // put_Eprom();  // initialise l'Eprom si nécessaire (la première fois)
  lire_eeprom();

  vArc = vRef;
  display_Vref.print(vRef);
  display_Vref.blink();

  exit_thc();
  delay(100);
  // initialisation de la tempo d'affichage de tension
  ms = millis();
  timelimit = ms + 200;
  debug_msg("Setup OK !");
  //test_moteur();
}


void loop() {

  // Défaut Torche décrochée : SWT
  byte SWT = digitalRead(SWT_Pin);
  if (SWT) {       // *** Pas de contact Torche = Collision ! ***
    TORCHE_OFF();  // arrêt torche
    THC_OFF();
    GRBL_pause();
    aff_pause();
    lcd_msg("COLLISION !");
    voyant_defaut_ON();  //pinDefaut_THC sur HIGH
  } else {
    voyant_defaut_OFF();
  }


  // Défaut Air de service : AC_PSL
  byte air_defaut = digitalRead(AC_PSL_Pin);
  if (air_defaut) {  // *** HIGH = manque air ***
    TORCHE_OFF();    // arrêt torche
    THC_OFF();
    GRBL_pause();
    aff_pause();
    lcd_msg("MANQUE AIR");
    voyant_defaut_ON();  //pinDefaut_THC sur HIGH
  } else {
    voyant_defaut_OFF();
  }

  // Bouton PAUSE GRBL
  byte btn_pause = digitalRead(SW_PAUSE_Pin);
  if (!btn_pause) {  // bouton enfoncé
    // digitalWrite(GRBL_pause_Pin, LOW);  // GRBL PAUSE
    TORCHE_OFF();  // arrêt torche
    THC_OFF();
    GRBL_pause();
    aff_pause();
    lcd_msg("> BOUTON PAUSE <");
  }

  // Bouton START / RESUME GRBL
  byte btn_start = digitalRead(SW_START_Pin);
  if (!btn_start) {
    voyant_defaut_OFF();                //pinDefaut_THC sur LOW
    digitalWrite(GRBL_start_Pin, LOW);  // GRBL START/RESUME ON
    delay(50);
    digitalWrite(GRBL_start_Pin, HIGH);  // GRBL START/RESUME OFF
  }


  if (millis() > tempo_relais_coupe) {
    bool etat_Coupe_Pin = digitalRead(Coupe_Pin);  // Actif = HIGH
    if (etat_Coupe_Pin != coupe_active) {
      coupe_active = etat_Coupe_Pin;
      if (coupe_active) {
        debut_coupe = true;
        chrono_retard_THC = millis() + RETARD_THC;  // retard THC avant lecture ARC_OK en debut de coupe
      }
      tempo_relais_coupe = millis() + 20;  // tempo pour rebonds relais (réduit pour Teensy)
    }
  }

  if (coupe_active) {
    TORCHE_ON();  // torche allumée
    if (debut_coupe) {
      LedH_ON();
      LedL_ON();
      START_ARC_ON();  // amorçage arc = electrode mise à la masse par relais
      delay(300);      // temporisation de l'amorçage
      START_ARC_OFF();
      LedH_OFF();
      LedL_OFF();
      // delay(500);
      while (millis() < chrono_retard_THC) {  // retard THC
        lire_ADC();
        if (ok == true) {  // nouvelle valeur disponible : val_tot
          ok = false;
          vArc = val_tot / nech;
          aff_tension();
        }
      }
      debut_coupe = false;
    }
    bool ARC_OK = digitalRead(ARC_OK_Pin);  // Niveau logique sur carte diviseur
    if (!ARC_OK) {                          // OK pour couper !
      TORCHE_OFF();
      THC_OFF();
      GRBL_pause();
      aff_pause();
      lcd_msg("DEFAUT ARC_OK");
      voyant_defaut_ON();  //pinDefaut_THC sur HIGH
    }
    // if (digitalRead(Coolant_Pin) == LOW) {  //  LOW = COOLANT ACTIF (Teensy sortie relais)
    if (digitalRead(Coolant_Pin) == HIGH) {  //  HIGH = COOLANT ACTIF (Teensy sortie logique)
      THC_ON();
    } else {
      THC_OFF();
    }
  } else {
    TORCHE_OFF();  // arrêt torche (déjà dans exit_THC();)
    THC_OFF();

    // test affichage tension permanent
    lire_ADC();
    if (ok == true) {  // nouvelle valeur disponible : val_tot
      // DEBUG_TOGG();    // V04.03.04.51c : durée boucle (nech = 100) : 18 ms
      ok = false;
      vArc_new = val_tot / nech;
      aff_tension();
    }
    // fin test
  }
}
