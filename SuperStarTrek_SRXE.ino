// Super Star Trek V2 May 16, 1978
// Original program by Mike Mayfield
// Modified by R.C. Leedom
// Modified 2023 by Dan Geiger for arduino and Smart Response Xe
//
// To be consistant with the original, the coordinates are 1-8 for
// Quadrants and Sectors. For ease of programming, the C 0-indexed
// arrays are used. This is seemless for data manipulation but you
// must add 1 to the values of Q1, Q2, S1, S2 when displaying to
// the user to match their percieved values.
#include <SmartResponseXE.h>

void (*resetFunc)(void) = 0;

int G[8][8];                   // The Galaxy (Quadrants)
byte S[8][8];                  // The Sector
boolean Z[8][8];               // Track where you have visited
double D[8];                   // Damage matrix
int Kl[3][3];                  // Klingon position and energy
int E;                         // Energy Level
int Sh;                        // Shield level
double T;                         // current stardate
double T9;                        // Game time (rnd 25-35) add 50 to extend game
int P;                         // number of photon torpedoes
int B;                         // Number of bases
int B3;                        // Number of bases in quadrant
int K;                         // Ttoal Number of Kilingons
int K3;                        // Number of Kligons in quadrant
int Q1;                        // X value of quadrant
int Q2;                        // Y value of quadrant
int S1;                        // X value of Sector
int S2;                        // Y value of Sector
int S3;                        // Number of stars in quadrant
int kAttackFactor = 2;         // Klingon odds of causing damage (2/10)
int boundaryFactor = 2;        // Odds of damage from boundary hit (2/10)
double klingonStrength = 1.0;  // Klingom damage factor

void setup() {
  TRXPR = 1 << SLPTR;          // send transceiver to sleep
  SRXEInit(0xe7, 0xd6, 0xa2);  // initialize and clear display // CS, D/C, RESET
  initGalaxy();
  printSRS();
}

void loop() {

  dispMainMenu();
  byte curCom = waitForMyKey();

  switch (curCom) {

    case 20:  //PWR
      SRXESleep();
      printStat();
      printSRS();
      break;
    case 21:  //OPT
      showOptions();
      break;
    case 22:  //COM
      modeCom();
      break;
    case 23:  //DAM
      dispDamage();
      break;
    case 24:  //SHE
      modeShields();
      printSRS();
      break;
    case 25:  //NAV
      {
        SRXEFill(0);
        modeMove();
        if (isDocked()) baseDock();
        printSRS();
      }
      break;
    case 26:  //SRS
      {
        printSRS();
      }
      break;
    case 27:  //LRS
      printLRS();
      break;
    case 28:  //PHA
      modePhaser();
      printSRS();
      break;
    case 29:  //TOR
      modeTorpedo();
      printSRS();
      break;
  }
  // Check for end of game (Time runs out, Energy depleted)
  if (E + Sh <= 0) endGame(1);
  if (K == 0) endGame(4);
  if (T >= T9) endGame(3);
  // Update damage data
}

void initGalaxy() {
  introText1();

  T = random(20, 40) * 100;  // Arbitrary start date
  T9 = 40 + int(random(10)) + T;  // Stardate to complete mission

  Q1 = int(random(0, 8));  // Start quadrant
  Q2 = int(random(0, 8));
  S1 = int(random(2, 7)); // Start sector
  S2 = int(random(2, 7));

  // Fill the galaxy
  for (int i = 0; i < 8; i++) {
    D[i] = 0.0;
    for (int j = 0; j < 8; j++) {
      Z[i][j] = false;
      K3 = 0;
      B3 = 0;
      int R1 = int(random(1, 101));
      if (R1 > 80) K3 = K3 + 1;
      if (R1 > 95) K3 = K3 + 1;
      if (R1 > 98) K3 = K3 + 1;
      if (int(random(1, 101) > 96)) {
        B3 = 1;
        B = B + 1;
      }
      K = K + K3;
      G[i][j] = (K3 * 100) + (B3 * 10) + int(random(2, 9));
    }
  }
  if (K > T9) T9 = K + 25;
  // Check to make sure you have a base somewhere
  if (B == 0) {
    if (G[Q1][Q2] < 200) {
      G[Q1][Q2] = G[Q1][Q2] + 100;
      K = K + 1;
      B = 1;
      G[Q1][Q2] = G[Q1][Q2] + 10;
    }
  }

  Q1 = int(random(2, 7)); // Start quadrant
  Q2 = int(random(2, 7));

  E = 3000;  //initial energy level
  P = 10;    // initial photon torpedoes
  Sh = 0;    // no power allocated to the shields
  updateLRS();
  updateSector();

  introText2();
}
