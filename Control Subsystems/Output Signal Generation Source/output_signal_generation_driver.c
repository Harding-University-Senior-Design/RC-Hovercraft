/*
 * File:   output_signal_generation_driver.c
 * Author: zdown
 *
 * Created on January 6, 2018, 1:21 AM
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

void PWM_Module_Initialize(PWM_Module* left_motor, PWM_Module* right_motor)
{
    left_motor->Initialize = PWM_LeftMotorInitialize;
    left_motor->GetCurrentDutyCycle = PWM_GetLeftMotorDutyCycle;
    left_motor->GetCurrentFrequency = PWM_GetLeftMotorFrequency;
    left_motor->UpdateDutyCycle = PWM_UpdateLeftMotorDutyCycle;
    left_motor->UpdateFrequency = PWM_UpdateLeftMotorFrequency;
    
    right_motor->Initialize = PWM_RightMotorInitialize;
    right_motor->GetCurrentDutyCycle = PWM_GetRightMotorDutyCycle;
    right_motor->GetCurrentFrequency = PWM_GetRightMotorFrequency;
    right_motor->UpdateDutyCycle = PWM_UpdateRightMotorDutyCycle;
    right_motor->UpdateFrequency = PWM_UpdateRightMotorFrequency;
}


int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialization();

    PWM_Module Left_Motor;
    PWM_Module Right_Motor;
    PWM_Module_Initialize(&Left_Motor, &Right_Motor);
    
    Left_Motor.Initialize(&Left_Motor);
    Right_Motor.Initialize(&Right_Motor);
    
    
    Left_Motor.frequency = 20000;
    Left_Motor.UpdateFrequency(&Left_Motor);
    
    while(true)
    {
        int i;
        for (i = 0; i < 100; i += 10)
        {
            Left_Motor.duty_cycle_percentage = i;
            Left_Motor.UpdateDutyCycle(&Left_Motor);
            __delay_ms(5000);
        }
    }
    
    return 0;
}