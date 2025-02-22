/*void setup_ADC() {
  // Source : EFY PC OsciloScope
  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage AVcc (avec condensateur externe 100 nF sur pin AREF) cf  https://fr.wikiversity.org/wiki/Micro_contr%C3%B4leurs_AVR/La_conversion_analogique_num%C3%A9rique
                          // defini par REFS0 (faible) REFS1 (fort) cf https://marcjuneau.ca/?p=112
                          // 00 = AREF
                          // 01 = AVCC
                          // 10 = rien
                          // 11 = 1,1V interne (avec condensateur externe 100 nF sur pin AREF)

  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescaler for 9.6 KHz
  //ADCSRA |= (1 << ADPS2) | (1 << ADPS1);      // 64 prescaler for 19.2 KHz
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0);  // 32 prescaler for 38.5 KHz
  //ADCSRA |= (1 << ADPS2);                     // 16 prescaler for 76.9 KHz
  //ADCSRA |= (1 << ADPS1) | (1 << ADPS0);      // 8 prescaler for 153.8 KHz


  //  ADCSRA |= (1 << ADATE); // enable auto trigger
  //  ADCSRA &= ~(1 << ADATE); // disable auto trigger

  // auto trigger désactivé par défaut

  // ADCSRA |= (1<<ADSC); start conversion without auto trigger = une fois seulement
}*/

/*void read_ADC() {

  // https://www.f-legrand.fr/scidoc/docmml/sciphys/arduino/adcrapide/adcrapide.html

  adcval = adcval + (ADCH >> 1);  // 4 bits de poids fort de l'ADC que l'on divise par 2 (V04)
  cpt++;
  if (cpt == nech) {
    //D23_TOGG(); // pour mesures temps

    //    val_moy = adcval/nech; //calcul vérifié avec des valeurs fixes
    val_tot = adcval;
    adcval = 0;
    cpt = 0;
    o_k = true;
  }
}*/

void lire_ADC() {
  adcval = adcval + (analogRead(signalPin) >> 3);  // 4 bits de poids fort de l'ADC que l'on divise par 2 (V04)
  cpt++;
  if (cpt == nech) {
    //D23_TOGG(); // pour mesures temps
    val_tot = adcval;
    adcval = 0;
    cpt = 0;
    ok = true;
  }
}