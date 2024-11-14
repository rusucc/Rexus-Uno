#include<Servo.h>
#include<Arduino.h>
#include<pins.h>

int currentTemperature,targetTemperature;
int currentStepsDisk,targetStepsDisk;
int currentStepsFM, targetStepsFM;

inline void runFunction(int i);
inline void calculateTemperature();
//DISK,LETCON,FM
int previousMillis[3]={0,0,0};
int deltaMillis[3]={10,10,10};

void setup() {
  // put your setup code here, to run once:
  pinMode(PWM_DC, OUTPUT); //letcon
  pinMode(PWM_MOTOR, OUTPUT);
  pinMode(STEP_DISK, OUTPUT);
  pinMode(DIR_DISK, OUTPUT);
  pinMode(STEP_FM, OUTPUT);
  pinMode(DIR_FM, OUTPUT);
  pinMode(TEMP_OUT, INPUT);
  Serial.begin(9600);
}

void loop() {
  int ms = millis();
  for(int i=0; i<3; i++){
    if(previousMillis[i]+deltaMillis[i]>=ms){
        runFunction(i);
        previousMillis[i]=ms;
    }
  }
  delay(1);
  targetTemperature = 250;
  targetStepsDisk = (ms%3000<1500)? -300:300;
}

bool DISK_DIR_CW = true; //directie disk clockwise
bool FM_DIR_CW = true;
#define STEP_PERIOD 100 //micros , de vazut cum e cu microstepping

inline void runFunction(int i){
  switch (i)
  {
  case 0://disk
    if(currentStepsDisk<targetStepsDisk) DISK_DIR_CW=true,digitalWrite(DIR_DISK,HIGH);
    else if(currentStepsDisk>targetStepsDisk) DISK_DIR_CW=false,digitalWrite(DIR_DISK,LOW);
    if(targetStepsDisk!=currentStepsDisk){
      digitalWrite(STEP_DISK,HIGH);
      delayMicroseconds(0.9*STEP_PERIOD);
      digitalWrite(STEP_DISK,LOW);
      delayMicroseconds(0.1*STEP_PERIOD);
      if(DISK_DIR_CW) currentStepsDisk++;
      else currentStepsDisk--;
    }
    break;
  case 1://temp
    digitalWrite(PWM_DC,LOW);
    delayMicroseconds(100);
    calculateTemperature();
    if(currentTemperature<targetTemperature) digitalWrite(PWM_DC,HIGH);
    else digitalWrite(PWM_DC,LOW);
    break;
  case 3: //FM
    if(currentStepsFM<targetStepsFM) FM_DIR_CW=true,digitalWrite(DIR_FM,HIGH);
    else if(currentStepsFM>targetStepsFM) FM_DIR_CW=false,digitalWrite(DIR_FM,LOW);
    if(targetStepsFM!=currentStepsFM){
      digitalWrite(STEP_FM,HIGH);
      delayMicroseconds(0.9*STEP_PERIOD);
      digitalWrite(STEP_FM,LOW);
      delayMicroseconds(0.1*STEP_PERIOD);
      if(FM_DIR_CW) currentStepsFM++;
      else currentStepsFM--;
    }
    break;
  default:
    break;
  }
}
inline void calculateTemperature() {
  float U = analogRead(TEMP_OUT);
  currentTemperature =(U * 0.3554) - 11.6426;
}