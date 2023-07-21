void updateDamage(double starD) {
  // Run this anytime you update the stardate to apply repair to systems
  for (int i = 0; i < 8; i++) {
    if (D[i] < 0) {
      D[i] += starD;
      if (D[i] > 0) D[i] = 0;
    }
  }
}

void modeCom() {

  if (D[7] < 0) {
    dispText("Spock", "Computer is damaged!", "Repairs are in", "progress.");
    return;
  }

  dispComMenu();

  byte sel = waitForMyKey();

  switch (sel) {
    case 25:  // Galaxy Map
      printGalaxy();
      break;
    case 26:  // Status
      printStat();
      printSRS();
      break;
    case 27:  // Nav to Base
      {
        if (B3 == 0) {
          dispText("Spock", "There is no base", "in this sector!", "");
          return;
        }
        SRXEFill(0);
        for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 8; j++) {
            if (S[j][i] == 4) {  // You found a base
              char buf[20];
              snprintf(buf, sizeof(buf) - 1, "Base at %i - %i", j + 1, i + 1);
              SRXEWriteString(21, 20, buf, FONT_MEDIUM, 3, 0);
              double aCourse = atan2(j - S1, S2 - i);
              aCourse = aCourse * 180 / PI;
              if (aCourse < 0) aCourse += 360;
              snprintf(buf, sizeof(buf) - 1, "Vector: %.1f", aCourse);
              SRXEWriteString(21, 38, buf, FONT_MEDIUM, 3, 0);
              double distB = sqrt(pow((i - S2), 2) + pow((j - S1), 2));
              snprintf(buf, sizeof(buf) - 1, "Range: %.3f", distB / 8.0);
              SRXEWriteString(21, 56, buf, FONT_MEDIUM, 3, 0);
            }
          }
        }
        SRXEWriteString(318, 122, "DONE>", FONT_LARGE, 3, 0);
        waitForMyKey();
        SRXEFill(0);
        printSRS();
      }
      break;
    case 28:  // Targetting Computer
      {
        if (K3 == 0) {
          dispText("Chekov", "There are no Klingons", "in this sector!!", "");
          printSRS();
          return;
        }
        double aCourse;
        double distK;

        for (int i = 0; i < 3; i++) {
          if (Kl[i][2] > 0) {  // You found a Klingon
            SRXEFill(0);
            char buf[20];
            snprintf(buf, sizeof(buf) - 1, "Klingon at %i - %i", Kl[i][0] + 1, Kl[i][1] + 1);
            SRXEWriteString(42, 30, buf, FONT_MEDIUM, 3, 0);
            aCourse = atan2(Kl[i][0] - S1, S2 - Kl[i][1]);
            aCourse = aCourse * 180 / PI;
            if (aCourse < 0) aCourse += 360;
            snprintf(buf, sizeof(buf) - 1, "Vector: %.1f", aCourse);
            SRXEWriteString(42, 48, buf, FONT_MEDIUM, 3, 0);
            distK = sqrt(pow((Kl[i][1] - S2), 2) + pow((Kl[i][0] - S1), 2));
            snprintf(buf, sizeof(buf) - 1, "Range: %.3f", distK / 8.0);
            SRXEWriteString(42, 66, buf, FONT_MEDIUM, 3, 0);
            SRXEWriteString(318, 122, "DONE>", FONT_LARGE, 3, 0);
            waitForMyKey();
          }
        }
        if (random(1, 11) < kAttackFactor) klingonAttacks(closeKlingon());

        SRXEFill(0);
        printSRS();
      }
      break;
    default:
      SRXEFill(0);
      printSRS();
      break;
  }
}

void baseDock() {
  SRXEFill(0);
  SRXEWriteString(0, 2, "You are docked at the Starbase.", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 18, "Photon torpedoes loaded.", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 34, "500 energy units added.", FONT_MEDIUM, 3, 0);

  P = 10;
  E = E + 500;

  double damNum = 0.0;
  for (int i = 0; i < 8; i++) damNum += D[i];
  damNum = -damNum;

  if (damNum == 0.0) {
    SRXEWriteString(9, 75, "You have no damage to repair.", FONT_MEDIUM, 3, 0);
    SRXEWriteString(318, 122, "Exit>", FONT_LARGE, 3, 0);
    waitForMyKey();
    SRXEFill(0);
    return;
  }

  char buf[25];
  snprintf(buf, sizeof(buf) - 1, "Damage will take %.1f SDs", damNum / 2.0);
  SRXEWriteString(0, 70, buf, FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 86, "to repair. Repair Damage?", FONT_MEDIUM, 3, 0);
  SRXEWriteString(318, 122, " NO>", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 122, "<YES", FONT_LARGE, 3, 0);
  byte noth = waitForMyKey();
  if (noth == 29) {
    SRXEFill(0);
    return;
  }
  T = T + damNum / 2.0;
  for (int i = 0; i < 8; i++) D[i] = 0;
  dispText("Scotty", "Repairs are complete.", "", "");
}

boolean clearPath(int kNum) {
  // check to see if a star or base is in the way
  float aCourse = atan2(Kl[kNum][0] - S1, S2 - Kl[kNum][1]);
  aCourse = aCourse * 180 / PI;
  if (aCourse < 0) aCourse += 360;
  // if not on a vertical, horizontal, or diagnal course, you are ok
  if (int(aCourse) % 45 != 0) return true;

  // if you are here, you are firing on a course that could have an obstruction
  if (aCourse == 0) {
    for (int i = Kl[kNum][1] + 1; i < S2; i++) {
      if (S[S1][i] > 2) return false;  // >2 would be a star or base
    }
  }

  if (aCourse == 180) {
    for (int i = S2 + 1; i < Kl[kNum][1]; i++) {
      if (S[S1][i] > 2) return false;
    }
  }

  if (aCourse == 90) {
    for (int i = S1 + 1; i < Kl[kNum][0]; i++) {
      if (S[i][S2] > 2) return false;
    }
  }

  if (aCourse == 270) {
    for (int i = Kl[kNum][0] + 1; i < S2; i++) {
      if (S[i][S2] > 2) return false;
    }
  }

  if (aCourse == 45) {
    for (int i = 1; i < Kl[kNum][0] - S1; i++) {
      if (S[S1 + i][S2 - i] > 2) return false;
    }
  }

  if (aCourse == 135) {
    for (int i = 1; i < Kl[kNum][0] - S1; i++) {
      if (S[S1 + i][S2 + i] > 2) return false;
    }
  }

  if (aCourse == 225) {
    for (int i = 1; i < S1 - Kl[kNum][0]; i++) {
      if (S[S1 - i][S2 + i] > 2) return false;
    }
  }

  if (aCourse == 315) {
    for (int i = 1; i < S1 - Kl[kNum][0]; i++) {
      if (S[S1 - i][S2 - i] > 2) return false;
    }
  }

  // if you are here there is no obstruction
  return true;
}

boolean isDocked() {
  // Just count the N,E,S,W positions
  //North
  if (S2 && (S[S1][S2 - 1] == 4)) return true;

  //West
  if (S1 && (S[S1 - 1][S2] == 4)) return true;
  //East
  if (S1 < 7 && (S[S1 + 1][S2] == 4)) return true;

  //South
  if (S2 < 7 && (S[S1][S2 + 1] == 4)) return true;

  return false;
}

void modePhaser() {
  if (D[3] < 0) {
    dispText("Chekov", "Phasers Inoperable!!", "", "");
    return;
  }
  if (!K3) {
    dispText("Spock", "Sensors show no enemy", "ships in the sector!!", "");
    return;
  }

  SRXEFill(0);

  if (D[7] < 0) dispText("Spock", "Computer damage will", "affect accuracy!.", "");

  char buf[20];
  snprintf(buf, sizeof(buf) - 1, "You have %i units", E);
  dispText("Sulu", "Phasers locked on target.", buf, "of energy to use.");

  int phE = (int)getFloat("Enter Phaser Energy");

  if (phE > E) {
    dispText("Spock", "We do not have enough", "energy for the attack.", "");
    return;
  }

  // Attack each Klingon
  for (int i = 0; i < 3; i++) {
    // Divide energy between klingons
    int phas = phE / K3;
    // Calculate the distance to Klingon
    float distK = sqrt(pow((Kl[i][1] - S2), 2) + pow((Kl[i][0] - S1), 2));
    if (distK > 4) phas = int(float(phas) * 0.8);  // >4 units, use 80% energy
    // Reduce effectiveness if Computer is damaged
    if (D[7] < 0) phas = int(float(phas) * 0.8);
    if (Kl[i][2] != 0) {
      if (!clearPath(i)) {
        snprintf(buf, sizeof(buf) - 1, "Klingon at %i - %i", Kl[i][0] + 1, Kl[i][1] + 1);
        dispText("Chekov", buf, "is obstructed", "Can not target.");
      } else {
        E -= phas;
        Kl[i][2] -= phas;
        if (Kl[i][2] <= 0) {
          //Klingon destroyed
          char buf2[20];
          snprintf(buf, sizeof(buf) - 1, "Klingon at %i - %i", Kl[i][0] + 1, Kl[i][1] + 1);
          snprintf(buf2, sizeof(buf2) - 1, "Klingons left %i", K3 - 1);
          dispText("Chekov", buf, "was destroyed!", buf2);
          int x = Kl[i][0];
          int y = Kl[i][1];
          S[x][y] = 0;
          K3 -= 1;
          K -= 1;
          Kl[i][2] = 0;
          G[Q1][Q2] -= 100;
        } else {
          // Klingon shoots back
          snprintf(buf, sizeof(buf) - 1, "%i units of damage..", phas);
          dispText("Chekov", "Klingon suffers", buf, "Klingon returns fire.");

          klingonAttacks(i);
        }
      }
    }
  }
}

void randomDamage() {
  int dNum = random(0, 8);
  D[dNum] -= random(4, 8);
  dispText("Spock", "Sir, the ship has", "sustained damage.", "");
}

void klingonAttacks(int kNum) {
  char buf[21];
  snprintf(buf, sizeof(buf) - 1, "Klingon at %i - %i", Kl[kNum][0] + 1, Kl[kNum][1] + 1);
  dispText("Chekov", buf, "attacks!!", "");

  int powK = random(25, 100) * klingonStrength;
  float distK = sqrt(pow((Kl[kNum][1] - S2), 2) + pow((Kl[kNum][0] - S1), 2));
  if (distK > 4) powK = int((float)powK * 0.8);  // >4 units, use 80% energy

  if (Sh >= 0) {
    snprintf(buf, sizeof(buf) - 1, "%i units of energy.", powK);
    dispText("Chekov", "The shields have absorbed", buf, "");
  }

  if (random(1, 10) < kAttackFactor) randomDamage();
  Sh -= powK;
  checkShield();
  checkEnergy();
}

void checkShield() {  // check to see if shields are still up
  if (Sh <= 0) {
    E = E + Sh * 2;  // make up by taking twice deficit from energy
    dispText("Scottie", "", "Shields are depleted!", "");
    Sh = 0;
    randomDamage();  // Take damage for this
  } else {
    char buf[20];
    snprintf(buf, sizeof(buf) - 1, "at %i units.", Sh);
    dispText("Scottie", "Shields are holding", buf, "");
  }
}

void checkEnergy() {
  if (E <= 0) endGame(2);
}

void modeTorpedo() {

  if (!K3) {
    dispText("Spock", "Sensors show no enemy", "ships in the sector!!", "");
    return;
  }

  if (P < 1) {
    dispText("Chekov", "We are out of", "torpedoes!", "");
    return;
  }

  if (E < 20) {
    dispText("Scottie", "We don't have the", "power, Captain!", "");
    return;
  }

  if (D[4] < 0) {
    dispText("Chekov", "Torpedo tubes are", "inoperable due to", "damage.");
    return;
  }

  SRXEFill(0);

  int tCourse = (int)getFloat("Enter Course");

  boolean kHit = false;

  for (int i = 0; i < 3; i++) {
    if (Kl[i][2] > 0 && !kHit) {
      float aCourse = atan2(Kl[i][0] - S1, S2 - Kl[i][1]);
      aCourse = aCourse * 180 / PI;
      if (aCourse < 0) aCourse += 360;
      if (abs(aCourse - tCourse) <= 2) {
        if (clearPath(i)) {
          torpAnim();
          P -= 1;   // Subtract a torpedo
          E -= 20;  // Subtract power
          Kl[i][2] -= 200;
          kHit = true;
          if (Kl[i][2] <= 0) {  // You destroyed the Klingon
            char buf[20];
            snprintf(buf, sizeof(buf) - 1, "Klingon at %i - %i", Kl[i][0] + 1, Kl[i][1] + 1);
            dispText("Chekov", buf, "was destroyed!!", "");

            // Remove Klingon
            Kl[i][2] = 0;               // Klingon energy to zero
            G[Q1][Q2] -= 100;           // One less Klingon in the quadrant
            K3 -= 1;                    // One less Klingon in sector
            K -= 1;                     // One less Klingon in the game
            S[Kl[i][0]][Kl[i][1]] = 0;  // update the sectormap

            // if there is another Klingon, he attacks
            int cKling = closeKlingon();
            if (cKling != -1) {
              klingonAttacks(cKling);
            }
          } else {
            char buf[20];
            snprintf(buf, sizeof(buf) - 1, "Klingon at %i - %i", Kl[i][0] + 1, Kl[i][1] + 1);
            dispText("Chekov", buf, "was hit and damaged!", "He returns fire.");
            klingonAttacks(i);
          }
        } else {
          dispText("Chekov", "Torpedo course is obstructed!", "Can not fire.", "");
          return;
        }
      }
    }
  }

  if (!kHit) {
    torpAnim();
    dispText("Chekov", "Torpedo missed!", "The Klingon fires back.", "");
    P -= 1;   // Subtract a torpedo
    E -= 20;  // Subtract power
    int cKling = closeKlingon();
    if (cKling != -1) klingonAttacks(cKling);
    if (random(1, 11) < kAttackFactor) randomDamage();
  }
}

int closeKlingon() {
  // Returns the index of the closest Klingon or -1 if there are none
  int cK = 5;
  float distK = 100;
  for (int i = 0; i < 3; i++) {
    if (Kl[i][2] > 0) {
      float tdist = sqrt(pow((Kl[i][1] - S2), 2) + pow((Kl[i][0] - S1), 2));
      if (tdist < distK) {
        distK = tdist;
        cK = i;
      }
    }
  }
  if (cK != 5) return cK;

  return -1;
}

void endGame(int i) {
  SRXEFill(0);
  switch (i) {
    case 1:  // No energy
      SRXEWriteString(0, 0, "You are out of energy", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 16, "and stranded your ship!!", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 32, "Rescue beacon engaged.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 64, "Good Luck!", FONT_MEDIUM, 3, 0);
      break;
    case 2:  // Destroyrd in battle
      SRXEWriteString(0, 0, "You have been destroyed!", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 16, "With luck, more skilled", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 32, "captains can save the", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 48, "Federation from the Klingons.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 64, "We will notify your", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 96, "loved ones of your demise. ", FONT_MEDIUM, 3, 0);
      break;
    case 3:  // Run out of time
      SRXEWriteString(0, 0, "You have run out of time!!", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 16, "The Klingons have taken", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 32, "the Federation capital and", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 48, "declared Marshall law.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 64, "Life, as we know it", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 80, "is over.", FONT_MEDIUM, 3, 0);
      break;
    case 4:  // You destroyed all Klingons
      SRXEWriteString(0, 0, "You destroyed all the", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 16, "invading Klingons. The", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 32, "Federation is safe.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 48, "Proceed to the planet of", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 64, "your choice and enjoy", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 80, "some much deserved rest.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 96, "A greatful galaxy salutes you.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 112, "Well Done!!", FONT_MEDIUM, 3, 0);
      break;
    case 5:  // You Quit
      SRXEWriteString(30, 32, "Are you sure you want", FONT_MEDIUM, 3, 0);
      SRXEWriteString(30, 50, "to resign?", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 122, "< YES", FONT_LARGE, 3, 0);
      SRXEWriteString(318, 122, "NO >", FONT_LARGE, 3, 0);
      if (waitForMyKey() != 24) {
        SRXEFill(0);
        return;
      }
      SRXEFill(0);
      SRXEWriteString(0, 0, "Your resignation comes", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 16, "as a shock!! Hopefully we", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 32, "can find capable captains", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 48, "to defend us.", FONT_MEDIUM, 3, 0);
      SRXEWriteString(0, 80, "Good Luck!", FONT_MEDIUM, 3, 0);
      delay(3000);
      SRXEWriteString(0, 96, "Quitter.", FONT_MEDIUM, 3, 0);
      break;
  }
  waitForMyKey();
  resetFunc();
}

float calcDist(int tX, int tY) {
  return sqrt(pow((tY - S2), 2) + pow((tX - S1), 2)) / 8.0;
}

void modeMove() {
  // Course should be 0 to 359
  // Warp should be between 0 and 8 inclusive

  float navCourse = getFloat("Enter Course(0-360)");
  if (navCourse > 360.0) {
    dispText("Sulu", "I didn't catch that", "course, Captain.", "");
    return;
  }

  SRXEFill(0);
  float navWarp = getFloat("Enter warp speed (0-8)");
  if (navWarp > 8.0) {
    dispText("Sulu", "I didn't catch that", "speed, Captain.", "");
    return;
  }

  if (D[0] < 0 && navWarp > 0.5) {
    dispText("Scottie", "Warp engines are damaged.", "Limited to 0.5 warp", "");
    navWarp = 0.5;
  }

  // Check to see the path through the sector is clear
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      float aCourse = atan2(j - S1, S2 - i);
      aCourse = aCourse * 180 / PI;
      if (aCourse < 0) aCourse += 360;

      if (navCourse == aCourse && (S[j][i] > 1)) {
        if (navWarp >= calcDist(j, i)) {  // Check if warp distance >= dist to spot
          dispText("Sulu", "", "That course is obstructed.", "");
          return;
        }
      }
    }
  }

  int tQ1 = Q1;
  int tQ2 = Q2;
  int tS1 = S1;
  int tS2 = S2;

  float powW = navWarp * 8;
  navCourse = navCourse * PI / 180;

  if (E - powW >= 0) {
    // Move your ship here
    int dX = (navWarp + 0.12) * sin(navCourse) * 8.0;
    int dY = (navWarp + 0.12) * cos(navCourse) * 8.0;
    // apply offsets to get you to the new location
    Q1 = Q1 + int(dX / 8);
    Q2 = Q2 - int(dY / 8);
    S1 = S1 + int(dX % 8);
    S2 = S2 - int(dY % 8);

    // Correct for sector values out of limits
    if (S1 > 7) {
      Q1 = Q1 + 1;
      S1 = S1 - 8;
    }
    if (S2 > 7) {
      Q2 = Q2 + 1;
      S2 = S2 - 8;
    }
    if (S1 < 0) {
      Q1 = Q1 - 1;
      S1 = S1 + 8;
    }
    if (S2 < 0) {
      Q2 = Q2 - 1;
      S2 = S2 + 8;
    }
    // Correct for Quadrant out of limits
    boolean flag = false;
    if (Q1 > 7) {
      Q1 = 7;
      S1 = 7;
      flag = true;
    }
    if (Q2 > 7) {
      Q2 = 7;
      S2 = 7;
      flag = true;
    }
    if (Q1 < 0) {
      Q1 = 0;
      S1 = 0;
      flag = true;
    }
    if (Q2 < 0) {
      Q2 = 0;
      S2 = 0;
      flag = true;
    }
    if (flag) {
      dispText("Uhura", "Starfleet has not given permission", "to cross the Galactic Boundary", "Engines shut down.");

      if (random(0, 4) == 2) {
        dispText("Spock", "", "High radiation has caused", "damage, captain!");
        int dNum = random(0, 8);
        D[dNum] -= random(4, 8);
      }
    }

    if (S[S1][S2] > 1 && Q1 == tQ1 && Q2 == tQ2) {
      dispText("Sulu", "That space is occupied", "Captain!", "");
      S1 = tS1;
      S2 = tS2;
      return;
    }
    S[tS1][tS2] = 0;  // Warp the ship out of the old position

    E = E - powW;
    S[S1][S2] = 1;  // Put ship in current position

    updateLRS();

    if (Q1 != tQ1 || Q2 != tQ2) updateSector();

    T = T + navWarp;
    updateDamage(navWarp);
  } else {
    dispText("Scottie", "", "Insufficient Energy Available.", "");
    return;
  }
}

void modeShields() {
  SRXEFill(0);
  int valShld = (int)getFloat("New Shield Level");

  if (valShld > E + Sh) {
    dispText("Scottie", "", "Insufficient energy for shields.", "");
    return;
  }
  E = E + Sh - valShld;
  Sh = valShld;
}

void updateLRS() {
  //Top row
  if (Q1 > 0 && Q2 > 0) Z[Q1 - 1][Q2 - 1] = true;

  if (Q2 > 0) Z[Q1][Q2 - 1] = true;

  if (Q1 < 7 && Q2 > 0) Z[Q1 + 1][Q2 - 1] = true;

  //Middle row
  if (Q1 > 0) Z[Q1 - 1][Q2] = true;

  Z[Q1][Q2] = true;

  if (Q1 < 7) Z[Q1 + 1][Q2] = true;

  //Bottom row
  if (Q1 > 0 && Q2 < 7) Z[Q1 - 1][Q2 + 1] = true;

  if (Q2 < 7) Z[Q1][Q2 + 1] = true;

  if (Q1 < 7 && Q2 < 7) Z[Q1 + 1][Q2 + 1] = true;
}

void printLRS() {
  if (D[2] < 0) {
    dispText("Spock", "Long Range Scanner", "is damaged!", "");
    return;
  }
  SRXEFill(0);
  SRXEHorizontalLine(33, 55, 60, 3, 2);
  SRXEHorizontalLine(33, 85, 60, 3, 2);
  SRXEVerticalLine(52, 29, 84, 3);
  SRXEVerticalLine(73, 29, 84, 3);
  char buf[5];

  //Top row
  if (Q1 < 1 || Q2 < 1) SRXEWriteString(105, 31, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1 - 1][Q2 - 1]);
    SRXEWriteString(105, 31, buf, FONT_LARGE, 3, 0);
  }
  if (Q2 < 1) SRXEWriteString(168, 31, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1][Q2 - 1]);
    SRXEWriteString(168, 31, buf, FONT_LARGE, 3, 0);
  }
  if (Q1 > 6 || Q2 < 1) SRXEWriteString(231, 31, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1 + 1][Q2 - 1]);
    SRXEWriteString(231, 31, buf, FONT_LARGE, 3, 0);
  }

  //Middle row
  if (Q1 < 1) SRXEWriteString(105, 63, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1 - 1][Q2]);
    SRXEWriteString(105, 63, buf, FONT_LARGE, 3, 0);
  }

  snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1][Q2]);
  SRXEWriteString(168, 63, buf, FONT_LARGE, 3, 0);

  if (Q1 > 6) SRXEWriteString(231, 63, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1 + 1][Q2]);
    SRXEWriteString(231, 63, buf, FONT_LARGE, 3, 0);
  }

  //Bottom row
  if (Q1 < 1 || Q2 > 6) SRXEWriteString(105, 95, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1 - 1][Q2 + 1]);
    SRXEWriteString(105, 95, buf, FONT_LARGE, 3, 0);
  }
  if (Q2 > 6) SRXEWriteString(168, 95, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1][Q2 + 1]);
    SRXEWriteString(168, 95, buf, FONT_LARGE, 3, 0);
  }
  if (Q1 > 6 || Q2 > 6) SRXEWriteString(231, 95, "***", FONT_LARGE, 3, 0);
  else {
    snprintf(buf, sizeof(buf) - 1, "%.3i", G[Q1 + 1][Q2 + 1]);
    SRXEWriteString(231, 95, buf, FONT_LARGE, 3, 0);
  }
}

void updateSector() {
  K3 = int(G[Q1][Q2] * 0.01);
  B3 = int(G[Q1][Q2] * 0.1) - K3 * 10;
  S3 = G[Q1][Q2] - 100 * K3 - 10 * B3;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      S[i][j] = 0;
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Kl[i][j] = 0;
    }
  }

  S[S1][S2] = 1;  // This is the enterprise

  boolean empty;

  for (int i = 0; i < K3; i++) {
    empty = false;
    while (!empty) {
      byte tX = random(0, 8);
      byte tY = random(0, 8);
      if (S[tX][tY] == 0) {
        S[tX][tY] = 2;  // Klingon
        Kl[i][0] = tX;
        Kl[i][1] = tY;
        Kl[i][2] = 50 * random(1, 5) + 50;  // Save position and energy
        empty = true;
      }
    }
  }

  for (int i = 0; i < S3; i++) {
    empty = false;
    while (!empty) {
      byte tX = random(0, 8);
      byte tY = random(0, 8);
      if (S[tX][tY] == 0) {
        S[tX][tY] = 3;  // Stars
        empty = true;
      }
    }
  }

  empty = false;
  if (B3) {
    while (!empty) {
      byte tX = random(0, 8);
      byte tY = random(0, 8);
      if (S[tX][tY] == 0) {
        S[tX][tY] = 4;  // Base
        empty = true;
      }
    }
  }
}

void printStat() {
  SRXEFill(0);
  char buf[35];
  snprintf(buf, sizeof(buf) - 1, "Stardate:   %.1f / %.1f left", T, T9 - T);
  SRXEWriteString(0, 0, buf, FONT_MEDIUM, 3, 0);
  if (K3 > 0) snprintf(buf, sizeof(buf) - 1, "Condition   %s", "RED");
  else snprintf(buf, sizeof(buf) - 1, "Condition   %s", "GREEN");
  SRXEWriteString(0, 16, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Quadrant    %i-%i  %s %s", Q1 + 1, Q2 + 1, getQuad(Q1, Q2), getQuad2(Q2));
  SRXEWriteString(0, 32, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Sector      %i-%i", S1 + 1, S2 + 1);
  SRXEWriteString(0, 48, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Torpedoes   %i", P);
  SRXEWriteString(0, 64, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Energy      %i", E);
  SRXEWriteString(0, 80, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Shields     %i", Sh);
  SRXEWriteString(0, 96, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Klingons left %i", K);
  SRXEWriteString(0, 112, buf, FONT_MEDIUM, 3, 0);
  SRXEWriteString(318, 122, "DONE", FONT_LARGE, 3, 0);

  waitForMyKey();

  SRXEFill(0);
}

void printGalaxy() {
  // Print the Galaxy
  SRXEFill(0);

  char buf[3];

  SRXEWriteString(108, 0, "1   2   3   4   5   6   7   8", FONT_SMALL, 3, 0);

  for (int i = 0; i < 8; i++) {
    snprintf(buf, sizeof(buf) - 1, "%d", i + 1);
    SRXEWriteString(81, 14 + i * 16, buf, FONT_SMALL, 3, 0);

    for (int j = 0; j < 8; j++) {

      if (Z[j][i]) {
        int tK3 = int(G[j][i] * 0.01);
        int tB3 = int(G[j][i] * 0.1) - tK3 * 10;
        int BG = 0;
        int FG = 3;
        if (tB3) {
          BG = 2;
          FG = 0;
        }
        if (j == Q1 && i == Q2) {
          SRXEWriteString(105 + j * 24, 10 + i * 16, "X", FONT_MEDIUM, FG, BG);
        } else {
          snprintf(buf, sizeof(buf) - 1, "%d", tK3);
          SRXEWriteString(105 + j * 24, 10 + i * 16, buf, FONT_MEDIUM, FG, BG);
        }
      } else {
        SRXEWriteString(105 + j * 24, 10 + i * 16, "*", FONT_MEDIUM, 3, 0);
      }
    }
  }
}

char* getQuad2(int tmp2) {
  switch (tmp2) {
    case 1:
    case 5:
      return "I";
      break;
    case 2:
    case 6:
      return "II";
      break;
    case 3:
    case 7:
      return "III";
      break;
    case 4:
    case 8:
      return "IV";
      break;
  }
}

char* getQuad(int tmp1, int tmp2) {

  if (tmp2 < 4) {
    switch (tmp1) {
      case 0:
        return "Antares";
        break;
      case 1:
        return "Rigel";
        break;
      case 2:
        return "Procyon";
        break;
      case 3:
        return "Vega";
        break;
      case 4:
        return "Canopus";
        break;
      case 5:
        return "Altair";
        break;
      case 6:
        return "Sagittarius";
        break;
      case 7:
        return "Pollux";
        break;
    }
  } else {
    switch (tmp1) {
      case 0:
        return "Sirius";
        break;
      case 1:
        return "Deneb";
        break;
      case 2:
        return "Capella";
        break;
      case 3:
        return "Betelgeuse";
        break;
      case 4:
        return "Aldebaran";
        break;
      case 5:
        return "Regulus";
        break;
      case 6:
        return "Arcturus";
        break;
      case 7:
        return "Spica";
        break;
    }
  }
}
