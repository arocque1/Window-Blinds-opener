#include <Stepper.h>

const int steps = 100;
int b1pin = 3;
int b2pin = 4;

int b1 = 0;
int b2 = 0;

Stepper stepname(steps, 8, 10, 9, 11);

int AMPM = 0;   // 1 for PM, 0 for AM
int smin = 12;  // Manually enter time of when you turn on the Arduino.
int shour = 12; // This will be the real world time when you press the power button

int sunhour = 7; // Hour and minute when the sun rises
int sunmin = 7;
int totalstep = 0;


void setup() {
  
  if(AMPM == 1){
    shour = shour + 12;
  }
  Serial.begin(9600);
}

void loop() {

  long time = millis();
  
  int seconds = time / 1000; // Seconds since Arduino turned on
  seconds = round(seconds); 

  b1 = digitalRead(b1pin);
  b2 = digitalRead(b2pin);
  
  int sstep = 1050; // 180 degrees

  //For testing stepper motor(not necessary for normal functionality)
  if(b1 == HIGH){ // Rotates Counter-Clockwise
    totalstep = totalstep - 1;
    b1 = digitalRead(b1pin);
    Stepper stepname(steps, 11, 9, 10, 8);
    stepname.setSpeed(150);
    stepname.step(sstep);
    Serial.println(totalstep);
  }
  if(b2 == HIGH){
    totalstep = totalstep + 1;
    b2 = digitalRead(b2pin);
    Stepper stepname(steps, 8, 10, 9, 11);
    stepname.setSpeed(150);
    stepname.step(sstep);
    Serial.println(totalstep);
  }

  

  int minutes = seconds / 60; // Minutes since Arduino turned on
  int hours = seconds / 3600; // Hours since Arduino turned on
  
  seconds = seconds - (minutes * 60);

  int realmin = minutes + smin;
  int realhour = hours + shour;

  if(realmin == 60){
    smin = 0;
    realmin = minutes + smin;
    shour = shour + 1;
  }

  if(realhour == 24){
    realhour = 0;
  }

  Serial.print(realhour);
  Serial.print(":");
  Serial.print(realmin);
  Serial.print(":");
  Serial.println(seconds);

  if(realhour == sunhour){
    if(realmin == sunmin){
      for(totalstep = 0; totalstep <= 18; totalstep++ ){
        Stepper stepname(steps, 11, 9, 10, 8);
        stepname.setSpeed(200);
        stepname.step(steps);
      }
    }
  }

}
