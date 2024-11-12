#include<Servo.h>
#include<Arduino.h>

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define USE_TIMER_2     true

#include "TimerInterrupt.h"
#define TIMER2_INTERVAL_MS    4

#define LED1 4
#define STEP 7

Servo myServo;
volatile int currentSteps = 0;
int targetHalfSteps = 0;
bool arrived = true;

int targetTemperature=0;
int currentTemperature = 0;

inline void calculateTemperature(float U) {
    currentTemperature = (pow(U, 4) * 1e-11 * -1.7685) +
              (pow(U, 3) * 1e-8 * 4.942) +
              (pow(U, 2) * -8.4224e-5) +
              (U * 0.3554) -
              11.6426;
}

void tim2Interrupt(){
  static bool level = false;
  if(currentSteps<targetHalfSteps){
    digitalWrite(6, HIGH); //dir
    digitalWrite(STEP,level);
    digitalWrite(LED_BUILTIN,HIGH);
    currentSteps++;
    arrived=false;
  }
  else if(currentSteps>targetHalfSteps){
    digitalWrite(6, LOW); //dir
    digitalWrite(STEP,level);
    digitalWrite(LED_BUILTIN,HIGH);
    currentSteps--;
    arrived=false;
  }
  else{
    arrived = true;
    digitalWrite(LED_BUILTIN,LOW);
  }
  if(level == false) calculateTemperature(analogRead(A0));
  else{
    if(currentTemperature<targetTemperature){
      digitalWrite(LED1,HIGH);
    }
    else{
      digitalWrite(LED1,LOW);
    }
  }
  level = !level;

}

void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(6, OUTPUT);
  Serial.begin(9600);

  ITimer2.init();

  if (ITimer2.attachInterruptInterval(TIMER2_INTERVAL_MS, tim2Interrupt))
  {
    Serial.print(F("Starting  ITimer2 OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer2. Select another freq. or timer"));

  myServo.attach(9);
  myServo.write(90);
}



void loop() {
  // put your main code here, to run repeatedly:
  targetHalfSteps = 100;
  targetTemperature = 200;
  Serial.println(targetHalfSteps);
  Serial.println(targetTemperature);
  Serial.println();
  delay(5000);
  targetHalfSteps = -100;
  targetTemperature = 250;
  Serial.println(targetHalfSteps);
  Serial.println(targetTemperature);
  Serial.println();
  delay(5000);
}
