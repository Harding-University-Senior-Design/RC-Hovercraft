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
	ANSA = 0x0000;
    ANSB = 0x0000;
    Nop();
    
    //changes all pins to output (except for RPI4, which is an input only pin)
	TRISA = 0x0000;
    TRISB = 0x0000;
    Nop();
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
    
    IC_Module Test_Input;
    IC_Module_Initialize(&Test_Input);
    
    Test_Input.Initialize();
    
    while(true)
    {
        __delay_ms(2);
        Test_Input.Update(&Test_Input);
        
        if (Test_Input.dutyCyclePercentage < 0)
        {
            LATA
        }
        else if (Test_Input.dutyCyclePercentage > 100)
        {
            LATA
        }
    }
    
    return -1;
}