/******* CONFIGURATION DES PARAMETRES D'ACCELERATION DU MOTEUR Z *******/
/*---------------------------------------------------------------------*/

unsigned int TMax = 1000;  // durée du pas de départ (µs)
//                                        Tmax détermine l'accélération

unsigned int TMax_Haut = TMax;            // en montée
char coef_bas = 3;                        // multiplie TMax = accélération réduite en descente
unsigned int TMax_Bas = TMax * coef_bas;  // en descente
unsigned int TLong = TMax;                // dans la boucle moteur

volatile unsigned int Tmin = 100;  // durée mini du pas = vitesse maxi
                                   //  70 : 10 000 mm/min - 1500 : 470 mm/min
//volatile bool up_vitesse = false;  // changement de vitesse en cours de mouvement
unsigned int Tstop = TLong / 2;  // durée mini du pas pour arrêt brutal (doit etre inferieur a TMax/2)
/*---------------------------------------------------------------------*/
volatile float Ti = TMax;  // durée du pas (entre 2 impulsions)
volatile float Ti2 = 0;    // (V02.21 : float)

bool arrete = true;  // controle le mouvement

uint16_t m = 0;  // ralentissement : nombre de pas (calculé en temps réel)

unsigned int nech = 100;  // nombre d'échantillons
unsigned int seuil = 2;   // écart accepté avec tension de consigne (vRef)

unsigned int vRef0 = 78;    // tension de référence initiale
unsigned int vRef = vRef0;  // tension de référence en cours
/*---------------------------------------------------------------------*/


// D20 et D21 reserves pour I2C SDA et SCL sur MEGA.

// Interruptions
#define INZ_DIR 2  // PE4 // pin pour interruption DIR Z GRBL
#define PIN_INZ_DIR PINE
#define PN_INZ_DIR PORTE4

#define INZ_PULSE 3  // pin pour interruption STEP Z GRBL

// E/S

#define pinZSTEP 4        // sortie PULSE stepper
#define PORT_ZSTEP PORTG  // pin 4 PORT pour DIR Z
#define PN_ZSTEP PORTG5   // pin 4 = PN_ZSTEP sur MEGA

// Roue codeuse
#define bouton_SW 43   // switch codeuse
#define bouton_DT 42   // data codeuse
#define bouton_CLK 41  // clock codeuse

// LiquidCrystal lcd(RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(49, 48, 47, 46, 45, 44);  // pin 49 à 44

#define pinDefaut_THC 13  // Voyant défaut THC

#define pinZDIR 14       // PJ1 // DIR Z : 1 = DESCENTE / 0 = MONTEE
#define PORT_ZDIR PORTJ  // pin 14
#define PN_ZDIR PORTJ1   // pin 14

// pin 15,16,17,18,19 laissées libres pour ports série

// pin D20 PD1 (SDA)
// pin D21 PD0 (SCL)

#define ledPinHH 22  // PA0 // led HH
#define PORT_ledHH PORTA
#define PN_ledHH PORTA0

#define ledPinH 23  // PA1   // led témoin montée
#define PORT_ledH PORTA
#define PN_ledH PORTA1

#define ledPinOK 24  // PA2          // led témoin tension OK
#define PORT_ledOK PORTA
#define PN_ledOK PORTA2

#define ledPinL 25       // PA3    // led témoin descente Pxx sur MEGA
#define PORT_ledL PORTA  // led témoin descente
#define PN_ledL PORTA3   // led témoin descente

#define ledPin_THC 26  // led THC ON

#define PIN_CLK2 53                                   // (2) TM1637 define CLK pin (any digital pin)
#define PIN_DIO2 52                                   // (2) TM1637 define DIO pin (any digital pin)
SevenSegmentTM1637 display_Vref(PIN_CLK2, PIN_DIO2);  // afficheur tension de consigne
#define PIN_CLK1 51                                   // (1) TM1637 define CLK pin (any digital pin)
#define PIN_DIO1 50                                   // (1) TM1637 define DIO pin (any digital pin)
SevenSegmentTM1637 display_Varc(PIN_CLK1, PIN_DIO1);  // afficheur tension mesurée

#define debug_Pin 30  // DEBUG pin 30 = PC7 sur MEGA
#define Pin_debug_Pin PINC
#define PORT_debug_Pin PORTC
#define PN_debug_Pin PORTC7

// PORT PF0-PF7
#define signalPin A0     // tension de la torche après diviseur
#define SW_PAUSE_Pin A1  // Bouton PAUSE GRBL
#define SW_START_Pin A2  // Bouton START GRBL
#define Coolant_Pin A3   // entrée signal COOLANT (M8)
// #define PIN_Coolant PINF   // PORT
// #define PN_Coolant PORTF3  // PORTN
// #define * A4   //
#define AC_PSL_Pin 11  // entrée détection seuil bas air comprimé
#define ARC_OK_Pin 10  // entrée signal ARC_OK du diviseur
#define SWT_Pin 12     // entrée contact presence torche

// PORT PK0-PK7
#define Coupe_Pin 7         // entrée signal début de coupe GRBL (M3)
#define GRBL_pause_Pin 8    // sortie vers GRBL
#define GRBL_start_Pin 9    // sortie Bouton GRBL START / REPRISE
#define relais_start_arc_Pin 6     // sortie relais d'amorçage = mise à la masse de la buse de la torche plasma
#define relais_coupe_Pin 5  // sortie "spindle" inversée pour commande du relais de la torche

// variables utilisées pour la mesure de tension par l'ADC de l'Arduino
//unsigned int val1     = 0;
unsigned int val_tot = 0;
//unsigned int val2     = 0;
//unsigned int Val_Max  = 0;
//unsigned int Val_min  = 0;
//unsigned int Val_Ref  = 0;
int vMax = 0;
int vMin = 0;
int vArc = 0;
int vArc_new = 0;

//static unsigned int cpt = 0, val_moy;
static unsigned int cpt = 0;
unsigned long adcval = 0;
bool ok = false;

// Mouvements du moteur Z

bool thc_is_OFF = true;
int ordre = 0;        // consigne THC : 2 = monter, 1 = descendre, 0 = stop.
int sens_actuel = 0;  // mouvement en cours (valeurs idem consigne)

// unsigned long chrono = 0;
// unsigned long chrono_prec = 0;

volatile long n = 1;  // index du pas moteur dans la rampe d'accélération
volatile long i = 1;  // incrément pour calcul du pas moteur
//bool init_coupe = true;
int mode_calme = 1;

int counter = 0;   // pour codeuse
int sw_count = 1;  // idem
static uint8_t prevNextCode = 0;
static uint16_t store = 0;
bool start_modes = true;  // iintialisation menu codeuse

/*-------------------- mesure duree boucle ----------------------------*/
unsigned long ms;
unsigned long timelimit = 0;
/*---------------------------------------------------------------------*/

//int ordre2;
int move = 1;  // pour simulation

float accel;  // cf calcul Accel_stepper.ods
float vitesse;


bool debut_coupe = true;    // debut d'une nouvelle coupe (temporisation pour ARC_OK)
bool coupe_active = false;  // Broche active = M3 (torche plasma allumee)

unsigned long tempo_relais_coupe = 0;  // tempo pour rebonds relais cycle de coupe


/*-------------------- THC ----------------------------*/
#define RETARD_THC 400
unsigned long chrono_retard_THC = 0;  // retard THC avant lecture ARC_OK en debut de coupe
/*---------------------------------------------------------------------*/
