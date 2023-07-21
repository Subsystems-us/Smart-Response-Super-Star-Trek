void dispMainMenu(void) {

  SRXERectangle(0, 0, 22, 135, 3, 3);    // Clear left
  SRXERectangle(105, 0, 22, 135, 3, 3);  // Clear right

  // Draw arrows pointing to the soft keys
  SRXEWriteString(0, 2, "<PWR", FONT_LARGE, 0, 3);
  SRXEWriteString(0, 32, "<OPT", FONT_LARGE, 0, 3);
  SRXEWriteString(0, 62, "<COM", FONT_LARGE, 0, 3);
  SRXEWriteString(0, 92, "<DAM", FONT_LARGE, 0, 3);
  SRXEWriteString(0, 122, "<SHE", FONT_LARGE, 0, 3);

  SRXEWriteString(318, 2, "NAV>", FONT_LARGE, 0, 3);
  SRXEWriteString(318, 32, "SRS>", FONT_LARGE, 0, 3);
  SRXEWriteString(318, 62, "LRS>", FONT_LARGE, 0, 3);
  SRXEWriteString(318, 92, "PHA>", FONT_LARGE, 0, 3);
  SRXEWriteString(318, 122, "TOR>", FONT_LARGE, 0, 3);
}

void showOptions() {
  SRXEFill(0);
  SRXEWriteString(0, 2, "< Quit", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 62, "< Set Levels", FONT_LARGE, 3, 0);
  SRXEWriteString(285, 122, "EXIT >", FONT_LARGE, 3, 0);
  byte cho = waitForMyKey();

  if (cho == 20) endGame(5);

  if (cho == 22) {
    while (changeOptions()) {}
  }
  SRXEFill(0);
}

boolean changeOptions() {
  SRXEFill(0);
  SRXEWriteString(0, 2, "< Damage from Attack (2)", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 32, "< Damage Boundary Hit (2)", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 62, "< Klingon Strength (1.0)", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 120, "< Adjust Energy", FONT_LARGE, 3, 0);
  SRXEWriteString(285, 122, "EXIT >", FONT_LARGE, 3, 0);

  byte selOp = waitForMyKey();
  if (selOp == 29) return false;  // Exit

  SRXEFill(0);
  float newVal = getFloat("Enter New Value");
  switch (selOp) {
    case 20:
      kAttackFactor = (int)newVal;
      break;
    case 21:
      boundaryFactor = (int)newVal;
      break;
    case 22:
      klingonStrength = newVal;
      break;
    case 24:
      E = (int)newVal;
      break;
  }
  return true;
}

void introText1() {
  SRXEFill(0);
  SRXEWriteString(27, 0, "THE USS ENTERPRISE", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 20, "YOUR ORDERS ARE AS FOLLOWS:", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 36, "--------------------------", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 52, "You must destroy the Klingons", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 68, "that have infiltrated the", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 84, "galaxy before they destroy", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 100, "the Federation", FONT_MEDIUM, 3, 0);
  SRXEWriteString(318, 122, "DONE", FONT_LARGE, 3, 0);
  waitForMyKey();
  randomSeed(millis());  // Randomize
}

void introText2() {
  SRXEFill(0);
  char buf[55];
  snprintf(buf, sizeof(buf) - 1, "Destroy the %i Klingon Warships", K);
  SRXEWriteString(0, 10, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Before SD %i.", (int)T9);
  SRXEWriteString(0, 26, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "You have %i SDs to complete", (int)(T9 - T));
  SRXEWriteString(0, 42, buf, FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 58, "your mission.", FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "There are %i starbases.", B);
  SRXEWriteString(0, 74, buf, FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 90, "Energy: 3000 units", FONT_MEDIUM, 3, 0);
  SRXEWriteString(0, 106, "Torpedoes: 10", FONT_MEDIUM, 3, 0);
  SRXEWriteString(318, 122, "DONE", FONT_MEDIUM, 3, 0);
  waitForMyKey();
  SRXEFill(0);
}

void clearArea() {
  SRXERectangle(23, 0, 81, 135, 0, 1);  // (int x, int y, int cx, int cy, byte color, byte bFilled)
}

void dispDamage() {
  if (D[5] < 0) {
    dispText("Spock", "Damage Control Computer", "is damaged!", "");
    return;
  }
  clearArea();
  char buf[20];
  snprintf(buf, sizeof(buf) - 1, "Warp Engines  %.2f", D[0]);
  SRXEWriteString(81, 4, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "SR Scanner    %.2f", D[1]);
  SRXEWriteString(81, 20, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "LR Scanner    %.2f", D[2]);
  SRXEWriteString(81, 36, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Phasers       %.2f", D[3]);
  SRXEWriteString(81, 52, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Torpedos      %.2f", D[4]);
  SRXEWriteString(81, 68, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Damage Cntl   %.2f", D[5]);
  SRXEWriteString(81, 84, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Shields       %.2f", D[6]);
  SRXEWriteString(81, 100, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "Computer      %.2f", D[7]);
  SRXEWriteString(81, 116, buf, FONT_MEDIUM, 3, 0);
}

void dispComMenu() {
  SRXEFill(0);
  SRXEWriteString(69, 2, "     Galactic Chart >", FONT_LARGE, 3, 0);
  SRXEWriteString(69, 32, "      Status Report >", FONT_LARGE, 3, 0);
  SRXEWriteString(69, 62, "  Starbase Nav Data >", FONT_LARGE, 3, 0);
  SRXEWriteString(69, 92, "Targetting Computer >", FONT_LARGE, 3, 0);
  SRXEWriteString(69, 122, "               Exit >", FONT_LARGE, 3, 0);
}

void torpAnim() {
  SRXEFill(0);
  SRXEWriteString(69, 30, "TORPEDO AWAY!!", FONT_LARGE, 3, 0);
  SRXEWriteString(24, 81, "E", FONT_LARGE, 3, 0);
  SRXEWriteString(324, 81, "K", FONT_LARGE, 3, 0);
  for (int i = 39; i < 324; i += 16) {
    SRXEWriteString(i, 81, "-", FONT_LARGE, 3, 0);
    delay(100);
  }
  SRXEFill(0);
}

void dispText(char* name, char* says1, char* says2, char* says3) {
  SRXEFill(0);
  char buf[30];
  snprintf(buf, sizeof(buf) - 1, "%s:", name);
  SRXEWriteString(12, 16, buf, FONT_LARGE, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "%s", says1);
  SRXEWriteString(18, 40, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "%s", says2);
  SRXEWriteString(18, 58, buf, FONT_MEDIUM, 3, 0);
  snprintf(buf, sizeof(buf) - 1, "%s", says3);
  SRXEWriteString(18, 76, buf, FONT_MEDIUM, 3, 0);
  SRXEWriteString(318, 122, "DONE", FONT_LARGE, 3, 0);
  byte inS = waitForMyKey();
  SRXEFill(0);
}

void printSRS() {
  // Print the SRS
  if (D[1] < 0) {
    dispText("Spock", "Short-Range Scanner", "is damaged!", "");
    return;
  }

  if (K3 > 0) {
    dispText("Spock", "Combat area ....", "CONDITION RED", "");
    if (Sh < 100) dispText("Sulu", "Shields are", "dangerously low!", "");
  }

  clearArea();
  char buf[3];
  SRXEWriteString(111, 0, "1   2   3   4   5   6   7   8", FONT_SMALL, 3, 0);

  for (int i = 0; i < 8; i++) {
    snprintf(buf, sizeof(buf) - 1, "%d", i + 1);
    SRXEWriteString(81, 14 + i * 16, buf, FONT_SMALL, 3, 0);

    for (int j = 0; j < 8; j++) {
      switch (S[j][i]) {
        case 0:
          SRXEWriteString(105 + j * 24, 10 + i * 16, ".", FONT_MEDIUM, 3, 0);
          break;
        case 1:
          SRXEWriteString(105 + j * 24, 10 + i * 16, "E", FONT_MEDIUM, 3, 0);
          break;
        case 2:
          SRXEWriteString(105 + j * 24, 10 + i * 16, "K", FONT_MEDIUM, 3, 0);
          break;
        case 3:
          SRXEWriteString(105 + j * 24, 10 + i * 16, "*", FONT_MEDIUM, 3, 0);
          break;
        case 4:
          SRXEWriteString(105 + j * 24, 10 + i * 16, "B", FONT_MEDIUM, 3, 0);
          break;
      }
    }
  }
}

byte getCurKey() {
  // Returns the value of the key pressed or 30 if there are none
  byte tKey = 30;
  char key1 = SRXEGetKey();

  // Check Special Keys
  if ((byte)key1 == 0xf0) tKey = 20;  // PWR
  if ((byte)key1 == 0xf1) tKey = 21;  // XXX
  if ((byte)key1 == 0xf2) tKey = 22;  // SRS
  if ((byte)key1 == 0xf3) tKey = 23;  // COM
  if ((byte)key1 == 0xf4) tKey = 24;  // SHE
  if ((byte)key1 == 0xf5) tKey = 25;  // NAV
  if ((byte)key1 == 0xf6) tKey = 26;  // SRS
  if ((byte)key1 == 0xf7) tKey = 27;  // LRS
  if ((byte)key1 == 0xf8) tKey = 28;  // PHA
  if ((byte)key1 == 0xf9) tKey = 29;  // TOR
  if (key1 == '.') tKey = 10;         // Use del key for INC also
  if (key1 == '0') tKey = 0;
  if (key1 == '1') tKey = 1;
  if (key1 == '2') tKey = 2;
  if (key1 == '3') tKey = 3;
  if (key1 == '4') tKey = 4;
  if (key1 == '5') tKey = 5;
  if (key1 == '6') tKey = 6;
  if (key1 == '7') tKey = 7;
  if (key1 == '8') tKey = 8;
  if (key1 == '9') tKey = 9;

  return tKey;  // use 30 for no key pressed
}

byte waitForMyKey() {
  byte inKeyt = 30;
  while (inKeyt == 30) {
    inKeyt = getCurKey();
  }
  return inKeyt;
}

float getFloat(char* prompt) {
  SRXEWriteString(3, 50, prompt, FONT_LARGE, 3, 0);
  SRXEWriteString(3, 70, ">", FONT_LARGE, 3, 0);
  SRXEWriteString(0, 122, "< BACK SP", FONT_LARGE, 3, 0);
  SRXEWriteString(279, 122, "ENTER >", FONT_LARGE, 3, 0);

  byte keyIn = 30;
  int floatIndex = 0;
  char finalFloat[10] = "         ";

  while ((byte)keyIn != 0xf9) {
    keyIn = SRXEGetKey();
    if (keyIn >= '0' && keyIn <= '9' || keyIn == '.') {
      finalFloat[floatIndex++] = keyIn;
    }
    if ((byte)keyIn == 0xf4 && floatIndex > 0) {
      floatIndex--;
      finalFloat[floatIndex] = ' ';
    }
    SRXEWriteString(24, 70, finalFloat, FONT_LARGE, 3, 0);
  }
  SRXEFill(0);
  return atof(finalFloat);
}