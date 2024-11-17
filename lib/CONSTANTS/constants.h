#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "pins.h"

//Motors
const uint8_t linearMotor_Pin = PWM_MOTOR;
const uint8_t linearMotor_maxPos = 110;   //motor is soldering
const uint8_t linearMotor_minPos =  90;   //motor is retracted

const uint8_t sampleDisc_stepPin = STEP_DISK;
const uint8_t sampleDisc_dirPin = DIR_DISK;

const uint8_t feedingMechanism_stepPin = STEP_FM;
const uint8_t feedingMechanism_dirPin = DIR_FM;

const uint8_t PWM_Solder = PWM_DC; //heating thermocouple

//simple pins
//const uint8_t cameraLed_Pin = CAM_LED;
//const uint8_t DCDCconverter10V_Pin = ONOFF10;
//const uint8_t DCDCconverter12V_Pin = ONOFF12;

//Rexus signals
// const uint8_t LO_Pin = LO;
// const uint8_t SOE_Pin = SOE;

// //SoftwareSerial needs RX and TX pins
// const uint8_t softwareSerial_RXpin = RXD1;
// const uint8_t softwareSerial_TXpin = TXD1;


//State machine
#define SYSTEM_NOT_ARMED    0   //Allows TCs from GND
#define SYSTEM_ARMED_COLD   1   //SolderingUnit will stay cold during experiment run
#define SYSTEM_ARMED_HOT    2   //SolderingUnit will stay heatup during experiment run

#endif