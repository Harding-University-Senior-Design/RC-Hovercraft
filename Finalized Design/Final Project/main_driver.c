/*
 * File:   kill_switch_driver.c
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
#define MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE (double)((MINIMUM_INPUT_SIGNAL_DUTY_CYCLE + MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE) / 2)

//basic initialization for all pins
void PIC_Initialization(void)
{
    //changes all pins to digital
    ANSA = 0x0000;
    ANSB = 0x0000;
	Nop();
    
    //changes all pins to output (except for RPI4, which is an input only pin)
    TRISA = 0x0000;
    TRISB = 0x0000;
    Nop();
}

void IC_Module_Initialize(IC_Module* kill_switch_input, IC_Module* propulsion_throttle_servo_input)
{
    kill_switch_input->Initialize = IC1_Initialize;
    kill_switch_input->Update = IC1_Update;
    
    propulsion_throttle_servo_input->Initialize = IC2_Initialize;
    propulsion_throttle_servo_input->Update = IC2_Update;
}

void PWM_Module_Initialize(PWM_Module* propulsion_thrust_servo_output)
{
    propulsion_thrust_servo_output->Initialize = PWM_OC1_Initialize;
    propulsion_thrust_servo_output->GetDutyCycle = PWM_Get_OC1_DutyCycle;
    propulsion_thrust_servo_output->GetFrequency = PWM_Get_OC1_Frequency;
    propulsion_thrust_servo_output->UpdateDutyCycle = PWM_Update_OC1_DutyCycle;
    propulsion_thrust_servo_output->UpdateFrequency = PWM_Update_OC1_Frequency;
}

void Kill_Switch_Initialize(void)
{
	LATAbits.LATA0 = 0;
	LATAbits.LATA1 = 0;
}

int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialization();
	Kill_Switch_Initialize();
    
    IC_Module kill_switch_input;
    IC_Module propulsion_throttle_servo_input;
    IC_Module_Initialize(&kill_switch_input, &propulsion_throttle_servo_input);
    
    PWM_Module propulsion_throttle_servo_output;
    PWM_Module_Initialize(&propulsion_throttle_servo_output);
	
    kill_switch_input.Initialize(&kill_switch_input);
    propulsion_throttle_servo_input.Initialize(&propulsion_throttle_servo_input)
    
    propulsion_throttle_servo_output.Initialize(&propulsion_throttle_servo_output);
    
    propulsion_throttle_servo_output.frequency = 62;
    propulsion_throttle_servo_output.dutyCyclePercentage = 2;
    propulsion_throttle_servo_output.UpdateFrequency(&propulsion_throttle_servo_output);
    propulsion_throttle_servo_output.UpdateDutyCycle(&propulsion_throttle_servo_output);
    
    while(true)
    {
        __delay_ms(2);
        kill_switch_input.Update(&kill_switch_input);
        
        propulsion_throttle_servo_input.Update(&propulsion_throttle_servo_input);
        propulsion_throttle_servo_output.dutyCyclePercentage = propulsion_throttle_servo_input.dutyCyclePercentage;
        propulsion_throttle_servo_output.UpdateDutyCycle();
        
        //This is here to account for minor variations that put the input duty cycle above or below
        //the minimum or maximum input signal duty (which could cause undefined behavior on the output signal)
        if (kill_switch_input.dutyCyclePercentage < MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
            LATAbits.LATA0 = 0;
            LATAbits.LATA1 = 0;
        }
        else if (kill_switch_input.dutyCyclePercentage >= MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
            LATAbits.LATA0 = 1;
            LATAbits.LATA1 = 1;
        }
    }
    
    return -1;
}