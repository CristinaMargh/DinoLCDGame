
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// PINS
const int JOY_JUMP_PIN  = A1; 
const int JOY_START_PIN = A0;   // start/reset
const int BUZ_PIN       = A3;   // buzzer

// JOYSTICK 
int jumpCenter  = 512;
int startCenter = 512;
const int DELTA_UP = -200;

bool joyUp(int pin, int ctr) {
  return (analogRead(pin) - ctr) <  DELTA_UP; 
  }
bool joyUpRel(int pin,int ctr) { 
  return (analogRead(pin) - ctr) >  DELTA_UP / 2; 
  }

// EEPROM HIGH-SCORE
const int HS = 0;
int  readHS ()  { 
  int v; 
  EEPROM.get(HS, v); 
  return (v < 0 || v > 99999) ? 0 : v; 
  }

void writeHS(int v) {
  EEPROM.put(HS,v); 
  }

byte dino_l[8] = {B111,B101,B111,B0110,B11111,B11110,B01110,B00100};
byte dino_r[8] = {B111,B101,B111,B0110,B11111,B11110,B01110,B00010};
byte cactus_s[8] = {B00100,B00101,B10101,B10101,B10111,B11100,B00100,B00000};
byte cactus_b[8] = {B00000,B00100,B00101,B10101,B10110,B01100,B00100,B00100};
byte bird[8] = {B00000,B00000,B01100,B11110,B01111,B00011,B00000};

char world[32];
bool inJump = false, canJump = true;
int  score = 0;

void buildWorld() {
  memset(world, 32, 32);
  const char t[]="Score:";
  for (byte i = 0; i < 6; i++) 
    world[7 + i] = t[i];
  world[17] = 0; 
}

void drawWorld() {
  lcd.setCursor(0,0);
  for (int i = 0; i < 32; i++) {
    if(i == 17 && !inJump) {
      world[17] ^= 1;
      lcd.write(byte(world[17])); 
      } else {
      if (i == 16)
        lcd.setCursor(0, 1);
      lcd.write(byte(world[i]));
    }
  }
  lcd.setCursor(13,0); 
  lcd.print(score);
}

void showHS() {
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("High Score:");
  lcd.setCursor(0,1); 
  lcd.print(readHS());
  delay(2000);
}

bool hasObstacleRow(int r) {
  int s = r * 16;
  for (int i = s; i < s + 16; i++)
    if((r == 0 && world[i]==4) || (r == 1 && (world[i] == 2 || world[i] == 3)))
      return true;
  return false;
}
bool advanceWorld() {                
  delay(100);
  if (random(35) < 4) {
    if (random(10) < 4 && !hasObstacleRow(0)) 
      world[15] = 4;
    else if (!hasObstacleRow(1))
      world[31] = random(2, 4);
  }else world[15] = world[31] = 32;

  for (int i = 0; i < 16; i++)
    if (world[i] == 4) {
      if (i > 0 && world[i - 1] == 0)
        return true;
      world[i - 1] = 4; 
      world[i] = (i < 15)?world[i + 1] : 32;
    }
  for (int i = 16; i < 32; i++)
    if(world[i] == 2 || world[i] == 3) {
      if (i > 16 && world[i - 1] < 2 && !inJump)
        return true;
      world[i - 1] = world[i];
      world[i] = (i < 31)?world[i + 1] : 32;
    }
  world[0] = world[16] = 32;
  return false;
}

bool updateWorld(){          
  if(advanceWorld()) 
    return true;
  ++score;
  drawWorld();
  return false;
}

void jump() {
  inJump = true;
  tone(BUZ_PIN,1000,80);
  world[1] = 0;
  world[17] = 32;
  for (byte i = 0; i < 4; i++) 
    if(updateWorld()) 
      break;
  world[1] = 32;
  world[17] = 0;
  inJump = false;
}

void setup() {
  pinMode(JOY_JUMP_PIN,INPUT);
  pinMode(JOY_START_PIN,INPUT);
  pinMode(BUZ_PIN,OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.createChar(0,dino_l);
  lcd.createChar(1,dino_r);
  lcd.createChar(2,cactus_s);
  lcd.createChar(3,cactus_b);
  lcd.createChar(4,bird);

  delay(100);
  jumpCenter  = analogRead(JOY_JUMP_PIN);
  startCenter = analogRead(JOY_START_PIN);

  buildWorld();
}


enum State {IDLE, PLAYING, GAMEOVER};
State state = IDLE;

void loop() {

    while(state == IDLE) {
    lcd.setCursor(0,1);
    lcd.print("Start");

    if (joyUp(JOY_JUMP_PIN,jumpCenter)){          
      showHS();
      while (joyUp(JOY_JUMP_PIN, jumpCenter));
      lcd.clear();
    }
    else if(joyUp(JOY_START_PIN, startCenter)){
      while (joyUp(JOY_START_PIN, startCenter));
      lcd.clear();
      score = 0; 
      canJump = true;
      inJump = false;
      buildWorld();
      drawWorld();
      state = PLAYING;
    }
  }

  while (state == PLAYING) {
    if (canJump && joyUp(JOY_JUMP_PIN,jumpCenter)) {
      jump();
      canJump = false; }
    if (!canJump && joyUpRel(JOY_JUMP_PIN,jumpCenter)) 
      canJump = true;

    if (updateWorld()) {
      tone(BUZ_PIN,600,300);
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("   GAME  OVER   ");
      lcd.setCursor(0,1); 
      lcd.print("Score: "); 
      lcd.print(score);
      if (score > readHS())
         writeHS(score);
      state = GAMEOVER; 
      break;
    }
  }

  // GAME OVER 
  while(state==GAMEOVER){
    lcd.setCursor(0,0);
    if(joyUp(JOY_JUMP_PIN,jumpCenter)){          
      showHS();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   GAME  OVER   ");
      lcd.setCursor(0,1);
      lcd.print("Score: ");
      lcd.print(score);
      while(joyUp(JOY_JUMP_PIN,jumpCenter));
    }
    else if(joyUp(JOY_START_PIN,startCenter)){   
      while (joyUp(JOY_START_PIN,startCenter));
      lcd.clear();
      score = 0;
      canJump = true;
      inJump = false;
      buildWorld();
      drawWorld();
      state = PLAYING;
    }
  }
}

