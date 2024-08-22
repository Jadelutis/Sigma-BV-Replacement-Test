#include <Arduino.h>

#define BUSY 1 // Busy signal BV pin 3
#define ACK 2 // Acknowledge signal BV pin 4
#define ENBL 4 // Enable Acceptor signal BV pin 10
#define VEND1 6 // VEND1 signal BV pin 11
#define VEND2 8 // VEND2 signal BV pin 12
#define VEND3 10 // VEND3 signal BV pin 13

int DOLLAR = 1;
int FIVE = 2;
int TEN = 3;
int TWENTY = 4;

// 50's and 100's not recognized by machine
// int FIFTY = 5;
// int HUNDRED = 6;

int VEND[] = {VEND1, VEND2, VEND3};
int DENOMS[] = {DOLLAR, FIVE, TEN, TWENTY};

int time_wait = 280; // Wait time for game to recognize signal is valid

void sendCredit(int denom) {
  if(digitalRead(ENBL)) {
    delay(10);
    //BV disabled by host
  }
  int counter = 0;
  bool VEND_SIG[] = {false, false, false};
  // Drive BUSY and VEND signals low to inform machine bill is being accepted
  digitalWrite(BUSY, LOW);
  digitalWrite(VEND1, LOW);
  digitalWrite(VEND2, LOW);
  digitalWrite(VEND3, LOW);
  delay(time_wait);
  // Invert bits because machine uses idle high signals
  int denomWrite = ~denom;
  // And with 0x07 (0b00000111) to extract the last three bits
  denomWrite &= 0x7;
  // Set bits according to denomination
  for(int i=0; i<3; i++) {
    // Extract bit according to signal line
    int bit = ((denomWrite>>i)&0x01);
    // Set bit and note which were set
    if(!bit) {VEND_SIG[i] = true;}
    digitalWrite(VEND[i], bit);
  }
  // Wait for ACK signal to go low signaling machine has acknowledge the signal
  while(digitalRead(ACK)) {
    // If ACK signal does not go low after 20ms, machine is not accepting the bill and we should restart
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
  // Wait for ACK line to return to high state
  while(digitalRead(ACK) == 0) {}
  // Set all VEND signals to low
  for(int i=0; i<3; i++) {
    digitalWrite(VEND[i], LOW);
  }
  delay(time_wait);
  // Set VEND signals to correct binary representations for denominations
  for(int i=0; i<3; i++) {
    int bit = ((denomWrite>>i)&0x01);
    digitalWrite(VEND[i], bit);
  }
  delay(time_wait);
  // Set the VEND lines that were low back to idle high state
  for(int i=0; i<3; i++) {
    if(VEND_SIG[i]) {
      digitalWrite(VEND[i], VEND_SIG[i]);
    }
  }
  delay(10);
  // We are done! Set busy back to high to indicate we are done accepting the bill
  digitalWrite(BUSY, HIGH);
}

void setup() {
  int outputs[] = {BUSY, VEND1, VEND2, VEND3};
  for(int i=0; i<sizeof(outputs)/sizeof(outputs[0]); i++) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], HIGH);
  }
  pinMode(ENBL, INPUT_PULLUP);
  pinMode(ACK, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(ENBL)) {
    delay(10);
    //Machine has BV disabled and will not listen to comms
  }
  for(int d=0; d<sizeof(DENOMS)/sizeof(DENOMS[0]); d++) {
    delay(2500);
    sendCredit(DENOMS[d]);
  }
}