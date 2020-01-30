/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/

/* **** pour SIMULATION ***** */

int move=1;
unsigned int simulx = 200; // i max pour simul bug !

/*void simul2() {


   //vRef = vRef0;

   if (Ti == TMax) {
      if( move == 1) {
        move=2;
        val_moy=vRef + seuil + 5; // descendre
        //Serial.println(move);
      digitalWrite(pinZDIR, (ordre - 1)); // ordre : 2 / 1 => ordre-1 : HIGH / LOW
        delay(50);
      digitalWrite(pinZDIR, !(ordre - 1)); // ordre : 2 / 1 => ordre-1 : HIGH / LOW
        ok = true;
        }
      else {
        move=1;
        val_moy=vRef - seuil - 5; // monter
        //Serial.println(move);
      digitalWrite(pinZDIR, (ordre - 1)); // ordre : 2 / 1 => ordre-1 : HIGH / LOW
        delay(50);
      digitalWrite(pinZDIR, !(ordre - 1)); // ordre : 2 / 1 => ordre-1 : HIGH / LOW
        delay(500);
        ok=true;
        }
   }
   else if (i > simulx) { // inverser en cours de chgt de vitesse ...
        ordre=0;
   }
}

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/

void simul() {

  if (Ti == Tmin) {
    val_moy=vRef;
  }

  else if (Ti == TMax) {  // descendre
      if( move == 1) {
        move=2;
        val_moy=vRef + seuil + 5;
        delay(20);
        }
      else {  // monter
        move=1;
        val_moy=vRef - seuil - 5;
        delay(10);
        }
  }
  ok=true;
}
