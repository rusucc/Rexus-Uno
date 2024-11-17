#include <Servo.h>
#include <Arduino.h>
#include <constants.h>
#include <pins.h>
#include <Servo.h>
#include "avr8-stub.h"

#define SYSTEM_NOT_ARMED 0  // Allows TCs from GND
#define SYSTEM_ARMED_COLD 1 // SolderingUnit will stay cold during experiment run
#define SYSTEM_ARMED_HOT 2  // SolderingUnit will stay heatup during experiment run
uint8_t armedState = SYSTEM_NOT_ARMED;

Servo linearMotor;

int currentTemperature, targetTemperature;
int currentStepsDisk, targetStepsDisk;
int currentStepsFM, targetStepsFM=0;

int machineState;

inline void runFunction(int i);
inline void calculateTemperature();
void stateMachineUpdate();
void armExperiment(uint8_t state);
void disarmExperiment();
// void commsUpdate();
// DISK,LETCON,FM
#define parallelStuff 4
int previousMillis[parallelStuff] = {0, 0, 0, 0};
int deltaMillis[parallelStuff] = {1000, 1000, 1000, 2500};
unsigned long LO_millis = 0;
unsigned long tHeat = 0;
unsigned long tSolder = 0;
unsigned long tWick = 0;
unsigned long tMove1 = 0;
unsigned long tMove2 = 0;
bool heatingDone = 0, solderDone = 0, wickDone = 0, move1Done = 0, move2Done = 0;

int ms = millis();

unsigned long tStartSample = 0;

int sampleNumber = 0;
int sampleDone = 1;
int sampleState = 0; // 0-heat; 1-solder, 2-wick

void setup()
{
  // put your setup code here, to run once:
  pinMode(PWM_DC, OUTPUT); // letcon
  pinMode(PWM_MOTOR, OUTPUT);
  pinMode(STEP_DISK, OUTPUT);
  pinMode(DIR_DISK, OUTPUT);
  pinMode(STEP_FM, OUTPUT);
  pinMode(DIR_FM, OUTPUT);
  pinMode(TEMP_OUT, INPUT);
  Serial.begin(115200);
  //debug_init();

  //!!!!! Doar de arduino
  pinMode(SOE, INPUT_PULLUP);
  pinMode(SODS, INPUT_PULLUP);
  pinMode(LO, INPUT_PULLUP);
  linearMotor.attach(9);
  machineState = 5;
  delay(1000);
}

void loop()
{
  ms = millis();
  for (int i = 0; i < parallelStuff; i++)
  {
    if (previousMillis[i] + deltaMillis[i] >= ms)
    {
      runFunction(i);
      previousMillis[i] = ms;
    }
  }
  stateMachineUpdate();
  delay(100);
  //Serial.println(ms);
}

bool DISK_DIR_CW = true; // directie disk clockwise
bool FM_DIR_CW = true;
#define STEP_PERIOD 100 // micros , de vazut cum e cu microstepping

inline void runFunction(int i)
{
  switch (i)
  {
  case 0: // disk
    digitalWrite(LED_BUILTIN,HIGH);
    if (currentStepsDisk < targetStepsDisk)
      DISK_DIR_CW = true, digitalWrite(DIR_DISK, HIGH);
    else if (currentStepsDisk > targetStepsDisk)
      DISK_DIR_CW = false, digitalWrite(DIR_DISK, LOW);
    if (targetStepsDisk != currentStepsDisk)
    {
      digitalWrite(LED_BUILTIN,LOW);
      digitalWrite(STEP_DISK, HIGH);
      delayMicroseconds(0.9 * STEP_PERIOD);
      digitalWrite(STEP_DISK, LOW);
      delayMicroseconds(0.1 * STEP_PERIOD);
      if (DISK_DIR_CW)
        currentStepsDisk++;
      else
        currentStepsDisk--;
    }
    break;
  case 1: // temp
    digitalWrite(PWM_DC, LOW);
    delayMicroseconds(100);
    calculateTemperature();
    if (currentTemperature < targetTemperature)
      digitalWrite(PWM_DC, HIGH);
    else
      digitalWrite(PWM_DC, LOW);
    break;
  case 2: // FM
    if (currentStepsFM < targetStepsFM)
      FM_DIR_CW = true, digitalWrite(DIR_FM, HIGH);
    else if (currentStepsFM > targetStepsFM)
      FM_DIR_CW = false, digitalWrite(DIR_FM, LOW);
    if (targetStepsFM != currentStepsFM)
    {
      digitalWrite(STEP_FM, HIGH);
      delayMicroseconds(0.9 * STEP_PERIOD);
      digitalWrite(STEP_FM, LOW);
      delayMicroseconds(0.1 * STEP_PERIOD);
      if (FM_DIR_CW)
        currentStepsFM++;
      else
        currentStepsFM--;
    }
    break;
  case 3:
  {
    Serial.print(targetStepsDisk); // telemetrie
    Serial.print(' ');
    Serial.println(currentStepsDisk);
  }
  break;
  default:
    break;
  }
}
void stateMachineUpdate()
{
  static unsigned long lastStateChange = 0;

  switch (machineState)
  {
  case 0: // test mode, allow commands from ground
    // commsUpdate();
    break;

  case 1:
  {
    // keep this empty, the experiment starts here
    lastStateChange = millis();
    machineState++;
    break;
  }
  case 2:
    // wait for LO signal
    {
      if (digitalRead(LO) == 0)
      {
        //////Serial.println("LO detected.");
        // dataLogger.println("LO detected");
        LO_millis = millis();

        // start heating the soldering unit
        if (armedState == SYSTEM_ARMED_HOT)
        {
          targetTemperature = 295;
        }
        else if (armedState == SYSTEM_ARMED_COLD)
        {
          targetTemperature = 0;
        }

        lastStateChange = millis();
        machineState++;
      }
      break;
    }
  case 3:
    // TODO: schimbat pentru camera
    {
      if (millis() - LO_millis > 100)
      {
        // digitalWrite(CAM_LED, HIGH);
        //////Serial.println("Camera Led ON");

        lastStateChange = millis();
        machineState++;
      }
      break;
    }
  case 4:
  {
    if (millis() - LO_millis > 100)
    {
      // wait for SOE signal
      if (digitalRead(SOE) == 0)
      {
        lastStateChange = millis();
        machineState++;
      }
    }
    break;
  }
  case 5: // Move soldering unit from parking slot
  {
    linearMotor.write(linearMotor_minPos);
    targetStepsDisk += 10;
    ////Serial.println("Mutat din parking slot");
    lastStateChange = millis();
    machineState++;
    break;
  }
  case 6:

    //Serial.println("\nSample");
    //Serial.println(sampleNumber);
    //Serial.print(targetStepsDisk), ////Serial.print(' '), ////Serial.println(targetStepsFM);
    if (sampleNumber < 9)
    {
      if(sampleDone == 1) {
        Serial.println("tHeat");
        linearMotor.write(linearMotor_maxPos);
        tStartSample = ms;
        sampleDone = 0;
        heatingDone = 0, move1Done = 0, move2Done = 0, wickDone = 0, solderDone = 0;
        break;
      }
      if(ms - tStartSample >=1000 and heatingDone == 0){
        Serial.println("tSolder");
        heatingDone = 1;
        tSolder = millis();
        targetStepsFM+=50;
        break;
      }
      if(ms - tStartSample >=2000 and solderDone == 0){
        Serial.println("tMove1");
        solderDone = 1;
        linearMotor.write(linearMotor_minPos);
        targetStepsDisk += 10; //pt 18 grade;
        tMove1 = millis();
        break;
      }
      if(ms - tStartSample >= 3000 and move1Done == 0){
        Serial.println("tWick");
        linearMotor.write(linearMotor_maxPos);
        move1Done = 1;
        tWick = millis();
        break;
      }
      if(ms - tStartSample >=3500 and wickDone == 0){
        Serial.println("tMove2");
        linearMotor.write(linearMotor_minPos);
        wickDone = 1;
        targetStepsDisk += 10; //pt 18 grade;
        tMove2=millis();
        break;
        ////Serial.print(tMove2);
      }
      if(ms - tStartSample >=4000 and move2Done == 0){
        Serial.println("gata");
        move2Done = 1;
        sampleNumber++;
        break;
      }
      if(move2Done == 1) {
        sampleDone=1;
        break;
      }
    }

    lastStateChange = ms;
    if(sampleNumber>9) machineState++;

    break;

  case 7:
    disarmExperiment();
    break;

  default:
    ////Serial.println("Err, unknown state..");
    // dataLogger.println("Err, unknown state..");
    machineState = 0;
    break;
  }
}
void armExperiment(uint8_t state)
{
  if (state != SYSTEM_ARMED_HOT || state != SYSTEM_ARMED_COLD)
  {
    ////Serial.println("Invalid arming.");
    // dataLogger.println("Invalid arming.");
    return;
  }

  machineState = 1;
  armedState = state;
}

void disarmExperiment()
{
  machineState = 0;
  armedState = SYSTEM_NOT_ARMED;

  // digitalWrite(ONOFF12, LOW);
  targetTemperature = 0;
  // ToDo: Return sample disc to initial position. Then the linear motor to park position. Stop feedingMechanism.
}
inline void calculateTemperature()
{
  float U = analogRead(TEMP_OUT);
  currentTemperature = (U * 0.3554) - 11.6426;
}