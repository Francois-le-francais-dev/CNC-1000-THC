
/* A valid CW or  CCW move returns 1, invalid returns 0.*/
int8_t read_rotary() {
  
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(bouton_DT)) prevNextCode |= 0x02;
  if (digitalRead(bouton_CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      //if (store==0xd42b) return 1;
      //if (store==0xe817) return -1;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}

void lire_codeuse() {

  if( counter=read_rotary() ) {
/*      if ( prevNextCode==0x0b) {
         Serial.print("eleven ");
         Serial.println(store,HEX);
      }

      if ( prevNextCode==0x07) {
         Serial.print("seven ");
         Serial.println(store,HEX);
      }
*/
   switch (sw_count) {
         case 1:
          // Volts (60-170) - increment roue = 1
          vRef = vRef + counter;
          if (vRef > 170) vRef = 170;
          if (vRef < 60)  vRef = 60;
          lcd.setCursor(0, 1);
          lcd.print (vRef);
          lcd.print ("    ");
          display.clear();
          display.print(vRef);
          break;

        case 2:
          // Seuil (1-5) - increment roue = 1
          seuil = seuil + counter;
          if (seuil > 5) seuil = 5;
          if (seuil < 1) seuil = 1;
          lcd.setCursor(0, 1);
          lcd.print (seuil);
          lcd.print ("    ");
          break;

        case 3:
          // Nb ech (10-1000) - increment roue = 10
          nech = nech + counter*10;
          if (nech > 10000) nech = 10000;
          if (nech < 10) nech = 10;
          lcd.setCursor(0, 1);
          lcd.print (nech);
          lcd.print ("    ");
          break;

        case 4:
          // TMax (> 1000) - increment roue = 100
          TMax = TMax + counter*100;
          if (TMax > 20000) TMax = 20000;
          if (TMax < 1000) TMax = 1000;
          if (Tmin > TMax/2) Tmin = TMax/2;
          Tstop = TMax / 2;
          Ti = TMax;
          lcd.setCursor(0, 1);
          lcd.print (TMax);
          lcd.print ("   V=");
          lcd.print (1125000/TMax);
          lcd.print ("    ");
          break;

        case 5:
          // Tmin (150-TMax/2) - increment roue = 50
          Tmin = Tmin + counter*50;
          if (Tmin > TMax/2) Tmin = TMax/2;
          if (Tmin < 150) Tmin = 150;
          lcd.setCursor(0, 1);
          lcd.print (Tmin);
          lcd.print ("   V=");
          lcd.print (1125000/Tmin);
          lcd.print ("    ");
          break;

        default :
          break;
      }
//   put_Eprom ();
   }
}

void aff_param(char message[40], unsigned int &param) {

    unsigned int val_param = param;
  
/*LCD*/
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print (message);

    lcd.setCursor(0, 1);
    lcd.print (val_param);

    return (true);
}

void select_param(int p) {

  char mess[40];

  switch (p) {
          case 1:
            strcpy (mess,"Volts (60-170)"); // increment roue = 1
            //------------XXXXXXXXXXXXXXXX  16 char
            aff_param (mess, vRef);
           display.clear();
           display.print(vRef);
		   break;
  
          case 2:
            strcpy (mess,"Seuil (1-5) "); // increment roue = 1
            //------------XXXXXXXXXXXXXXXX  16 char
            aff_param (mess, seuil);
            break;
  
          case 3:
            strcpy (mess,"Echan (< 10 000)"); // increment roue = 10
            //------------XXXXXXXXXXXXXXXX  16 char
            aff_param (mess, nech);
            break;
  
          case 4:
            strcpy (mess,"TMax ( >1000)"); // increment roue = 100
            //------------XXXXXXXXXXXXXXXX  16 char
            aff_param (mess, TMax);
            lcd.print ("   V=");
            lcd.print (1125000/TMax);
            lcd.print ("    ");
            break;
  
          case 5:
            strcpy (mess,"Tmin(150-TMax/2)"); // increment roue = 5
            //------------XXXXXXXXXXXXXXXX  16 char
            aff_param (mess, Tmin);
            lcd.print ("   V=");
            lcd.print (1125000/Tmin);
            lcd.print ("    ");
            break;

          default :
            break;

 
        }
}


void lire_valeurs() {

  if (ok) {
    select_param(sw_count);
    ok = false;
  }

  // bouton switch => changer de parametre
  if (!digitalRead(bouton_SW)){
    sw_count++;
    if (sw_count > 5) sw_count = 1;
    select_param(sw_count); // affiche le parametre actuel
    delay(500);
  }
  // tourner => changer la valeur du parametre actuel
  lire_codeuse();

  }
