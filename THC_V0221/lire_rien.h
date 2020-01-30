void lire_rien() {
  //D2_TOGG();

  adcval = adcval + ADCH;  // read 8 bit value from ADC
  cpt++;
  if (cpt == nech)
  {
    D2_TOGG();
    adcval = adcval/nech; //calcul vérifié avec des valeurs fixes
    adcval = 0;
    
        val_moy = val_moy;
        
    cpt = 0;
    ok = true;
  }
  //D2_TOGG();

}
