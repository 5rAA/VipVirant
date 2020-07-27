#include "Arduino.h"
#include <SoftwareSerial.h>

//Define interupt pins (D2 & D3)
#define encoder0PinA  3
#define encoder0PinB  2

// Incremental rotyry encoder IRD5810R is used
// which has 2048 stpes
// The qadruplle count in implemented
// so one round has 8192 values
// 1 step is 0.02436728481 mm
// 10  steps are 0.2436728481 mm
// 5 steps are 0.12183642405 mm
// premer koles = 63.54mm
//double obs = 199.616797209; //DOLZINA KROGCA
//double korak = 0.09725438195; //DOLZINA ENEGA KORAKA
#define resolucija  3
#define razlikaPolic  10
#define microdelay 1500

//Inicialite software serial object MySerial
// in RX <- TX out
// out TX -> RX in
SoftwareSerial mySerial(6, 7); // RX, TX

//Print in ARD serial monitor
bool printToSM = false;

//volatile 
// Specifically, it directs the compiler to load the variable from RAM 
// and not from a storage register, which is a temporary memory location
// where program variables are stored and manipulated. 
// Under certain conditions, the value for a variable stored in registers
// can be inaccurate.
//The use of this is necesery because  vaireable is being changed by interup
//and my be diffent than the one that is stores in storage register.
volatile long encoder0Pos = 1000000;

// For calculatinf avarege value the lasst 20 values are used
int velikostZanjihNekaj = 20;
long zadnjihNekajVr[20];
long povprecje = 1000000;
long povprecjeVsota = 1000000 * 20;
long trenuten = 0;

//The value arround whish all the differeces are being calculated
long zadnjaPolica = 1000000;

int count = 0;


//unsigned long cas = 0;

void doEncoderA() {
  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) == HIGH) {

    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == LOW) {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  else   // must be a high-to-low edge on channel A
  {
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == HIGH) {
      encoder0Pos = encoder0Pos + 1;          // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
  //mySerial.println(encoder0Pos);
  // use for debugging - remember to comment out
}

void doEncoderB() {
  // look for a low-to-high on channel B
  if (digitalRead(encoder0PinB) == HIGH) {

    // check channel A to see which way encoder is turning
    if (digitalRead(encoder0PinA) == HIGH) {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  // Look for a high-to-low on channel B

  else {
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinA) == LOW) {
      encoder0Pos = encoder0Pos + 1;          // CW
    }
    else {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
  //Serial.println (encoder0Pos, DEC);
  //mySerial.println(encoder0Pos);
}


void setup() {
  for(int i = 0; i < velikostZanjihNekaj; i++){
    zadnjihNekajVr[i] = long(1000000);
  }
  pinMode(13, OUTPUT);
  digitalWrite(13, 1);
  
  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(0, doEncoderA, CHANGE);

  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(1, doEncoderB, CHANGE);

  if (printToSM){
    Serial.begin (9600);
  }
  
  mySerial.begin(9600);

}

void loop() {

  trenuten = encoder0Pos;

  povprecjeVsota = povprecjeVsota + trenuten - zadnjihNekajVr[count];
  povprecje = povprecjeVsota / velikostZanjihNekaj;
 
  
  if(printToSM){
    Serial.println(povprecje);
  }
  
  
  delayMicroseconds(microdelay);
  
  zadnjihNekajVr[count] = trenuten;
  count++;
  if(count >= velikostZanjihNekaj){
    count = 0;
  }

  
    if(abs(povprecje -  trenuten) < resolucija  && abs(povprecje - zadnjaPolica) > razlikaPolic){
            
      
      if(printToSM){
        Serial.print("DOLZINA: ");
        Serial.println(abs(zadnjaPolica - povprecje));
      }
      
     
      mySerial.println(zadnjaPolica - povprecje);
      povprecje = 1000000;
      encoder0Pos = 1000000;
      povprecjeVsota = 1000000 * velikostZanjihNekaj;
      for(int i = 0; i < velikostZanjihNekaj; i++){
        zadnjihNekajVr[i] = long(1000000);
      }
      
  }
    
}