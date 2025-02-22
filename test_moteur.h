/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/

/* **** pour test mouvement moteur ***** */


void inverseur() {
  if (Ti == TMax) {
    if (move == 1) {  // monter
      move = 2;
      ordre = 2;
      if (ok) counter--;
      ok = true;
      delay(100);
    } else {  // descendre
      move = 1;
      ordre = 1;
      delay(100);
    }
  } else if (Ti == Tmin) {
    ordre = 0;
  }
}

void test_moteur() {  // **** Test moteur ****

  // valeurs pour test moteurs
  TMax = 1000;  //   700 mm/min
  Tmin = 100;    // 10 000 mm/min
  Ti = TMax;

  counter = 1;  // Monter + descendre
  ok = false;

  while (counter > 0) {
    move_torch();
    inverseur();
  }
  ordre = 0;
  move_torch();
}
