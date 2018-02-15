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
#include "InputCapture.h"

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
        
		//7.29 is approximately the lowest duty cycle of the signal
		//0.065 is the ratio of the difference in duty cycle (high - low) / 100, which makes
		//the output duty cycle 100% when the input signal is at its maximum duty cycle
        Test_Motor.dutyCyclePercentage = (Test_Input.dutyCyclePercentage - 7.29) / 0.065;
        
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