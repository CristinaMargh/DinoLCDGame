
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ───────────────────────────── PINI HARDWARE ─────────────────────────────
const int JOY_JUMP_PIN  = A1;  // joystick STANGA  – VRy (saritura)
const int JOY_START_PIN = A0;  // joystick DREAPTA – VRy (start / restart)
const int BUZ_PIN       = A3;  // buzzer pasiv

// praguri (offset fata de pozitia de repaus, calibrate în setup)
int  jumpCenter   = 512;  // valoare medie cand joystickul stanga e la centru
int  startCenter  = 512;  // valoare medie joystick dreapta
const int DELTA_UP = -200; // cat de mult trebuie să scada fata de centru pentru a fi "UP"

// detectie fronturi
bool canJump  = true;   // cand poate declansa jump
bool canStart = true;   // cand poate declansa start/restart

// ─── VARIABILE JOC ──────────────────────────────────
int  score  = 0;
bool inJump = false;

// ─── SPRITE-URI PERSONALIZATE ───────────────────────
byte dino_l[8]  = {B00000111,B00000101,B00000111,B00010110,B00011111,B00011110,B00001110,B00000100};
byte dino_r[8]  = {B00000111,B00000101,B00000111,B00010110,B00011111,B00011110,B00001110,B00000010};
byte cactus_s[8]= {0b00100,0b00101,0b10101,0b10101,0b10111,0b11100,0b00100,0b00000};
byte cactus_b[8]= {B00000000,B00000100,B00000101,B00010101,B00010110,B00001100,B00000100,B00000100};
byte bird[8]    = {B00000,B00000,B00001100,B00011110,B00001111,B00000011,B00000000};

// ─── HARTA (0‑15 sus, 16‑31 jos) ────────────────────
char world[32];

// ──────────────────── FUNCTII JOYSTICK ──────────────
bool joyUp(int pin, int center)      { return (analogRead(pin) - center) <  DELTA_UP; }
bool joyReleased(int pin, int center){ return (analogRead(pin) - center) > (DELTA_UP / 2); }

// ───────────────────── HELPERS DISPLAY ─────────────
void buildInitialWorld(){
  memset(world,32,32);
  const char txt[] = "Score:";
  for(byte i = 0; i < 6;++i) 
    world[7 + i] = txt[i];
  world[17] = 0; // dino pe randul de jos, col 1 (index 17)
}
void drawWorld(){
  lcd.setCursor(0,0);
  for (int i = 0; i < 32; ++i) {
    if (i == 17 && !inJump){
      world[17]^= 1; lcd.write(byte(world[17])); 
      }
    else { if(i == 16) lcd.setCursor(0,1); lcd.write(byte(world[i])); }
  }
  lcd.setCursor(13, 0);
  lcd.print(score);
}

// ───────────── OBSTACOLE & COLIZIUNI ─────────────
bool hasObstacleInRow(int r){
  int s = r*16;
  for(int i = s; i < s + 16;++i)
    if((r==0&&world[i]==4)||(r == 1 && (world[i]==2||world[i]==3))) return true;
  return false;
}
uint8_t scrollWorld() {
  delay(100);
  if(random(35) < 4) {
    if(random(10) < 4 && !hasObstacleInRow(0)) world[15] = 4;               // pasare
    else if(!hasObstacleInRow(1))            world[31] = random(2,4);     // cactus
  } else world[15]=world[31] =32;

  for(int i = 0; i < 16; ++i)
    if(world[i] == 4) {
      if(i > 0 && world[i-1] == 0) 
        return 1;
      world[i - 1] = 4; 
      world[i] = (i<15)?world[i+1]:32;
    }
  for(int i = 16; i < 32; ++i)
    if(world[i] == 2 || world[i]==3){
      if(i>16&&world[i-1] < 2 && !inJump) 
        return 1;
      world[i-1] = world[i]; 
      world[i] = (i<31)?world[i + 1]:32;
    }
  world[0] = world[16] = 32;
  return 0;
}
void updateWorld() {
  if (scrollWorld()) {
    tone(BUZ_PIN,600,300);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("  GAME  OVER!  ");
    lcd.setCursor(0,1);
    lcd.print("Score: ");
    lcd.print(score);
    // asteapta UP pe joystick dreapta
    while (!joyUp(JOY_START_PIN,startCenter));
    while (!joyReleased(JOY_START_PIN,startCenter));
    buildInitialWorld(); 
    score = 0;
    inJump = false;
    canJump = true;
    canStart = false;
    return;
  }
  ++score;
  drawWorld();
}

// ──────────── LOGICA JUMP ──────────────────────────
void jump(){
  inJump=true; tone(BUZ_PIN,1000,80);
  world[1]=0;
  world[17]=32;
  for(byte i = 0; i < 4; ++i)
    updateWorld();
  world[1] = 32;
  world[17]=0;
  inJump=false;
}

// ───────────────────── SETUP ─────────────────────────
void setup(){
  pinMode(JOY_JUMP_PIN,  INPUT);
  pinMode(JOY_START_PIN, INPUT);
  pinMode(BUZ_PIN,       OUTPUT);
  lcd.init(); lcd.backlight();
  lcd.createChar(0,dino_l); lcd.createChar(1,dino_r);
  lcd.createChar(2,cactus_s); lcd.createChar(3,cactus_b); lcd.createChar(4,bird);

  // calibrare pe 100 ms
  delay(100);
  jumpCenter  = analogRead(JOY_JUMP_PIN);
  startCenter = analogRead(JOY_START_PIN);

  buildInitialWorld();
  lcd.clear(); lcd.setCursor(0,1); lcd.print("Swipe left-Start Game");
}

// ───────────────────── LOOP ─────────────────────────
void loop(){
  // START / RESTART cu joystick DREAPTA
  if(!joyUp(JOY_START_PIN,startCenter)) return;
  while(!joyReleased(JOY_START_PIN,startCenter));

  lcd.clear(); score=0; world[1]=32; world[17]=0; drawWorld();
  canJump=true; canStart=false;

  while(true){
    // Jump cu joystick STÂNGA
    if(canJump && joyUp(JOY_JUMP_PIN,jumpCenter)) { jump(); canJump=false; }
    if(!canJump && joyReleased(JOY_JUMP_PIN,jumpCenter)) canJump=true;

    updateWorld();
  }
}
