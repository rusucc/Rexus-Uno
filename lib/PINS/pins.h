#ifndef PINS_H
#define PINS_H

//signals:
#define LO PIN_PB4 //4  
#define SOE PIN_PB3 //3  
#define SODS PIN_PB2 //2

//232
#define RTS_MPU PIN_PB1 //1  
#define CTS_MPU PIN_PB0 //0  
#define USB_TXMPU PIN_PD1 //9
#define USB_RXMPU PIN_PD0 //8

//Feeding
#define STEP_FM 12 //28
#define DIR_FM 11 //23
#define EN_FM PIN_PC6 //22
#define NSLEEP_FM PIN_PC5 //21

//Disk
#define STEP_DISK 7 //20
#define DIR_DISK 6 //19
#define EN_DISK PIN_PC2 //18
#define NSLEEP_DISK PIN_PC1 //17

//DC-DC
#define ONOFF10 PIN_PE2 //34
#define ONOFF12 PIN_PD7 //15

//Motoare
#define PWM_MOTOR 9 //12 //motor linear

//Heating
#define PWM_DC 4 //14
#define TEMP_OUT A0 //A5

//optical encoder
#define OE_1 PIN_PA6 //A6
#define OE_2 PIN_PA7 //A7
#define ENC_LED PIN_PE5 //37

//camera:
//#define PWM_CAMERA PIN_PD5
//#define CAM_EN PIN_PC0 //16
//#define ADC_INPUT PIN_PA2
#define CAM_LED PIN_PE6 //38 

//RS-422
#define TXD1 PIN_PD3 //11
#define RXD1 PIN_PD2 //10

//programator
// #define SCK PB7
// #define MISO PIN_PB6
// #define MOSI PIN_PB5

#endif