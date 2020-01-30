
void lire_ADC() {

  adcval = adcval + ADCH;  // read 8 bit value from ADC
  cpt++;
  if (cpt == nech)
  {
    D2_TOGG(); // pour mesures temps

    val_moy = adcval/nech; //calcul vérifié avec des valeurs fixes
    adcval = 0;
    cpt = 0;
    ok = true;
  }
}
