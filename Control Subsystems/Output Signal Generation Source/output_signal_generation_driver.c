/*
 * File:   output_signal_generation_driver.c
 * Author: zdown
 *
 * Created on January 6, 2018, 1:21 AM
 */

#include "mcc_generated_files/mcc.h"

#define __PIC24FJ128GA202__
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

void PWM_Module_Initialization(PWM_Module* left_motor, PWM_Module* right_motor)
{
    left_motor->Initialize = PWM_Left_Motor_Initialization;
    left_motor->GetCurrentDutyCycle = PWM_Get_Left_Motor_Duty_Cycle;
    left_motor->GetCurrentFrequency = PWM_Get_Left_Motor_Frequency;
    left_motor->UpdateDutyCycle = PWM_Update_Left_Motor_Duty_Cycle;
    left_motor->UpdateFrequency = PWM_Update_Left_Motor_Frequency;
    
    right_motor->Initialize = PWM_Right_Motor_Initialization;
    right_motor->GetCurrentDutyCycle = PWM_Get_Right_Motor_Duty_Cycle;
    right_motor->GetCurrentFrequency = PWM_Get_Right_Motor_Frequency;
    right_motor->UpdateDutyCycle = PWM_Update_Right_Motor_Duty_Cycle;
    right_motor->UpdateFrequency = PWM_Update_Right_Motor_Frequency;
}

int main(void)
{
    OSCILLATOR_Initialize();
    PIC_Initialization();

    PWM_Module Left_Motor;
    PWM_Module Right_Motor;
    PWM_Module_Initialization(&Left_Motor, &Right_Motor);
    
    Left_Motor.Initialize(&Left_Motor);
    Right_Motor.Initialize(&Right_Motor);
    
    
    Left_Motor.frequency = 100;
    Left_Motor.UpdateFrequency(&Left_Motor);
    
    while(true)
    {
        Left_Motor.duty_cycle_percentage = 10;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 20;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 30;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 40;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 50;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 60;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 70;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 80;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 90;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
        Left_Motor.duty_cycle_percentage = 100;
        Left_Motor.UpdateDutyCycle(&Left_Motor);
        __delay_ms(1000);
    }
    
    return 0;
}