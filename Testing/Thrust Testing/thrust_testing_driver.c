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
    //changes all pins to digital
    ANSB = 0x0000;
    Nop();
    
    //changes all pins to output (except for RP4, which is an input only pin)
    TRISB = 0x0000;
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


int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialization();

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