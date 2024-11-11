#include<Servo.h>
#include<Arduino.h>

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define USE_TIMER_2     true

#include "TimerInterrupt.h"
#define TIMER2_INTERVAL_MS    5

#define LED1 4
#define STEP 7

Servo myServo;
volatile int currentSteps = 0;
int targetHalfSteps = 0;
bool arrived = true;


void stepMotor(){
  static bool level = false;
  if(currentSteps!=targetHalfSteps){
    digitalWrite(STEP,level);
    level = !level;
    currentSteps++;
    arrived=false;
  }
  else{
    arrived = true;
  }
}
void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);

  ITimer2.init();

  if (ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS, stepMotor))
  {
    Serial.print(F("Starting  ITimer2 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer2. Select another freq. or timer"));


  myServo.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  targetHalfSteps = 1000;
  delay(1);
}
