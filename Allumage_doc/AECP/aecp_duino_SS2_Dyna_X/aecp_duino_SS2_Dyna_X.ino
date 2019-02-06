#include <TimerOne.h> //Génère une interruption toutes les  trech ou Dsecu µs
#include <SoftwareSerial.h>
//**************************************************************
//Aecp-Duino    Allumage electronique programmable - Arduino - 2016
//Mr Loutrel Philippe, voir son site,
//http://a110a.free.fr/SPIP172/article.php3?id_article=142
//Mr Dedessus Les Moutier Christophe pour la version Panhard
char ver[] = "version du 13_12_2018";//Choix entre 3 types de Dwell.
//En option, connexion d'un sélecteur entre la patte A4 et A5 et la masse
//pour changer de courbe d'avance centrifuge et dépression
//Les datas envoyées pour Processing sont en fin de boucle loop
//T>14ms, correction Christophe.Avance 0°jusqu'a 500t/mn, anti retour de kick
//Detection et traitement special au demarrage (première etincelle)
//******************************************************************************
//Le module BlueTooth est le très courant HC 06 à moins de 5€
//Connecter la patte TX du module à D11, la patte RX du module à D12
//Connecter les masses et le Vcc du module au +5V du 7805
//Mettre le module BlueTooth en mode AT( pin 34 au +5V ) et entrer AT+UART=38400,1,0
//38400 bps entre module et smartphone
//Installer l'appli BlueTerm , connecter: sur l'ecran l'avance enn degrés est affichée

SoftwareSerial BT(11, 12); // RX| TX vers le module BlueTooth HC05/06

//************* ces lignes explique la lecture de la dépression ****************
// Pour la dépression ci-dessous le tableau des mesures relevés sur un banc Souriau
// Degdep = map((analogRead(A0)),xhigh,xlow,yhigh,ylow);
int xhigh = 0;// valeur ADC haute pour conversion de la valeur mmHg haute
int xlow = 0;// valeur ADC basse pour conversion de la valeur mmHg basse
int yhigh = 0;// valeur de la limite en degré haute x 10 pour avoir les centièmes
int ylow = 0;// valeur de la limite en degré basse soit 0 degré
// tableau des valeurs mmHg vs 8 bits
// {0,707}{25,667}{50,620}{60,601}{75,576}{80,565}{95,538}{100,530}{125,485}{150,438}{175,394}{185,373}{200,350}{210,330}{225,305}{250,260}{275,216}{300,172}
// relevé en activant la ligne Degdepcal et en ajoutant cette variable à l'édition dans le terminal série
// on peut en déduire une équation qui donne la valeur en mmHg au départ de la conversion ADC , utilisé sous Processing
// mmHg = (707 - valeur ADC)/1.78 ou 1.78 est la pente du capteur pour tension VS mmHg

//**************  Ces 6 lignes sont à renseigner obligatoirement.****************
// Ce sont : Na[],Anga[],  Ncyl, AngleCapteur, CaptOn, Dwell
//Les tableau Na[] et Anga[] doivent obligatoirement débuter puis se terminer par  0
//et  contenir des valeurs  entières >=1
//Le nombre de points est libre.L'avance est imposée à 0° entre 0 et Nplancher t/mn
//Le dernier N fixe la ligne rouge, c'est à dire la coupure de l'allumage
int Na[] = {0, 1320, 1600, 1800, 2000, 2260, 2500, 2680, 2900, 3040, 3220, 3400, 3520, 3600, 3760, 3880, 4000, 7000, 0};
//degrés d'avance vilebrequin correspondant soit en tour volant moteur ( attention ! ):
int Anga[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15, 0};

int Ncyl = 2;           //Nombre de cylindres, moteur Panhard
const int AngleCapteur = 90; //le capteur(Hall) est 45° avant le PMH habituellement
const int Avancestatique = 14; // Correspond à 4 dents d'avance statique sur volant moteur
int Avancedemarrage = 0;
const int CaptOn = 0;  //CapteurOn = 1 déclenche sur front montant (capteur saturé)
//CapteurOn = 0 déclenche sur front descendant (capteur non saturé).Voir fin du listing
//**********************************************************************************
const int Dwell = 1; //1 pour alimenter la bobine en permanence sauf 1ms/cycle.Elle doit pouvoir chauffer sans crainte
// 2 pour alimentation de la bobine seulement trech ms par cycle, 3ms par exemple,
//indispensable pour bobine 'electronique' ou  de faible resistance: entre 2 et 0.5ohm
//3 pour simuler un allumage à vis platinées: bobine alimentée 2/3 (66%) du cycle.
//************************************************************************************
const int Typedatasend = 3; // 0 pas de données transmises, 1 pour terminal série, 2 pour laptop avec bluetooth
// 3 pour smartphone avec bluetooth
//************************************************************************************
//*******************MULTICOURBES****IL FAUT TOURNER LE SELECTEUR!!!!!!!*******
//A la place de la courbe a, on peut selectionner la courbe b, c, d ou la e
//Un sélecteur rotatif et 4 résistances de 4K7, 18K, 47K et 100K font le job
//avec l'entrée configurée en Input Pull-up
//*******//*********Courbe   b
int Nb[] = {0, 800, 1000, 1320, 1600, 1800, 2040, 2200, 2400, 2600, 2800, 3000, 3140, 3300, 3440, 3560, 3680, 3800, 3900, 7000, 0}; //Courbe b 
int Angb[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 0};
//*******//*********Courbe   c
int Nc[] = {0, 1700, 2000, 2300, 2520, 2740, 2960, 3140, 3300, 3480, 3620, 3740, 3840, 3960, 4060, 7000, 0}; //Courbe c
int Angc[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 0};
//*******//*********Courbe   d
int Nd[] = {0, 800, 1400, 1800, 2000, 2260, 2500, 2740, 2960, 3200, 3360, 3560, 3720, 3880, 4060, 7000,0}; //Courbe d
int Angd[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 0};
//*******//*********Courbe   e
int Ne[] = {0, 1700, 1920, 2120, 2280, 2440, 2600, 2740, 2900, 3020, 3140, 3260, 3360, 3480, 3560, 3660, 3740, 3840, 3900, 7000, 0}; //Courbe e
int Ange[] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 0};
//**********************************************************************************
//********************FLASH********************
//Module flash 3W à Led (type Keyes) connecté sur A5, +5V et la masse
//Decommenter dans la fonction Etincelle ()la ligne 179 :
//digitalWrite(Flash, 1); delayMicroseconds(tFlash); digitalWrite(Flash, 0);
int tFlash = 200; //Durée du flash en µs 100, typique
//**********************************************************************************
//************Ces 4 valeurs sont eventuellement modifiables*****************
//Ce sont Nplancher, trech , Dsecu et delAv
const int Nplancher = 500; // vitesse en t/mn jusqu'a laquelle l'avance  = 0°
const int trech  = 6000;//temps de recharge bobine, 3ms= 3000µs typique
const int unsigned long Dsecu  = 1000000;//Securite: bobine coupee à l'arret apres Dsecu µs
int delAv = 1;//delta avance,par ex 2°. Quand Pot avance d'une position, l'avance croit de delAv

const int Bob1 = 8;    //Sortie D8 vers bobine1
const int Bob2 = 4;    //Sortie D4 vers bobine2
const int Cible = 2;  //Entrée sur D2 du capteur, R PullUp et interrupt
const int Pot1 = A4;   //Entrée analogique sur A4 pour potard de changement de courbes d'avance centrifuge. R PullUp
const int Pot2 = A5;   //Entrée analogique sur A5 pour potard de changement de courbes d'avance dépression. R PullUp
const int Flash = 11;  //Sortie D11 vers module Flash Led 3w  Keyes
const int Led = 13; //Sortie D13 avec la led built-in pour caller le disque par rapport au capteur
int unsigned long D = 0;  //Delai en µs à attendre après la cible pour l'étincelle
int unsigned long Ddep = 0;
int unsigned long Dsum = 0;
int valPot1 = 0;       //0 à 1023 selon la position du potentiomètre en entree
int valPot2 = 0;       //0 à 1023 selon la position du potentiomètre en entree
int milli_delay = 0;
int micro_delay = 0;
float RDzero = 0; //pour calcul delai avance 0° < vitesse seuil plancher
float  Tplancher = 0; //idem
const int tcor  = 380; //correction en µs  du temps de calcul pour D 120µs + 120µs de lecture de dépression + 140µs de traitement
int unsigned long Davant_rech = 0;  //Delai en µs après etincelle pour demarrer la recharge bobine.
int unsigned long Tprec  = 0;//Periode precedant la T en cours, pour calcul de Drech
int unsigned long prec_H  = 0;  //Heure du front precedent en µs
int unsigned long T  = 0;  //Periode en cours
int unsigned long Tant  = 0;  //Periode en cours précédente sauvée en cas de T < Tlim
int N1  = 0;  //Couple N,A de debut d'un segment
int Ang1  = 0; // Car A1 reservé pour entrée analogique!
int N2  = 0; //Couple N,A de fin de segment
int Ang2  = 0;
int*  pN = &Na[0];//pointeur au tableau des régimes. Na sera la courbe par defaut
int*  pA = &Anga[0];//pointeur au tableau des avances. Anga sera la  courbe par defaut
float k = 0;//Constante pour calcul de l'avance courante
float C1[20]; //Tableaux des constantes de calcul de l'avance courante
float C2[20]; //Tableaux des constantes de calcul de l'avance courante
float Tc[20]; //Tableau des Ti correspondants au Ni
//Si necessaire, augmenter ces 3 valeurs:Ex C1[30],C2[30],Tc[30]
int Tlim  = 0;  //Période minimale, limite, pour la ligne rouge
int j_lim = 0;  //index maxi des N , donc aussi  Ang
int unsigned long NT  = 0;//Facteur de conversion entre N et T à Ncyl donné
int AngleCibles = 0;//Angle entre 2 cibles, 180° pour 4 cyl, 120° pour 6 cyl, par exemple
int UneEtin = 1; //=1 pour chaque étincelle, testé par isr_CoupeI et remis à zero
int Ndem = 60;//Vitesse estimée du vilo entrainé par le demarreur en t/mn
int unsigned long Tdem  = 0;  //Periode correspondante à Ndem,forcée pour le premier tour
int Mot_OFF = 0;//Sera 1 si moteur detecté arrété par l'isr_GestionIbob()
float   AV = 0; //Avance en degrès pour transmission vesr module BlueTooth
float   AVtot = 0; //Avance totale en degrés pour transmission vesr module BlueTooth
String Message = ""; //Données renvoyer au smartphone ou pc via bluetooth ou usb

float uspardegre = 0;
int Dep = 0;
float Degdep = 0;
int unsigned long Vitesse  = 0;  //vitesse en cours
float Delaideg  = 0;  //µs/deg pour la dépression
// Tableau pour sauver des données temporelle
unsigned long Stop_temps;
unsigned long Tempsecoule = 0;

// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

//Macro  ps(v) de debug pour imprimer le numero de ligne, le nom d'une variable, sa valeur
//puis s'arreter definitivement .
#define ps(v) Serial.print("Ligne_") ; Serial.print(__LINE__) ; Serial.print(#v) ; Serial.print(" = ") ;Serial.println((v)) ; Serial.println("  Sketch stop"); while (1);
//Exemple, à la ligne 140, l'instruction     ps(var1);
//inprimera  "Ligne_140var1 = 18  Sketch stop"

//Macro  pc(v)de debug pour imprimer le numero de ligne, le nom d'une variable, sa valeur,
//puis s'arreter et attendre un clic de souris  sur le bouton 'Envoyer'en haut de l'ecran seriel pour continuer.
#define pc(v) Serial.print("Ligne_") ; Serial.print(__LINE__) ; Serial.print(#v) ;Serial.print(" = ") ; Serial.println((v)) ; Serial.println(" Clic bouton 'Envoyer' pour continuer") ;while (Serial.available()==0);{ int k_ = Serial.parseInt() ;}
//Exemple, à la ligne 145, l'instruction    pc(var2);
// inprimera   "Ligne_145var2 = 25.3   Clic bouton 'Envoyer' pour continuer"

int deb = 0; //0 sinon 1 ou 2 pour le debugging,et executer les impressions pc() ou pc(). Voir Setup()

//********************LES FONCTIONS*************************

void  CalcD ()////////////////////while (1); delay(1000);/////////////////////////////
// Noter que T1>T2>T3...
{ for (byte j = 1; j <= j_lim; j++)//On commence par T la plus longue et on remonte
  { //Serial.print("Tc = "); Serial.println(Tc[j]);delay(1000);
    if  (T >=  Tc[j]) {     //on a trouvé le bon segment de la courbe d'avance
      D = float(T * C1[j]  + C2[j]);//D en µs, C2 est déjà diminué du temps de calcul~50mus
      if ( T > Tplancher)D = T * RDzero;
      //break;  //Sortir, on a D
      Ddep = (Degdep * Delaideg ) ;//Ddep en µs
      Dsum = D - Ddep - tcor ;// tcor appliqué ici donne de meilleurs résultats niveau précision
      //if (deb > 1) {  pc(C1[j] );  pc(C2[j] );pc(D);}
      break;
    }
  }
}
void  Etincelle ()//////////while (1); delay(1000);/////////////////////////////
//{gf = 1; while (gf < D/14)gf++;//10= 100µs,100=1.1ms,2000=21.8ms//attente possible
{
  if (Dsum < 6500) {
    delayMicroseconds(Dsum);
  }
  else {
    milli_delay = ((Dsum / 1000) - 2); //Quand D >10ms car problèmes avec delayMicroseconds(D) si D>14ms!
    micro_delay = (Dsum - (milli_delay * 1000)); //
    delay(milli_delay); // Currently, the largest value that will produce an accurate delay is 16383 µs
    delayMicroseconds(micro_delay);
  }
  digitalWrite(Bob1, 0); //Couper le courant, donc étincelle
  digitalWrite(Bob2, 0); //Couper le courant, donc étincelle
  Stop_temps = micros();
  //digitalWrite(Flash, 1); delayMicroseconds(tFlash); digitalWrite(Flash, 0);
  switch (Dwell)  //Attente courant coupé selon le type de Dwell
  { case 1:       //Ibob coupe 1ms par cycle seulement, la bobine peut chauffer
      Davant_rech = 1200; //1ms2 off par cycle
      break;

    case  2:      //Type bobine faible resistance, dite "electronique"
      Davant_rech = 2 * T - Tant - trech;//On doit enir compte des variations de régime moteur
      break;

    case  3:      //Type "vis platinées", Off 1/3, On 2/3
      Davant_rech = T / 3;
      break;
  }
  Timer1.initialize(Davant_rech);//Attendre Drech µs avant de retablire le courant dans la bobine
  UneEtin = 1; //Signaler une étincelle à l'isr_GestionIbob().
}
void  Init ()////////////////////while (1); delay(1000);/////////////////////////////
{ AngleCibles = 720 / Ncyl; //Ex pour 4 cylindres 180°, et 120° pour 6 cylindres
  NT  = 120000000 / Ncyl; //Facteur de conversion Nt/mn en Tµs
  Tdem  = NT / Ndem; //Periode de la première étincelle
  Tplancher = 120000000 / Nplancher / Ncyl; //T à  vitesse plancher en t/mn, en dessous, avance centrifuge = 0
  RDzero = float(AngleCapteur - Avancestatique - Avancedemarrage) / float(AngleCibles);
  Select_Courbe();  //Ajuster éventuellement les pointeurs pN et pA pour la courbe a,b,c,d ou e
  Select_Courbe_depression(); // Sélectionne pour la courbe a,b,c,d ou e de dépression
  N1  = 0; Ang1 = 0; //Toute courbe part de  0
  int i = 0;    //locale mais valable hors du FOR
  pN++; pA++; //sauter le premier element de tableau, toujours =0
  for (i  = 1; *pN != 0; i++)//i pour les C1,C2 et Tc.Arret quand regime=0.
    //pN est une adresse (pointeur) qui pointe au tableau N.Le contenu pointé est *pN
  { N2 = *pN; Ang2 = *pA;//recopier les valeurs pointées dans N2 et Ang2
    k = float(Ang2 - Ang1) / float(N2  - N1);
    C1[i] = float(AngleCapteur - Avancestatique - Ang1 + k * N1) / float(AngleCibles);
    C2[i] = -  float(NT * k) / float(AngleCibles);
    Tc[i] = float(NT / N2);
    N1 = N2; Ang1 = Ang2; //fin de ce segment, début du suivant
    pN++; pA++;   //Pointer à l'element suivant de chaque tableau
  }
  j_lim = i - 1; //Revenir au dernier couple entré
  Tlim  = Tc[j_lim]; //Ligne rouge
  Serial.print("Tc = "); for (i = 1 ; i < 12; i++)Serial.println(Tc[i]);
  Serial.print("Tlim = "); Serial.println(Tlim);
  Serial.print("C1 = "); for (i = 1 ; i < 12; i++)Serial.println(C1[i]);
  Serial.print("C2 = "); for (i = 1 ; i < 12; i++)Serial.println(C2[i]);

  //Timer1 a deux roles:
  //1....couper le courant dans la bobine en l'absence d'etincelle pendant plus de Dsecu µs
  //2... après une étincelle, attendre le delais Drech avant de retablir le courant dans la bobine
  //Ce courant n'est retabli que trech ms avant la prochaine étincelle, condition indispensable
  //pour une bobine à faible resistance, disons inférieure à 3 ohms.Typiquement trech = 3ms.
  Timer1.attachInterrupt(isr_GestionIbob);//IT d'overflow de Timer1 (16 bits)
  //Timer1.initialize(Dsecu);//Le courant dans la bobine est coupé si aucune etincelle durant Dsecu µs
  Mot_OFF = 1;
}
void  isr_GestionIbob()////////////////////while (1); delay(1000);/////////////////////////////
{ Timer1.stop();    //Arreter le decompte du timer
  if (UneEtin == 1) {
    digitalWrite(Bob1, 1);  //Retablire le courant dans bobine
    digitalWrite(Bob2, 1);  //Retablire le courant dans bobine
  }
  else
  { digitalWrite(Bob1, 0);//Preserver la bobine, couper le courant
    digitalWrite(Bob2, 0);//Preserver la bobine, couper le courant
    Mot_OFF = 1;//Permettra à loop() de detecter le premier front de capteur
  }
  UneEtin = 0;  //Remet  le detecteur d'étincelle à 0
  //Timer1.initialize(Dsecu);//Au cas où le moteur s'arrete, couper la bobine apres Dsecu µs
}
void  Select_Courbe()////////////while (1); delay(1000);/////////////////////////////
//Par défaut, la courbe a est déja selectionnée
{ valPot1 = analogRead(Pot1);
  Serial.print("Le selecteur 1 = "); Serial.print(valPot1); Serial.print(" ,Centrifuge = ");
  if (valPot1 < 99) {                  // Shunt 0 ohm donne 15
    Serial.println(" Courbe a");
  }
  if (valPot1 > 110 && valPot1 < 150) { // Résistance de 4K7 donne 130
    pN = &Nb[0];  // pointer à la courbe b
    pA = &Angb[0];
    Serial.println(" Courbe b");
  }
  if (valPot1 > 320 && valPot1 < 360) {  // Résistance de 18K donne 340
    pN = &Nc[0];  // pointer à la courbe c
    pA = &Angc[0];
    Serial.println(" Courbe c");
  }
  if (valPot1 > 545 && valPot1 < 585) {  // Résistance de 47K donne 565
    pN = &Nd[0];  // pointer à la courbe c
    pA = &Angd[0];
    Serial.println(" Courbe d");
  }
  if (valPot1 > 715 && valPot1 < 755) {  // Résistance de 100K donne 735
    pN = &Ne[0];  // pointer à la courbe c
    pA = &Ange[0];
    Serial.println(" Courbe e");
  }
  if (valPot1 > 995) {                  // Pas de shunt donne 1015
    Serial.println(" Courbe a");
  }
}
void  Select_Courbe_depression()////////////while (1); delay(1000);/////////////////////////////
{ valPot2 = analogRead(Pot2);
  Serial.print("Le selecteur 2 = "); Serial.print(valPot2); Serial.print(" ,Depression = ");
  if (valPot2 < 99) {                  // Shunt 0 ohm donne 15 
    xhigh = 205; // soit 282 mmHg = 205
    xlow = 520;  // soit 105 mmHg = 520
    yhigh = 300; // soit 30°
    ylow = 0;    // soit 0°
    Serial.println(" Courbe a");
  }
  if (valPot2 > 110 && valPot2 < 150) { // Résistance de 4K7 donne 130 
    xhigh = 209; // soit 280 mmHg = 209
    xlow = 547;  // soit 90 mmHg = 547
    yhigh = 320; // soit 32°
    ylow = 0;    // soit 0°
    Serial.println(" Courbe b");
  }
  if (valPot2 > 320 && valPot2 < 360) {  // Résistance de 18K donne 340    
    xhigh = 201; // soit 284 mmHg = 201
    xlow = 493;  // soit 120 mmHg = 493
    yhigh = 280; // soit 28°
    ylow = 0;    // soit 0°
    Serial.println(" Courbe c");
  }
  if (valPot2 > 545 && valPot2 < 585) {  // Résistance de 47K donne 565    
    xhigh = 201; // soit 284 mmHg = 201
    xlow = 547;  // soit 90 mmHg =547
    yhigh = 280; // soit 28°
    ylow = 0;    // soit 0°
    Serial.println(" Courbe d");
  }
  if (valPot2 > 715 && valPot2 < 755) {  // Résistance de 100K donne 735    
    xhigh = 209; // soit 280 mmHg
    xlow = 493;  // soit 120 mmHg
    yhigh = 320; // soit 32°
    ylow = 0;    // soit 0°
    Serial.println(" Courbe e");
  }
  if (valPot2 > 995) {                  // Pas de shunt donne 1015 
    xhigh = 205; // soit 282 mmHg
    xlow = 520;  // soit 105 mmHg
    yhigh = 300; // soit 30°
    ylow = 0;    // soit 0°
    Serial.println(" Courbe a");
  }
}
////////////////////////////////////////////////////////////////////////
void setup()//////////////////while (1); delay(1000);//////////////////////////
/////////////////////////////////////////////////////////////////////////
{ deb = 0; //pour debugging, 1 ou 2 sinon 0
  Serial.begin(115200);
  BT.begin(115200);//Vers module BlueTooth HC05/06
  BT.flush();//A tout hasard Ligne suivante, 3 Macros du langage C
  Serial.println(__FILE__); Serial.println(__DATE__); Serial.println(__TIME__);
  Serial.println(ver);
  if (Ncyl < 2)Ncyl = 2; //On assimile le mono cylindre au bi, avec une étincelle perdue
  pinMode(Cible, INPUT_PULLUP); //Entrée interruption sur D2, front descendant
  pinMode(Bob1, OUTPUT); //Sortie sur D4 controle du courant dans la bobine1
  pinMode(Bob2, OUTPUT); //Sortie sur D6 controle du courant dans la bobine2
  digitalWrite(Bob1, 0);//par principe, couper la bobine
  digitalWrite(Bob2, 0);//par principe, couper la bobine
  //Nota: on peut connecter une Led de controle sur D4 avec R=330 ohms vers la masse
  pinMode(Pot1, INPUT_PULLUP); //Entrée pour potard 100kohms ou sélecteur rotatif avec résistances, optionnel !
  pinMode(Pot2, INPUT_PULLUP); //Entrée pour potard 100kohms ou sélecteur rotatif avec résistances, optionnel !
  // set up the ADC
  ADCSRA &= ~PS_128;  // remove bits set by Arduino library
  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_64;
  // set our own prescaler to 64
  Init(); // Executée une fois au demarrage et à chaque changement de courbe
  pinMode(Led, OUTPUT); // pour signaler le calage du capteur lors de la mise au point
  pinMode(Flash, OUTPUT);//Sortie vers flash, patte S
}


///////////////////////////////////////////////////////////////////////////
void loop()   /////////////////////while (1); delay(1000);/////////////////
////////////////////////////////////////////////////////////////////////////
{
  while (digitalRead(Cible) == !CaptOn); //Attendre front actif de la cible
  T = micros() - prec_H;    //front actif, arrivé calculer T
  prec_H = micros(); //heure du front actuel qui deviendra le front precedent
  digitalWrite(Led,LOW); // Décommenter cette ligne pour caller le capteur , voir plus loin la 2ème ligne 409
  Dep = analogRead(A0);
  Degdep = map(Dep, xhigh, xlow, yhigh, ylow); //Mesure la dépression
  Degdep = Degdep / 10;
  //int Degdepcal = analogRead(A0); Pour calibrage du capteur MV3P5050
  if (Degdep < 0) {
    Degdep = 0;
  }
  else if (Degdep > (yhigh / 10)) {
    Degdep = (yhigh / 10);
  }
  else ;
  Vitesse = NT / T;
  Delaideg = NT / Vitesse / float(AngleCibles);
  if ( Mot_OFF == 1 ) {
    T = Tdem;//Fournir  T = Tdem car prec_H n'existe par pour la première étincelle
    digitalWrite(Bob1, 1);  //Retablire le courant dans bobine
    digitalWrite(Bob2, 1);  //Retablire le courant dans bobine
    Mot_OFF = 0; //Le moteur tourne
  }
  if (T > Tlim)     //Sous la ligne rouge?
  { CalcD(); // Top();  //Oui
    Etincelle();
  }
  if (T < Tlim)     //Au dessus de la ligne rouge?
  {
    T = Tant; // fige la vitesse sous 7000 T/min ainsi que le délai
    CalcD(); // Top();  //Oui
    Etincelle();
    Vitesse = NT / Tant;
    
  }
  while (digitalRead(Cible) == CaptOn); //Attendre si la cible encore active

  Tempsecoule = Stop_temps - prec_H ;
  Tant = T; // Sauve la valeur de T en cas de perte d'info du capteur
  digitalWrite(Led,HIGH);

  senddata();

}

void senddata(){

  switch (Typedatasend)
  {
    case 0:
    break;
    
    case 1:
    
     Message = ("S,"+String(Vitesse)+(",")+String(Degdep,1)+(",")+String(D)+(",")+String(int(Delaideg))+(",")+String(Tempsecoule)+(",")+String(Dep)+(","));
     
     Serial.println(Message);
    break;

    case 2:
    
     Message = ("S,"+String(Vitesse)+(",")+String(Degdep,1)+(",")+String(D)+(",")+String(int(Delaideg))+(",")+String(Tempsecoule)+(",")+String(Dep)+(","));
    
     BT.println(Message);
     
    break;

    case 3:

     AV = AngleCapteur - (D /float(Delaideg)) - Avancestatique ;
     AVtot = AV + Degdep + Avancestatique;
     Message = ("S,"+String(Vitesse)+(",")+String(Degdep,1)+(",")+String(AV, 1)+(",")+String(AVtot,1)+(",")+String(Dep)+(","));
    
     BT.println(Message);
     
    break;
  }
}

////////////////DEBUGGING////////////////////////
//Voir les macros ps ()à et pc() en début de listing

//Une autre possibilité est de générer ou non du code de debug à la compilation.
//#define DEBUG    // #if defined DEBUG #endif

//Hertz = Nt/mn / 30 , pour moteur 4 cylindre
//N 1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000
//Hz  33, 50,   66,  83  100  117   133 150   166 183 200    216  233 250   266 283   300 316 333

//Hertz = Nt/mn / 60 , pour moteur 2 cylindre
//N 1000,1500,2000,2500,3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,8000,8500,9000,9500,10000
//Hz  16,  25,  33,  41   50   58   66   75   83   91  100  108  116  125  133  141  150  158   166
//Capteur Honeywell 1GT101DC,contient un aimant sur le coté,type non saturé, sortie haute à vide,
//et basse avec une cible en métal. Il faut  CapteurOn = 0, declenche sur front descendant.
//Le capteur à fourche SR 17-J6 contient un aimant en face,type saturé, sortie basse à vide,
//et haute avec une cible métallique. Il faut  CapteurOn = 1, declenche sur front montant.


