/*
 * File:   wireless_controller_driver.c
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

void PWM_Module_Initialize(PWM_Module* test_motor)
{
    test_motor->Initialize = PWM_OC1_Initialize;
    test_motor->GetDutyCycle = PWM_Get_OC1_DutyCycle;
    test_motor->GetFrequency = PWM_Get_OC1_Frequency;
    test_motor->UpdateDutyCycle = PWM_Update_OC1_DutyCycle;
    test_motor->UpdateFrequency = PWM_Update_OC1_Frequency;
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

    PWM_Module Test_Motor;
    PWM_Module_Initialize(&Test_Motor);
    
    IC_Module Test_Input;
    IC_Module_Initialize(&Test_Input);
    
    Test_Motor.Initialize(&Test_Motor);
    Test_Motor.frequency = 15000;
    Test_Motor.UpdateFrequency(&Test_Motor);
    
    Test_Input.Initialize();
    
    while(true)
    {
        __delay_ms(2);
        Test_Input.Update(&Test_Input);
        
        Test_Motor.dutyCyclePercentage = (Test_Input.dutyCyclePercentage - MINIMUM_INPUT_SIGNAL_DUTY_CYCLE) / OUTPUT_DUTY_CYCLE_INCREMENT;
        
        //This is here to account for minor variations that put the input duty cycle above or below
        //the minimum or maximum input signal duty (which could cause undefined behavior on the output signal)
        if (Test_Motor.dutyCyclePercentage < 0)
        {
            Test_Motor.dutyCyclePercentage = 0;
        }
        else if (Test_Motor.dutyCyclePercentage > 100)
        {
            Test_Motor.dutyCyclePercentage = 100;
        }
        
        Test_Motor.UpdateDutyCycle(&Test_Motor);
    }
    
    return -1;
}