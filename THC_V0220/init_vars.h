/******* CONFIGURATION DES PARAMETRES D'ACCELERATION DU MOTEUR Z *******/
/*---------------------------------------------------------------------*/

unsigned int TMax     = 6000;      // durée du pas de départ (µs)
//                                        Tmax détermine l'accélération
//                                        TMax=2000 => accélération=9.377 m/s²
//                                        et vitesse de départ = 9.4 mm/min
unsigned int Tmin      = 3000;      // durée du pas pour vitesse maxi
//                                        187:6000 mm/min - 200:5600 mm/min - 300:3750 mm/min
unsigned int Tstop     = TMax / 2; // durée mini du pas pour arrêt brutal (doit etre inferieur a TMax/2)
/*---------------------------------------------------------------------*/

float Ti    = TMax;                // durée du pas (entre 2 impulsions)
uint16_t m  = 0;                   // ralentissement : nombre de pas (calculé en temps réel)
uint16_t Ti2= 0;

unsigned int nech  = 10; // nombre d'échantillons
unsigned int seuil = 1;    // écart accepté avec tension de consigne (vRef)

unsigned int vRef0 = 78;
unsigned int vRef  = vRef0;

const int bouton_SW   = 3;  // switch codeuse
const int pinZSTEP    = 4;  // PULSE stepper (idem GRBL)
const int bouton_DT   = 5;  // data codeuse
const int bouton_CLK  = 6;  // clock codeuse
const int pinZDIR     = 7;  // DIR Z (idem GRBL) 1 = DESCENTE / 0 = MONTEE (partagee avec LCD !)
const int ledPin      = LED_BUILTIN;// the number of the LED pin (D13)

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int signalPin     = A0; // tension de la torche après diviseur lecture par ISR
const byte PIN_CLK      = A1; // TM1637 define CLK pin (any digital pin)
const byte PIN_DIO      = A2; // TM1637 define DIO pin (any digital pin)
const int pinCoolant    = A3; // COOLANT (idem GRBL)
const int pinCoolant_B  = 3; // COOLANT pour PORTC
const int ledPinH       = A4; // led témoin montée
const int ledPinL       = A5; // led témoin descente
const int ledPinH_B     = 4; // led témoin montée pour PORTC
const int ledPinL_B     = 5; // led témoin descente pour PORTC

SevenSegmentTM1637    display(PIN_CLK, PIN_DIO);

unsigned int val1     = 0;
unsigned int val2     = 0;

static unsigned int cpt = 0, val_moy;
unsigned long adcval = 0;
bool ok = false;

int ordre = 0; //sens moteur : 2 = monter, 1 = descendre, 0 = stop.
int sens_actuel = 0;

unsigned long chrono       = 0;
unsigned long chrono_prec  = 0;

long n = 1;
long i = 1;
bool init_coupe = true;

int counter = 0;
int sw_count = 1;

static uint8_t prevNextCode = 0;
static uint16_t store=0;

/*-------------------- mesure duree boucle ----------------------------*/
unsigned long tt;
unsigned int dt =0;
/*---------------------------------------------------------------------*/
