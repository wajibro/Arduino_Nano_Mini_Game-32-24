#include <Arduino.h>

byte maping = 0;

byte mode1[8] = {0, 1, 2, 5, 8, 7, 6, 3};
byte mode2[7] = {6, 3, 0, 1, 2, 5, 8};

byte snailMode[9] = {6, 3, 0, 1, 2, 5, 8, 7, 4};
bool clearmode = 0;

byte led[9] = {A1, A2, A3, A4, A5, 7, 6, 5, 4};

enum shift{
  CLK,
  DATA,
  LATCH
};
byte sr[3] = {10, 9, 8};

byte digit1(byte x){
  return x / 10;
}
byte digit2(byte x){
  return x % 10;
}

byte num [10][8]={
	{0,0,1,1,1,1,1,1},
	{0,0,0,0,0,1,1,0},
	{0,1,0,1,1,0,1,1},
	{0,1,0,0,1,1,1,1},
	{0,1,1,0,0,1,1,0},
	{0,1,1,0,1,1,0,1},
	{0,1,1,1,1,1,0,1},
	{0,0,0,0,0,1,1,1},
	{0,1,1,1,1,1,1,1},
	{0,1,1,0,1,1,1,1}
};
 
void seg(byte x, byte y){
  for(byte i=0; i<8; i++){
      digitalWrite(sr[DATA], num[x][i] ? 1 : 0);
      digitalWrite(sr[CLK], 0);
      digitalWrite(sr[CLK], 1);
    }
    for(byte i=0; i<8; i++){
      digitalWrite(sr[DATA], num[y][i] ? 1 : 0);
      digitalWrite(sr[CLK], 0);
      digitalWrite(sr[CLK], 1);
    }
  digitalWrite(sr[LATCH], 0);
  digitalWrite(sr[LATCH], 1);
}
void segNull(){
  for(byte i=0; i<8; i++){
    digitalWrite(sr[DATA], 0);
    digitalWrite(sr[CLK], 0);
    digitalWrite(sr[CLK], 1);
  }
  for(byte i=0; i<8; i++){
    digitalWrite(sr[DATA], 0);
    digitalWrite(sr[CLK], 0);
    digitalWrite(sr[CLK], 1);
  }
  digitalWrite(sr[LATCH], 0);
  digitalWrite(sr[LATCH], 1);

}
enum buttonPos{ 
  top, 
  left, 
  middle, 
  right, 
  bottom
  };
byte button[5] = {2, 12, 11, 3, 13};

int8_t cursor = 1;
byte stateLed[9] = {0,0,0,0,0,0,0,0,0};

void setup() {
  pinMode(A0, INPUT);
  {
    byte count = 0;
    while(count <= 9){
      pinMode(led[count], OUTPUT);
      digitalWrite(led[count], 0);
      count++;
    }
  }
  {
    byte count = 0;
    byte icount = 0;
    while(count <= 3){
      pinMode(sr[count], OUTPUT);
      count++;
    }
    while(icount <= 8){
      digitalWrite(sr[DATA], 0);
      digitalWrite(sr[CLK], 0);
      digitalWrite(sr[CLK], 1);
      icount++;
    }
    digitalWrite(sr[LATCH], 0);
    digitalWrite(sr[LATCH], 1);
  }
  {
    byte count = 0;
    while (count < 5){
      pinMode(button[count], INPUT);
      count++;
    }
  }
  //Main Begin
  {
    delay(1000);

    for(byte i=0; i<9; i++){
      digitalWrite(led[i], 1);
    }
    seg(3, 2);
    delay(800);
    
    for(byte i = 0; i < 9; i++){
      digitalWrite(led[i], 0);
    }
    segNull();
    delay(500);

    seg(2, 4);
    for(byte i = 0; i < 9; i++){
      digitalWrite(led[snailMode[i]], 1);
      delay(2000/9);
    }

    for(byte i = 0; i < 9; i++){
      digitalWrite(led[i], 0);
    }
    segNull();

    delay(500);
  }
  
}

byte counter1 = 0;

byte selector = 1;
byte instate = 0;

byte finalState;

byte lb[5] = {0, 0, 0, 0, 0};

bool blink = 1;

unsigned long lasmil = 0;
bool lastTog = 1;

void tog(byte x, void(*action)()){
  if(digitalRead(button[x]) == 1 && lb[x] == 0){
    action();
    lb[x] = 1;
  }
  if(digitalRead(button[x]) == 0)lb[x] = 0;
}

void selectorMin(){
if(selector > 1)selector -= 1;
}
void selectorMax(){
  if(selector < 3)selector += 1;
}
void getInstate(){
  instate = 1;
}

void blinkMode(){
  unsigned long mil = millis();
  if(mil - lasmil >= 300){   
    blink = !blink;
    lasmil = mil;
  }
  digitalWrite(led[cursor], blink);
}

void blinkAll(){
  unsigned long mil = millis();
  if(mil - lasmil >= 250){
    blink = !blink;
    lasmil = mil;
  }
  blink ? seg(8, 8) : segNull();
  for(byte i = 0; i < 9; i++){
    digitalWrite(led[i], blink);
  }
}

void cursorL(){
  if(cursor > 0 && cursor % 3 != 0)cursor -= 1;
  lastTog = 0;
}
void cursorR(){
  if(cursor % 3 != 2)cursor += 1;
  lastTog = 1;
}
void cursorT(){
  if(cursor > 2)cursor -= 3;
}
void cursorB(){
  if(cursor < 6)cursor += 3;
}
void cursorLock(){
  stateLed[cursor] = 1;
}

void searchNull(){
  if (stateLed[cursor] == 1) {
    if(lastTog == 0){
      cursor--;
    }
    if(lastTog == 1){
      cursor++;
    }
  }
  if(cursor == 9){
    lastTog = 0;
    cursor = 8;
  }
  if(cursor == -1){
    lastTog = 1;
    cursor = 0;
  }
}

void end(){
  for(byte i = 0; i < 9; i++){
    if(stateLed[i] == 0){
      finalState = 0;
      break;
    }else if(stateLed[i] == 1){
      finalState = 1;
    }
  }
  if(finalState == 1){
    for(byte i = 0; i < 9; i++){
      digitalWrite(led[i], 0);
    }
    blinkAll();
  }
}

void gameMode1(){
  seg(counter1 / 10, counter1 % 10);
  counter1++;
  for(byte i = 0; i < 8; i++){
    digitalWrite(led[mode1[i]], 1);
    delay((1000 - 50)/8);
  }
  for(byte i = 0; i < 9; i++){
    digitalWrite(led[i], 0);
  }
  if(counter1 == 100)counter1 = 0;
}

void gameMode2(){
  long pot = analogRead(A0);
  maping = pot * 100 / 1024;
  byte mapLed = pot * 9 / 1024;

  seg(digit1(maping), digit2(maping));
  if(mapLed == 0 || mapLed == 8){
    for(byte i = 0; i < 9; i++){
      digitalWrite(led[i], 0);
    }
  }else{
    digitalWrite(led[mode2[mapLed-1]], 1);
  }
}

void gameMode3(){
  tog(top, cursorT);
  tog(bottom, cursorB);
  tog(left, cursorL);
  tog(right, cursorR);
  tog(middle, cursorLock);
  
  searchNull();

  if(finalState == 0){
    segNull();
    blinkMode();
    for(byte i = 0; i < 9; i++){
      if(cursor != i)digitalWrite(led[i], 0);
      digitalWrite(led[i], stateLed[i]);
    }
  }

  end();
}

void loop() {\
  if(instate == 0){
    tog(middle, getInstate);
    tog(top, selectorMin);
    tog(bottom, selectorMax);

    seg(digit1(selector), digit2(selector));
  }

  if(instate == 1){
    switch(selector){
      case 1:
        gameMode1();
        delay(50);
        break;
      case 2:
        gameMode2();
        delay(100);
        break;
      case 3:
        gameMode3();
        delay(50);
        break;
    }
  }
}
