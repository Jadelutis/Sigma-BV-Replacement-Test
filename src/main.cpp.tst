#include <Arduino.h>

#define BUSY 1
#define ACK 2
#define ENBL 4
#define VEND1 6
#define VEND2 8
#define VEND3 10

int DOLLAR = 1;
int FIVE = 2;
int TEN = 3;
int TWENTY = 4;
// int FIFTY = 5;
// int HUNDRED = 6;

int VEND[] = {VEND1, VEND2, VEND3};
int DENOMS[] = {DOLLAR, FIVE, TEN, TWENTY};

int time_wait = 280;

void sendCredit(int denom) {
  if(digitalRead(ENBL)) {
    delay(10);
    //BV disabled by host
  }
  int counter = 0;
  bool VEND_SIG[] = {false, false, false};
  digitalWrite(BUSY, LOW);
  digitalWrite(VEND1, LOW);
  digitalWrite(VEND2, LOW);
  digitalWrite(VEND3, LOW);
  delay(time_wait);
  int denomWrite = ~denom;
  denomWrite &= 0x7;
  for(int i=0; i<3; i++) {
    int bit = ((denomWrite>>i)&0x01);
    if(!bit) {VEND_SIG[i] = true;}
    digitalWrite(VEND[i], bit);
  }
  while(digitalRead(ACK)) {
    if(counter > 10) {
      digitalWrite(BUSY, HIGH);
      digitalWrite(VEND1, HIGH);
      digitalWrite(VEND2, HIGH);
      digitalWrite(VEND3, HIGH);
      return;
    }
    delay(2);
    counter++;
  }
  while(digitalRead(ACK) == 0) {}
  for(int i=0; i<3; i++) {
    digitalWrite(VEND[i], LOW);
  }
  delay(time_wait);
  for(int i=0; i<3; i++) {
    int bit = ((denomWrite>>i)&0x01);
    digitalWrite(VEND[i], bit);
  }
  delay(time_wait);
  for(int i=0; i<3; i++) {
    if(VEND_SIG[i]) {
      digitalWrite(VEND[i], VEND_SIG[i]);
    }
  }
  delay(10);
  digitalWrite(BUSY, HIGH);
}

void setup() {
  Serial.begin(9600);

  int outputs[] = {BUSY, VEND1, VEND2, VEND3};
  for(int i=0; i<sizeof(outputs)/sizeof(outputs[0]); i++) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], HIGH);
  }
  pinMode(ENBL, INPUT_PULLUP);
  pinMode(ACK, INPUT_PULLUP);
}

void sendOneDollarTest() {
  digitalWrite(BUSY, LOW);
  digitalWrite(VEND1, LOW);
  digitalWrite(VEND2, LOW);
  digitalWrite(VEND3, LOW);
  delay(time_wait);
  digitalWrite(VEND2, HIGH);
  digitalWrite(VEND3, HIGH);
  while(digitalRead(ACK)) {}
  while(digitalRead(ACK) == 0) {}
  digitalWrite(VEND2, LOW);
  digitalWrite(VEND3, LOW);
  delay(time_wait);
  digitalWrite(VEND2, HIGH);
  digitalWrite(VEND3, HIGH);
  delay(time_wait);
  digitalWrite(VEND1, HIGH);
  delay(10);
  digitalWrite(BUSY, HIGH);
}

void loop() {
  // Serial.print("1");
  // testRun();
  // for(int l=1; l<5; l++) {
  //   sendCredit(l);
  //   delay(5000);
  // }
  if(digitalRead(ENBL)) {
    delay(10);
    //Machine has BV disabled and will not listen to comms
  }
  for(int d=0; d<4; d++) {
    delay(2500);
    sendCredit(DENOMS[d]);
  }
}