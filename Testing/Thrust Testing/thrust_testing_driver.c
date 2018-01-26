/*
 * File:   output_signal_generation_driver.c
 * Author: Zachary Downum
 */

#include "mcc_generated_files/mcc.h"

//FCY is based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define FCY (_XTAL_FREQ / 2)

#include <stdlib.h>
#include <libpic30.h>
#include <xc.h>

#include "PWM.h"

//basic initialization for all pins
void PIC_Initialization(void)
{
    //changes all Port A and B pins to digital (except pin A4, which must be analog)
	ANSA = 0b0000000000000000;
    ANSB = 0b0000000000000000;
    Nop();
	
	//changes all Port A pins to input except for RA0 and RA1 (used for motor control)
	TRISA = 0b1111111111111100;

    //changes all Port B pins to output (except for RP4, which is an input only pin)
    TRISB = 0b0000000000000000;
    Nop();
}

void PWM_Module_Initialize(PWM_Module* test_motor)
{
    test_motor->Initialize = PWM_RightMotorInitialize;
    test_motor->GetCurrentDutyCycle = PWM_GetRightMotorDutyCycle;
    test_motor->GetCurrentFrequency = PWM_GetRightMotorFrequency;
    test_motor->UpdateDutyCycle = PWM_UpdateRightMotorDutyCycle;
    test_motor->UpdateFrequency = PWM_UpdateRightMotorFrequency;
}

void Motor_Driver_Initialize()
{
	//will be used to control the direction of the motor (1 = forward, 0 = backward)
	LATAbits.LATA0 = 1;
	//will be used to prevent the motor from entering sleep mode (1 = sleep mode off)
	LATAbits.LATA1 = 1;
}


int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialization();
	Motor_Driver_Initialize();

    PWM_Module Test_Motor;
    PWM_Module_Initialize(&Test_Motor);
	
    Test_Motor.Initialize(&Test_Motor);
    
    
    Test_Motor.frequency = 20000;
    Test_Motor.UpdateFrequency(&Test_Motor);
    
    while(true)
    {
        int i;
        for (i = 0; i <= 100; i += 10)
        {
            Test_Motor.duty_cycle_percentage = i;
            Test_Motor.UpdateDutyCycle(&Test_Motor);
            __delay_ms(60000);
        }
    }
    
    return -1;
}