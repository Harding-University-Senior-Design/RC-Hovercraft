/*
 * File:   propulsion_motor_control_driver.c
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
#include "InputCapture.h"

//these were experimentally derived, so these may not be the optimal values
#define MINIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)7.29
#define MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)13.79

//Setting the INCREMENT_ADJUSTMENT_FACTOR to 100 achieves an output duty cycle that goes from 0% to 100%
//make the INCREMENT_ADJUSTMENT_FACTOR smaller to make the maximum output duty cycle % smaller
//make the INCREMENT_ADJUSTMENT_FACTOR larger to make the maximum output duty cycle % larger (not recommended as 100% should be the absolute max)
#define INCREMENT_ADJUSTMENT_FACTOR 100
#define OUTPUT_DUTY_CYCLE_INCREMENT ((double)(MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE - MINIMUM_INPUT_SIGNAL_DUTY_CYCLE) / INCREMENT_ADJUSTMENT_FACTOR)

//basic initialization for all pins
void PIC_Initialization(void)
{
    //changes all pins to digital
    ANSB = 0x0000;
    Nop();
    
    //changes all pins to output (except for RPI4, which is an input only pin)
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

void IC_Module_Initialize(IC_Module* test_input)
{
    test_input->Initialize = IC1_Initialize;
    test_input->Update = IC1_Update;
}

int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialization();

    PWM_Module left_motor;
	PWM_Module right_motor;
    PWM_Module_Initialize(&left_motor, &right_motor);
    
    IC_Module propulsion_motor_thrust_input;
    IC_Module_Initialize(&propulsion_motor_thrust_input);
    
    left_motor.Initialize(&left_motor);
	right_motor.Initialize(&right_motor);
	
    left_motor.frequency = 15000;
    right_motor.frequency = 15000;
	
	left_motor.UpdateFrequency(&left_motor);
    right_motor.UpdateFrequency(&right_motor);
	
    propulsion_motor_thrust_input.Initialize();
    
    while(true)
    {
        __delay_ms(2);
        propulsion_motor_thrust_input.Update(&propulsion_motor_thrust_input);
        
        left_motor.dutyCyclePercentage = (propulsion_motor_thrust_input.dutyCyclePercentage - MINIMUM_INPUT_SIGNAL_DUTY_CYCLE) / OUTPUT_DUTY_CYCLE_INCREMENT;
        
        //This is here to account for minor variations that put the input duty cycle above or below
        //the minimum or maximum input signal duty (which could cause undefined behavior on the output signal)
        if (left_motor.dutyCyclePercentage < 0)
        {
            left_motor.dutyCyclePercentage = 0;
        }
        else if (left_motor.dutyCyclePercentage > 100)
        {
            left_motor.dutyCyclePercentage = 100;
        }
		
		//the left and right motor should have identical values when calculating thrust, so there is no need to do any calculations or adjustments for the other motor
		right_motor.dutyCyclePercentage = left_motor.dutyCyclePercentage;
        
        left_motor.UpdateDutyCycle(&left_motor);
		right_motor.UpdateDutyCycle(&right_motor);
    }
    
    return -1;
}