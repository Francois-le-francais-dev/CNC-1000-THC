/*  lire_serial : lecture de commandes
     Attention : regler le moniteur serie sur "Nouvelle ligne" => envoie '\n'

         ***************  Commandes  *******************

           ? = liste les valeurs de réglage actuelles
           S = valeur du seuil (1 a 5)
           N = nombre d'échantillons (1 a 500)
           T = valeur de TMax ( > 1000) TMax=2000 => accélération=9.377 m/s²
           t = valeur de Tmin (187:6000 mm/min - 200:5600 mm/min - 300:3750 mm/min)
           V = tension de consigne
*/

void put_Eprom () {
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

  EEPROM.put(0,seuil);
  EEPROM.put(2,nech);
  EEPROM.put(4,TMax);
  EEPROM.put(6,Tmin);
  EEPROM.put(8,vRef);

}

void help_serial() {
  //Serial.println(F("rappel : moniteur serie > 'Nouvelle ligne'"));
}

bool saisie(char message[40], unsigned int &param) {

    unsigned int nb_val = 0;
    char nb_txt [6];
    nb_txt[0]=0; // important  (initialiser la chaine) equivaut a : strcpy (nb_txt,"")
    char Buf = '\0';  // 1 caractere de fin de chaine
  
    Serial.print (message);
    Serial.read(); // enlever '\n'
    while (!Serial.available()) {;} // attendre une entrée
    while (Serial.available() > 0)  // tant qu'il y a a manger sur le port série
    {
      Buf = Serial.read();         // lire 1 caractère dans le buffer serie
      if (Buf == '\n')             // '\n' comme caractère de fin de séquence.
      {
        Buf = '\0';
        if (nb_txt[0] != '\0') {
          // Traitement
          nb_val = atoi(nb_txt);
          if (nb_val != 0) { // ok c'est un nombre
            param = nb_val;
          }
          else {
            return (false);
          }
        }
        else {
          Serial.println("erreur de saisie");
          help_serial();
          return (false);
        }
      }
      else {
        char bout[] = {Buf,'\0'};
        strcat(nb_txt, bout);  // concaténer le caractère dans la chaine
        delay(1);  // sinon bug !!
      }
    }
    Serial.read(); // enlever '\n'
  
    if (nb_val != 0) {
      return (true);
    }
    else {
      Serial.println();
      return (false);
    }
}

void aff_valeurs() {

          Serial.read(); // enlever '\n'
          Serial.print(F("\n\n[S] seuil (1 - 5)               = "));
          Serial.println(seuil);
          Serial.print(F("[N] nb echantillons (1 - 1000)  = "));
          Serial.println(nech);
          Serial.print(F("[T] TMax (> 1000)               = "));
          Serial.println(TMax);
          Serial.print(F("    Tstop                       = "));
          Serial.println(Tstop);
          Serial.print(F("[t] Tmin (150 - 1000)           = "));
          Serial.println(Tmin);
          Serial.print(F("[V] tension consigne (60 - 170) = "));
          Serial.println(vRef);
          Serial.println("");
          Serial.read(); // enlever '\n'
          delay(10);

}

void lire_Serial() {

char mess[40];

if (Serial.available()) {
    delay(1); // patience ...
    if (Serial.available() > 2) {         // un seul caractere sinon erreur de saisie.
      //Serial.println("erreur de saisie");
      while (Serial.available() > 0) {
        Serial.read();                  // plusieurs car. => vider le buffer
      }
    }
    else {                                // ok, analyse du choix
      while (Serial.available() > 0)
      {
        switch (Serial.read())
        {
        //////////////////////////////////////
  
          case '?':
            aff_valeurs();
            break;
  
        //////////////////////////////////////
  
          case 'S':
            strcpy (mess,"Valeur de seuil (1-5): ");
            saisie (mess, seuil);
            aff_valeurs();
            break;
  
        //////////////////////////////////////
  
          case 'N':
            strcpy (mess,"Nombre d'echantillons (1-1000): ");
            saisie (mess, nech);
            aff_valeurs ();
            break;
  
        //////////////////////////////////////
  
          case 'T':
            strcpy (mess,"TMax (> 1000) : ");
            if (saisie (mess, TMax)) {
              Tstop = TMax / 2;
            }
            aff_valeurs();
            break;
  
        //////////////////////////////////////
  
          case 't':
            strcpy(mess,"Tmin (150-1000): ");
            saisie (mess, Tmin);
            aff_valeurs();
            break;
  
        //////////////////////////////////////
  
          case 'V':
            strcpy(mess,"Tension de consigne (60-170): ");
            saisie (mess, vRef);
            aff_valeurs();
            break;
  
        //////////////////////////////////////
  
          default :
            //Serial.println ("Quoi ??");
            Serial.read(); // enlever '\n'
   
         //////////////////////////////////////
  
        }
      }
      Serial.read(); // enlever '\n'
    }
  put_Eprom ();
  }
}
    /* Mise a jour de l'EEPROM (valeurs modifiees seulement) */
