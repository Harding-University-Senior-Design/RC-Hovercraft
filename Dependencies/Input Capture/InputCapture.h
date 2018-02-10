/*
* File:    InputCapture.h
* Author:  Zachary Downum
*/
#pragma once

#define RISING_EDGE_TRIGGER_SETTING 0b011
#define FALLING_EDGE_TRIGGER_SETTING 0b010

typedef struct IC_Module IC_Module;

//this struct is designed to store information about
//pwm-type square wave input signals
struct IC_Module
{
    int priorRisingTime;
    int risingTime;
	int fallingTime;
    
    void (*Initialize)();
	void (*Update)(const struct IC_Module*);
    
    double (*GetDutyCycle)(const IC_Module*);
    double (*GetFrequency)(const IC_Module*);
};

//NOTE:  all these modules are intended to be used with
//       a 3.3V square wave (pwm-type) input signal by
//       measuring the rising and falling edges of the
//       signal.  Any attempt to use a different input
//       signal with this framework will result in undefined
//       behavior.

//this interrupt is for propulsion thrust magnitude
//will be used to manipulate the speed of the propulsion motors
//it must be named IC1Interrupt so it can
//be recognized as an IC module #1 interrupt
void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void);
void IC_1_Initialize(void);
void IC_1_Update(const IC_Module* IC1_Module);
double IC_1_GetDutyCycle(const IC_Module* IC1_Module);
double IC_1_GetFrequency(const IC_Module* IC1_Module);

//this interrupt is for propulsion thrust direction
//will be used to manipulate the propulsion rudders
//it must be named IC1Interrupt so it can
//be recognized as an IC module #1 interrupt
void __attribute__ ((__interrupt__, auto_psv)) _IC2Interrupt(void);
void IC_2_Initialize(void);
void IC_2_Update(const IC_Module* IC2_Module);
double IC_2_GetDutyCycle(const IC_Module* IC2_Module);
double IC_2_GetFrequency(const IC_Module* IC2_Module);

//this interrupt is for lift engine throttle (magnitude) control
//will be used to manipulate the servo connected to the lift engine throttle
//it must be named IC1Interrupt so it can
//be recognized as an IC module #1 interrupt
void __attribute__ ((__interrupt__, auto_psv)) _IC3Interrupt(void);
void IC_3_Initialize(void);
void IC_3_Update(const IC_Module* IC3_Module);
double IC_3_GetDutyCycle(const IC_Module* IC3_Module);
double IC_3_GetFrequency(const IC_Module* IC3_Module);

//this interrupt is for the kill switch (toggled on/off)
//will be used to disable all output signals for the following subsystems:
//    Lift Engine Throttle Control
//    Propulsion Thrust Magnitude Control
//    Propulsion Thrust Direction Control
//    Lift Engine On/Off Switch
//it must be named IC1Interrupt so it can
//be recognized as an IC module #1 interrupt
void __attribute__ ((__interrupt__, auto_psv)) _IC4Interrupt(void);
void IC_4_Initialize(void);
void IC_4_Update(const IC_Module* IC4_Module);
double IC_4_GetDutyCycle(const IC_Module* IC4_Module);
double IC_4_GetFrequency(const IC_Module* IC4_Module);

//Unused as of now in the hovercraft project, but it is here because
//the framework should have the potential to use all 6 of the IC modules if necessary
void __attribute__ ((__interrupt__, auto_psv)) _IC5Interrupt(void);
void IC_5_Initialize(void);
void IC_5_Update(const IC_Module* IC5_Module);
double IC_5_GetDutyCycle(const IC_Module* IC5_Module);
double IC_5_GetFrequency(const IC_Module* IC5_Module);

//Unused as of now in the hovercraft project, but it is here because
//the framework should have the potential to use all 6 of the IC modules if necessary
void __attribute__ ((__interrupt__, auto_psv)) _IC6Interrupt(void);
void IC_1_Initialize(void);
void IC_1_Update(const IC_Module* IC6_Module);
double IC_1_GetDutyCycle(const IC_Module* IC6_Module);
double IC_1_GetFrequency(const IC_Module* IC6_Module);