/*
 * File:   PWM.c
 * Author: zdown
 *
 * Created on January 6, 2018, 2:21 AM
 */

#include "mcc_generated_files/mcc.h"

#define __PIC24FJ128GA202__
#include <xc.h>
#include "PWM.h"

void PWM_Left_Motor_Initialization(PWM_Module* left_motor)
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

void PWM_Right_Motor_Initialization(PWM_Module* right_motor)
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

double PWM_Get_Left_Motor_Duty_Cycle(void)
{
    return (double)Left_Motor_OCR / Right_Motor_OCRS;
}

double PWM_Get_Right_Motor_Duty_Cycle(void)
{
    return (double)Right_Motor_OCR / Right_Motor_OCRS;
}

double PWM_Get_Left_Motor_Frequency(void)
{
    //TODO: find an actual internal value to use instead of this hard coded one
    const unsigned long CLOCK_FREQUENCY = 32000000;
    const unsigned long FCY = 2 * CLOCK_FREQUENCY;
    const double TCY = 1 / FCY;
    
    double frequency;
    double period;
    
    period = (Left_Motor_OCRS + 1) * TCY;
    frequency = 1 / period;
    
    return frequency;
}

double PWM_Get_Right_Motor_Frequency(void)
{
    //TODO: find an actual internal value to use instead of this hard coded one
    const unsigned long CLOCK_FREQUENCY = 32000000;
    const unsigned long FCY = 2 * CLOCK_FREQUENCY;
    const double TCY = 1 / FCY;
    
    double frequency;
    double period;
    
    period = (Right_Motor_OCRS + 1) * TCY;
    frequency = 1 / period;
    
    return frequency;
}

void PWM_Update_Left_Motor_Duty_Cycle(const PWM_Module* left_motor)
{
    int logicHighClockCycles = (double)left_motor->duty_cycle_percentage * Left_Motor_OCRS / 100;
    Left_Motor_OCR = logicHighClockCycles;
}

void PWM_Update_Right_Motor_Duty_Cycle(const PWM_Module* right_motor)
{
    int logicHighClockCycles = (double)right_motor->duty_cycle_percentage * Right_Motor_OCRS / 100;
    Right_Motor_OCR = logicHighClockCycles;
}

void PWM_Update_Left_Motor_Frequency(const PWM_Module* left_motor)
{
    const unsigned long FCY = _XTAL_FREQ / 2;
    const double TCY = 1.0 / FCY;
    
    double totalClockCycles = 1.0 / (left_motor->frequency * TCY) - 1;
    Left_Motor_OCRS = (int)totalClockCycles;
    
    left_motor->UpdateDutyCycle(left_motor);
}

void PWM_Update_Right_Motor_Frequency(const PWM_Module* right_motor)
{
    const unsigned long FCY = _XTAL_FREQ / 2;
    const double TCY = 1.0 / FCY;
    
    double totalClockCycles = 1.0 / (right_motor->frequency * TCY) - 1;
    Right_Motor_OCRS = (int)totalClockCycles;
    
    right_motor->UpdateDutyCycle(right_motor);
}