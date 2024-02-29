#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <Stepper.h>

const int irIn = 2;

IRrecv irrecv(irIn);
decode_results results;

const int steps = 100;
int b1pin = 3;
int b2pin = 4;
int onPin = 13;

int upDown = 0;
int hold = 0;
//bool On = false;

int b1 = 0;
int b2 = 0;
int n1;
int n2;
int n3;
int n4;

Stepper stepname(steps, 8, 10, 9, 11);



int AMPM = 0;    // 1 for PM, 0 for AM
int smin = 16;   // Manually enter time of when you turn on the Arduino.
int shour = 5;  // This will be the real world time when you press the power button

int sunhour = 12;  // Hour and minute when the sun rises
int sunmin = 17;
double totalstep = 0;



void setup() {
  irrecv.enableIRIn();
  irrecv.blink13(true);
  if (AMPM == 1) {
    shour = shour + 12;
  }
  pinMode(onPin, OUTPUT);
  //digitalWrite(onPin, LOW);
  Serial.begin(9600);
}

void loop() {

  long time = millis();

  int seconds = time / 1000;  // Seconds since Arduino turned on
  seconds = round(seconds);

  b1 = digitalRead(b1pin);
  b2 = digitalRead(b2pin);

  int sstep = 1050;  // 180 degrees


  int minutes = seconds / 60;  // Minutes since Arduino turned on
  int hours = seconds / 3600;  // Hours since Arduino turned on

  seconds = seconds - (minutes * 60);

  int realmin = minutes + smin;
  int realhour = hours + shour;

  if (realmin == 60) {
    smin = 0;
    realmin = minutes + smin;
    shour = shour + 1;
  }

  if (realhour == 24) {
    realhour = 0;
  }
  /*
  Serial.print(realhour);
  Serial.print(":");
  Serial.print(realmin);
  Serial.print(":");
  Serial.println(seconds);
  */
/*
For future implementation of a transistor on off switch
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFA25D){
      On = !On;
      Serial.println(On);
      digitalWrite(onPin, On);
    }
  }
*/
  if ((realhour == sunhour) && (realmin == sunmin)) {
    while (totalstep <= 19) {
      Stepper stepname(steps, 11, 9, 10, 8);
      stepname.setSpeed(200);
      stepname.step(steps);
      totalstep = totalstep + 0.09;
      Serial.println(totalstep);
    }
  } else if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFF906F:
        // Move blinds to closed position
        //Serial.println("up");
        upDown = 1;
        hold = 0;
        break;

      case 0xFFE01F:
        // Move blinds to let light in (open)
        //Serial.println("down");
        upDown = 2;
        hold = 0;
        break;

      case 0xFFC23D:
        // Move blinds to let light in (open) without holding button
        results.value = 4294967295;
        while (totalstep <= 19 && results.value == 4294967295) {
          irrecv.resume();
          Stepper stepname(steps, 11, 9, 10, 8);
          stepname.setSpeed(200);
          stepname.step(steps);
          totalstep = totalstep + 0.09;
          Serial.println(totalstep);
          if (irrecv.decode(&results)){
            if(results.value != 4294967295){
              irrecv.resume();
            }
          }
          
        }
        break;
      
      case 0xFF22DD:
        // Move blinds to closed position without holding button
        totalstep = 5;
        results.value = 4294967295;
        while (totalstep >= 0 && results.value == 4294967295) {
          irrecv.resume();
          //Serial.println(results.value);
          Stepper stepname(steps, 8, 10, 9, 11);
          stepname.setSpeed(200);
          stepname.step(steps);
          totalstep = totalstep - 0.09;
          Serial.println(totalstep);
          if (irrecv.decode(&results)){
            if(results.value != 4294967295){
              irrecv.resume();
            }
          }

          
        }
        break;

      case 0XFFFFFFFF:
        hold = 1;
        //Serial.println("holding");
        break;

      default:
        hold = 0;
        upDown = 0;
        break;
    }

    //Serial.println(results.value, HEX);
    irrecv.resume();
    //Serial.println(hold);
    while (hold == 1 && upDown != 0) {
      switch (upDown) {
        case 1:  // Right down arrow button (Counter clockwise) Opens blinds
          //Serial.println("case 1");
          totalstep = totalstep + 0.09;
          n1 = 11;
          n2 = 9;
          n3 = 10;
          n4 = 8;
          //Stepper stepname(steps, 11, 9, 10, 8);
          break;

        case 2:  // Left down arrow button (Clockwise) Closes blinds
          //Serial.println("case 2");
          totalstep = totalstep - 0.09;
          n1 = 8;
          n2 = 10;
          n3 = 9;
          n4 = 11;
          //Stepper stepname(steps, 8, 10, 9, 11);
          break;
      }
      Stepper stepname(steps, n1, n2, n3, n4);
      stepname.setSpeed(200);
      stepname.step(100);
      irrecv.resume();
      Serial.println(totalstep);
      hold = 0;
    }
  }
}
