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

void PWM_Module_Initialize(PWM_Module* left_motor, PWM_Module* right_motor)
{
    left_motor->Initialize = PWM_OC1_Initialize;
    left_motor->GetDutyCycle = PWM_Get_OC1_DutyCycle;
    left_motor->GetFrequency = PWM_Get_OC1_Frequency;
    left_motor->UpdateDutyCycle = PWM_Update_OC1_DutyCycle;
    left_motor->UpdateFrequency = PWM_Update_OC1_Frequency;
    
    right_motor->Initialize = PWM_OC2_Initialize;
    right_motor->GetDutyCycle = PWM_Get_OC2_DutyCycle;
    right_motor->GetFrequency = PWM_Get_OC2_Frequency;
    right_motor->UpdateDutyCycle = PWM_Update_OC2_DutyCycle;
    right_motor->UpdateFrequency = PWM_Update_OC2_Frequency;
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
    
    Right_Motor.frequency = 20000;
    Right_Motor.UpdateFrequency(&Right_Motor);
    
    while(true)
    {
        int i;
        for (i = 0; i <= 100; i += 10)
        {
            Left_Motor.dutyCyclePercentage = i;
            Left_Motor.UpdateDutyCycle(&Left_Motor);
            
            Right_Motor.dutyCyclePercentage = i;
            Right_Motor.UpdateDutyCycle(&Right_Motor);
            __delay_ms(5000);
        }
    }
    
    return -1;
}