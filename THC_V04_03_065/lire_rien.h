void lire_rien() {
  //D23_TOGG();

  adcval = adcval + ADCH;  // read 8 bit value from ADC
  cpt++;
  if (cpt == nech)
  {
    //D23_TOGG();
    adcval = adcval/nech; //calcul vérifié avec des valeurs fixes
    adcval = 0;
    
        val_moy = val_moy;
        
    cpt = 0;
    ok = true;
  }
  //D23_TOGG();

}
