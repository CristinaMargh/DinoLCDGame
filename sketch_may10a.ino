
// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);

// // ─── PINI ─────────────────────────────────────────────
// const int BTN_PIN = A2;
// const int BUZ_PIN = A3;

// // ─── VARIABILE ────────────────────────────────────────
// int  score  = 0;
// bool inJump = false;

// // ─── SPRITE-URI ───────────────────────────────────────
// byte dino_l[8] = {B00000111,B00000101,B00000111,B00010110,B00011111,B00011110,B00001110,B00000100};
// byte dino_r[8] = {B00000111,B00000101,B00000111,B00010110,B00011111,B00011110,B00001110,B00000010};
// byte cactus_s[8]= {0b00100,0b00101,0b10101,0b10101,0b10111,0b11100,0b00100,0b00000};
// byte cactus_b[8]= {B00000000,B00000100,B00000101,B00010101,B00010110,B00001100,B00000100,B00000100};

// // ─── HARTA ────────────────────────────────────────────
// char world[32];              // două rânduri a câte 16 celule (0-15 sus, 16-31 jos)

// // ──────────────────────────────────────────────────────
// bool buttonPressed()
// {
//   if (digitalRead(BTN_PIN) == LOW) { delay(50); return digitalRead(BTN_PIN) == LOW; }
//   return false;
// }

// void buildInitialWorld()                     // pentru ecranul Press Start!
// {
//   memset(world, 32, 32);
//   const char txt[] = "Score:";
//   for (byte i = 0; i < 6; ++i) world[7 + i] = txt[i];
//   world[17] = 0;                              // dino jos
// }

// void displayPressStart()
// {
//   lcd.clear();
//   lcd.setCursor(0, 1);
//   lcd.print("Press  Start!");
// }

// void drawWorld()
// {
//   lcd.setCursor(0, 0);
//   for (int i = 0; i < 32; ++i) {
//     if (i == 17) { world[17] ^= 1; lcd.write(byte(world[17])); }
//     else {
//       if (i == 16) lcd.setCursor(0, 1);
//       lcd.write(byte(world[i]));
//     }
//   }
//   lcd.setCursor(13, 0);
//   lcd.print(score < 100 ? (score < 10 ? "  " : " ") : "");
//   lcd.print(score);
// }

// uint8_t scrollWorld()
// {
//   delay(100);
//   world[31] = (random(35) < 4) ? random(2, 4) : 32;   // 0-1 dino, 2-3 cactus

//   for (int i = 16; i < 32; ++i) {
//     if (world[i] == 2 || world[i] == 3) {
//       char prev = (i < 31) ? world[i + 1] : 32;
//       if (world[i - 1] < 2) return 1;                 // coliziune
//       world[i - 1] = world[i];
//       world[i]     = prev;
//     }
//   }
//   world[15] = 32;
//   if (world[16] < 2) world[16] = 32;
//   return 0;
// }

// void updateWorld()
// {
//   if (scrollWorld()) {
//     tone(BUZ_PIN, 600, 300);
//     lcd.clear();
//     lcd.setCursor(0, 0); lcd.print("  GAME  OVER!  ");
//     lcd.setCursor(0, 1); lcd.print("Score: "); lcd.print(score);
//     while (!buttonPressed());                    // așteaptă restart
//     while (buttonPressed());
//     buildInitialWorld(); score = 0; inJump = false;
//     displayPressStart();                         // re-afișează meniul
//     return;
//   }
//   ++score;
//   drawWorld();
// }

// void jump()
// {
//   inJump      = true;
//   tone(BUZ_PIN, 1000, 80);
//   world[1]  = 0;          // dino sus
//   world[17] = 32;         // șterge jos
//   for (byte i = 0; i < 4; ++i) updateWorld();
//   world[1]  = 32;
//   world[17] = 0;
//   inJump      = false;
// }

// // ─── SETUP ────────────────────────────────────────────
// void setup()
// {
//   pinMode(BTN_PIN, INPUT_PULLUP);
//   pinMode(BUZ_PIN, OUTPUT);

//   lcd.init();
//   lcd.backlight();
//   lcd.createChar(0, dino_l);
//   lcd.createChar(1, dino_r);
//   lcd.createChar(2, cactus_s);
//   lcd.createChar(3, cactus_b);

//   buildInitialWorld();
//   displayPressStart();
// }

// // ─── LOOP ─────────────────────────────────────────────
// void loop()
// {
//   if (!buttonPressed()) return;          // încă nu s-a apăsat START
//   while (buttonPressed());               // așteaptă eliberarea

//   lcd.clear(); score = 0;                // start joc
//   world[1]  = 32; world[17] = 0;         // dino jos sigur
//   drawWorld();

//   while (true) {
//     if (!inJump && buttonPressed()) { while (buttonPressed()); jump(); }
//     updateWorld();
//   }
// }
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ─── PINI ─────────────────────────────────────────────
const int BTN_PIN = A2;
const int BUZ_PIN = A3;
const int BTN_DOWN_PIN = A1;  // Nou: buton pentru aplecare

// ─── VARIABILE ────────────────────────────────────────
int  score  = 0;
bool inJump = false;
bool inDuck = false;  // Nou: pentru aplecare

// ─── SPRITE-URI ───────────────────────────────────────
byte dino_l[8] = {B00000111,B00000101,B00000111,B00010110,B00011111,B00011110,B00001110,B00000100};
byte dino_r[8] = {B00000111,B00000101,B00000111,B00010110,B00011111,B00011110,B00001110,B00000010};
byte cactus_s[8]= {0b00100,0b00101,0b10101,0b10101,0b10111,0b11100,0b00100,0b00000};
byte cactus_b[8]= {B00000000,B00000100,B00000101,B00010101,B00010110,B00001100,B00000100,B00000100};
byte bird[8] = {B00000000,B00000000,B00001100,B00011110,B00001111,B00000011,B00000000,B00000000};  // Nou: pasăre

// ─── HARTA ────────────────────────────────────────────
char world[32];              // două rânduri a câte 16 celule (0-15 sus, 16-31 jos)

// ──────────────────────────────────────────────────────
bool buttonPressed() {
  if (digitalRead(BTN_PIN) == LOW) { delay(50); return digitalRead(BTN_PIN) == LOW; }
  return false;
}

bool downButtonPressed() {   // Nou: funcție pentru butonul de aplecare
  if (digitalRead(BTN_DOWN_PIN) == LOW) { delay(50); return digitalRead(BTN_DOWN_PIN) == LOW; }
  return false;
}

void buildInitialWorld()                     // pentru ecranul Press Start!
{
  memset(world, 32, 32);
  const char txt[] = "Score:";
  for (byte i = 0; i < 6; ++i) world[7 + i] = txt[i];
  world[17] = 0;                              // dino jos
}

void displayPressStart() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Press  Start!");
}

void drawWorld() {
  lcd.setCursor(0, 0);
  for (int i = 0; i < 32; ++i) {
    if (i == 17 && !inJump) {
      world[17] ^= 1;  // Alternează între cele două animații ale dinozaurului
      lcd.write(byte(world[17]));
    }
    else {
      if (i == 16) lcd.setCursor(0, 1);
      lcd.write(byte(world[i]));
    }
  }
  lcd.setCursor(13, 0);
  lcd.print(score < 100 ? (score < 10 ? "  " : " ") : "");
  lcd.print(score);
}

uint8_t scrollWorld() {
  delay(100);
  
  // Decide dacă adaugă un obstacol nou la finalul ecranului
  if (random(35) < 4) {
    if (random(10) < 4 && !hasObstacleInRow(0)) {  // 40% șansă pentru pasăre pe rândul de sus
      world[15] = 4;  // Codul pentru pasăre
    } else if (!hasObstacleInRow(1)) {
      world[31] = random(2, 4);  // Cactus pe rândul de jos
    }
  } else {
    world[31] = 32;  // Spațiu gol
    world[15] = 32;  // Spațiu gol
  }

  // Verifică coliziuni și mută obstacolele spre stânga
  for (int i = 0; i < 16; ++i) {  // Procesează rândul de sus
    if (world[i] == 4) {  // Pasăre
      if (i > 0 && world[i-1] == 0 && !inDuck) {
        return 1;  // Coliziune cu pasărea
      }
      char prev = (i < 15) ? world[i + 1] : 32;
      world[i - 1] = world[i];
      world[i] = prev;
    }
  }
  
  for (int i = 16; i < 32; ++i) {  // Procesează rândul de jos
    if (world[i] == 2 || world[i] == 3) {  // Cactus
      if (i > 16 && world[i-1] < 2 && !inJump) {
        return 1;  // Coliziune cu cactusul
      }
      char prev = (i < 31) ? world[i + 1] : 32;
      world[i - 1] = world[i];
      world[i] = prev;
    }
  }
  
  // Curățare de la marginile ecranului
  world[0] = 32;   // Curăță marginea stângă sus
  world[16] = 32;  // Curăță marginea stângă jos
  
  return 0;  // Nicio coliziune
}

// Nou: verifică dacă există deja un obstacol în rândul specificat
bool hasObstacleInRow(int row) {
  int start = row * 16;
  int end = start + 16;
  
  for (int i = start; i < end; ++i) {
    if ((row == 0 && world[i] == 4) || (row == 1 && (world[i] == 2 || world[i] == 3))) {
      return true;
    }
  }
  return false;
}

void updateWorld() {
  if (scrollWorld()) {
    tone(BUZ_PIN, 600, 300);
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("  GAME  OVER!  ");
    lcd.setCursor(0, 1); lcd.print("Score: "); lcd.print(score);
    while (!buttonPressed());                    // așteaptă restart
    while (buttonPressed());
    buildInitialWorld(); score = 0; inJump = false; inDuck = false;
    displayPressStart();                         // re-afișează meniul
    return;
  }
  ++score;
  drawWorld();
}

void jump() {
  if (inDuck) return;  // Nu poți sări în timp ce ești aplecat
  
  inJump = true;
  tone(BUZ_PIN, 1000, 80);
  world[1] = 0;          // dino sus
  world[17] = 32;        // șterge jos
  for (byte i = 0; i < 4; ++i) updateWorld();
  world[1] = 32;
  world[17] = 0;
  inJump = false;
}

// Nou: funcție pentru aplecare
void duck() {
  if (inJump) return;  // Nu te poți apleca în timp ce sari
  
  inDuck = true;
  world[1] = 0;  // Animație dinozaur aplecat (folosim același sprite)
  
  while(downButtonPressed()) {
    updateWorld();
  }
  
  inDuck = false;
  world[1] = 32;
}

// ─── SETUP ────────────────────────────────────────────
void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);  // Nou: setare pin pentru butonul de aplecare
  pinMode(BUZ_PIN, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, dino_l);
  lcd.createChar(1, dino_r);
  lcd.createChar(2, cactus_s);
  lcd.createChar(3, cactus_b);
  lcd.createChar(4, bird);  // Nou: creează caracterul pentru pasăre
  
  buildInitialWorld();
  displayPressStart();
}

// ─── LOOP ─────────────────────────────────────────────
void loop() {
  if (!buttonPressed()) return;          // încă nu s-a apăsat START
  while (buttonPressed());               // așteaptă eliberarea
  
  lcd.clear(); score = 0;                // start joc
  world[1] = 32; world[17] = 0;         // dino jos sigur
  drawWorld();
  
  while (true) {
    if (!inJump && !inDuck && buttonPressed()) { 
      while (buttonPressed()); 
      jump(); 
    }
    
    if (!inJump && downButtonPressed()) {
      duck();
    }
    
    updateWorld();
  }
}
