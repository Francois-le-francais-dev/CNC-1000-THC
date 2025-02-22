
void init_codeuse() {
  sw_count = 1;
  start_modes = true;
}

int8_t read_rotary() {  // A valid CW or  CCW move returns 1, invalid returns 0.

  static int8_t rot_enc_table[] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

  prevNextCode <<= 2;
  if (digitalRead(bouton_DT)) prevNextCode |= 0x02;
  if (digitalRead(bouton_CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

  // If valid then store as 16 bit data.
  if (rot_enc_table[prevNextCode]) {
    store <<= 4;
    store |= prevNextCode;
    //if (store==0xd42b) return 1;
    //if (store==0xe817) return -1;
    if ((store & 0xff) == 0x2b) return -1;
    if ((store & 0xff) == 0x17) return 1;
  }
  return 0;
}

void lire_codeuse() {

  counter = read_rotary();
  if (counter) {  // utilise la valeur retournee par read_rotary() ** ne pas mettre "==" **
    switch (sw_count) {
      case 1:
        // Volts (70-130) - increment roue = 1
        vRef = vRef + counter;
        if (vRef > 130) vRef = 130;
        if (vRef < 70) vRef = 70;
        vMax = vRef + seuil;
        vMin = vRef - seuil;
        lcd.setCursor(0, 1);
        lcd.print(vRef);
        lcd.print("    ");
        display_Vref.clear();
        display_Vref.print(vRef);
        break;

      case 2:
        // Seuil (0-5) - increment roue = 1
        if (seuil == 0 && counter == -1) counter = 0;  //seuil = 1;
        if (seuil == 50 && counter == +1) counter = 0;
        if (seuil == 50 && counter == -1) seuil = 7;

        seuil = seuil + counter;
        if (seuil > 6) seuil = 50;
        //          if (seuil > 7) seuil = 50;

        if (seuil == 0) {
          mode_calme = 0;
          nech = 10;
          TMax = 1000;
        } else {
          mode_calme = 1;
          vMax = vRef + seuil;
          vMin = vRef - seuil;
        }

        lcd.setCursor(0, 1);
        if (seuil == 0) {
          lcd.print(seuil);
          lcd.print("  MODE RAPIDE   ");
        } else if (seuil == 6) {
          lcd.print("0.5             ");
        } else if (seuil == 50) {
          lcd.print(" *** MESURE ***");
        } else {
          lcd.print(seuil);
          lcd.print("              ");
        }
        break;

      case 3:
        // Nb ech (10-1000) - increment roue = 10
        nech = nech + counter * 10;
        if (nech > 500) nech = 500;
        if (nech < 10) nech = 10;
        lcd.setCursor(0, 1);
        lcd.print(nech);
        lcd.print("    ");
        break;

      case 4:
        // TMax (> 1000) - increment roue = 100
        TMax = TMax - counter * 100;
        if (TMax > 6000) TMax = 6000;
        if (TMax < 500) TMax = 500;
        if (Tmin > TMax / 2) Tmin = TMax / 2;
        //Tstop = TMax / 2;  NON, cf move_torch()
        //Ti = TMax;
        TMax_Haut = TMax;
        TMax_Bas = TMax * coef_bas;
        accel = 23562000000 / TMax / TMax;  // cf calcul Accel_stepper.ods
        lcd.setCursor(0, 1);
        if (accel < 10000) lcd.print(" ");
        if (accel < 1000) lcd.print(" ");
        lcd.print(accel, 0);
        lcd.print(" mm/s2");
        break;

      case 5:
        // Tmin (70 - TMax/2) - increment roue = 50
        //if (counter > 0) up_vitesse = true;
        //else up_vitesse = false;
        if (Tmin > 100) Tmin = Tmin - counter * 50;
        else Tmin = Tmin - counter * 25;
        if (Tmin > TMax / 2) Tmin = TMax / 2;
        if (Tmin < 50) Tmin = 50;  // vitesse maximale autorisée !
        vitesse = 11781 / Tmin;
        lcd.setCursor(0, 1);
        if (vitesse < 10000) lcd.print(" ");
        if (vitesse < 1000) lcd.print(" ");
        if (vitesse < 100) lcd.print(" ");
        //lcd.print("   ");
        lcd.print(vitesse, 0);
        lcd.print(" mm/sec");
        break;

      default:
        break;
    }
    //   put_Eprom ();
  }
}

bool aff_param(char message[40], unsigned int &param) {

  unsigned int val_param = param;

  /*LCD*/
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);

  lcd.setCursor(0, 1);
  lcd.print(val_param);

  return (true);
}

void select_param(int p) {

  char mess[20];

  switch (p) {
    case 1:
      strcpy(mess, "Volts (70-130)");  // increment roue = 1
      //------------XXXXXXXXXXXXXXXX  16 char
      aff_param(mess, vRef);
      display_Vref.clear();
      display_Vref.print(vRef);
      break;

    case 2:
      strcpy(mess, "Seuil (0-5) ");  // increment roue = 1
      //------------XXXXXXXXXXXXXXXX  16 char
      aff_param(mess, seuil);
      if (seuil == 0) {
        //              lcd.print (seuil);
        lcd.print("  MODE RAPIDE   ");
      } else if (seuil == 6) {
        lcd.setCursor(0, 1);
        lcd.print("0.5              ");
      } else if (seuil == 50) {
        lcd.setCursor(0, 1);
        lcd.print(" *** MESURE ***");
      } else {
        //              lcd.print (seuil);
        lcd.print("               ");
      }
      break;

    case 3:
      strcpy(mess, "Echant (Max 500)");  // increment roue = 10
      //------------XXXXXXXXXXXXXXXX  16 char
      aff_param(mess, nech);
      break;

    case 4:
      //strcpy(mess, "TMax            ");  // increment roue = 100
      //------------XXXXXXXXXXXXXXXX  16 char
      //aff_param(mess, TMax);
      lcd.setCursor(0, 0);
      lcd.print("Acceleration Z :");
      //---------XXXXXXXXXXXXXXXX---------  16 char
      lcd.setCursor(0, 1);
      if (accel < 10000) lcd.print(" ");
      if (accel < 1000) lcd.print(" ");
      //lcd.print(" ");
      lcd.print(accel, 0);
      lcd.print(" mm/s2");
      break;

    case 5:
      //strcpy (mess,"Tmin( < TMax/2 )"); // increment roue = 5
      //------------XXXXXXXXXXXXXXXX  16 char
      //aff_param (mess, Tmin);
      if (Tmin > TMax / 2) Tmin = TMax / 2;
      if (Tmin < 50) Tmin = 50;  // vitesse maximale autorisée !
      vitesse = 11781 / Tmin;
      lcd.setCursor(0, 0);
      lcd.print("Vitesse Max :   ");
      //---------XXXXXXXXXXXXXXXX---------  16 char
      lcd.setCursor(0, 1);
      if (vitesse < 10000) lcd.print(" ");
      if (vitesse < 1000) lcd.print(" ");
      if (vitesse < 100) lcd.print(" ");
      // lcd.print("   ");
      lcd.print(vitesse, 0);
      lcd.print(" mm/sec");
      break;

    default:
      break;
  }
}

void lire_modes() {
  char sw_max;
  // bouton switch => changer de parametre
  if (!digitalRead(bouton_SW)) {
    if (!thc_is_OFF) sw_max = 1;
    else sw_max = 5;
    sw_count++;
    if (sw_count > sw_max) sw_count = 1;
    select_param(sw_count);  // affiche le parametre actuel
    delay(500);
  } else if (start_modes) {  // premier passage uniquement
    select_param(sw_count);
    start_modes = false;
  }
  // tourner => changer la valeur du parametre actuel
  lire_codeuse();
}
