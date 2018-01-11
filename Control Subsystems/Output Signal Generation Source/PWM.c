/*
 * File:   PWM.c
 * Author: zdown
 *
 * Created on January 6, 2018, 2:21 AM
 */

#include "mcc_generated_files/mcc.h"

//FCY and TCY are based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define FCY (_XTAL_FREQ / 2)
#define TCY ((double)1.0 / FCY)

#include <xc.h>
#include "PWM.h"

#define PWM_ROUNDING_OFFSET 0.5

void PWM_LeftMotorInitialize(PWM_Module* left_motor)
{
    //disables the left motor's OC while the pwm is set up
    Left_Motor_OC_CON1 = 0x0000;
    
    //sets the remappable pin RP0 to output the value of OC1 (the output compare
    //pin that is being used to generate the PWM)
    Left_Motor_RP = Left_Motor_Remappable_Pin_Reference;
    
    //sets the default duty cycle percentage to 0 to reflect the register values
    left_motor->duty_cycle_percentage = 0;
    //sets the default period to 20000 (Hz) to reflect the register values
    left_motor->frequency = 20000;
    
    left_motor->UpdateFrequency(left_motor);
    left_motor->UpdateDutyCycle(left_motor);
    
    //sets OC1 as the synchronization source
    Left_Motor_OC_CON2 = 0b00011111;
    
    //sets the pwm timer to the peripheral clock (Fcy)
    Left_Motor_OC_CON1bits.OCTSEL = 0b111;
    
    //turns on edge-aligned pwm functionality
    Left_Motor_OC_CON1bits.OCM = 0b110;
}

void PWM_RightMotorInitialize(PWM_Module* right_motor)
{
    //disables the left motor's OC while the pwm is set up
    Right_Motor_OC_CON1 = 0x0000;
    
    //sets the remappable pin RP0 to output the value of OC1 (the output compare
    //pin that is being used to generate the PWM)
    Right_Motor_RP = Right_Motor_Remappable_Pin_Reference;
    
    //sets the default duty cycle percentage to 0 to reflect the register values
    right_motor->duty_cycle_percentage = 0;
    //sets the default period to 20000 (Hz) to reflect the register values
    right_motor->frequency = 20000;
    
    right_motor->UpdateFrequency(right_motor);
    right_motor->UpdateDutyCycle(right_motor);
    
    //sets OC1 as the synchronization source
    Right_Motor_OC_CON2 = 0b00011111;
    
    //sets the pwm timer to the peripheral clock (Fcy)
    Right_Motor_OC_CON1bits.OCTSEL = 0b111;
    
    //turns on edge-aligned pwm functionality
    Right_Motor_OC_CON1bits.OCM = 0b110;
}

double PWM_GetLeftMotorDutyCycle(void)
{
    return (double)Left_Motor_OCR / Right_Motor_OCRS;
}

double PWM_GetRightMotorDutyCycle(void)
{
    return (double)Right_Motor_OCR / Right_Motor_OCRS;
}

double PWM_GetLeftMotorFrequency(void)
{
    double frequency;
    
    frequency = 1 / ((Left_Motor_OCRS + 1) * TCY);
    
    return frequency;
}

double PWM_GetRightMotorFrequency(void)
{    
    double frequency;
    
    frequency = 1 / ((Right_Motor_OCRS + 1) * TCY);
    
    return frequency;
}

void PWM_UpdateLeftMotorDutyCycle(const PWM_Module* left_motor)
{
    int logicHighClockCycles = (left_motor->duty_cycle_percentage * Left_Motor_OCRS / 100) + PWM_ROUNDING_OFFSET;
    Left_Motor_OCR = logicHighClockCycles;
}

void PWM_UpdateRightMotorDutyCycle(const PWM_Module* right_motor)
{
    int logicHighClockCycles = (right_motor->duty_cycle_percentage * Right_Motor_OCRS / 100) + PWM_ROUNDING_OFFSET;
    Right_Motor_OCR = logicHighClockCycles;
}

void PWM_UpdateLeftMotorFrequency(const PWM_Module* left_motor)
{    
    double totalClockCycles = (double)1.0 / (left_motor->frequency * TCY) - 1;
    Left_Motor_OCRS = (int)totalClockCycles;
    
    left_motor->UpdateDutyCycle(left_motor);
}

void PWM_UpdateRightMotorFrequency(const PWM_Module* right_motor)
{    
    double totalClockCycles = (double)1.0 / (right_motor->frequency * TCY) - 1;
    Right_Motor_OCRS = (int)totalClockCycles;
    
    right_motor->UpdateDutyCycle(right_motor);
}