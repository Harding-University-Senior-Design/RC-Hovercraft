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

#include "InputCapture.h"

//these were experimentally derived, so these may not be the optimal values
#define MINIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)7.29
#define MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)13.79
#define MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE (double)((MINIMUM_INPUT_SIGNAL_DUTY_CYCLE + MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE) / 2)

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

void IC_Module_Initialize(IC_Module* kill_switch_input)
{
    kill_switch_input->Initialize = IC1_Initialize;
    kill_switch_input->Update = IC1_Update;
}

void Kill_Switch_Initialize(void)
{
	LATBbits.LATB6 = 0;
}

int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialization();
    
    IC_Module kill_switch_input;
    IC_Module_Initialize(&kill_switch_input);
	
    kill_switch_input.Initialize();
    
    while(true)
    {
        __delay_ms(2);
        kill_switch_input.Update(&kill_switch_input);
        
        //This is here to account for minor variations that put the input duty cycle above or below
        //the minimum or maximum input signal duty (which could cause undefined behavior on the output signal)
        if (kill_switch_input.dutyCyclePercentage < MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
            LATBbits.LATB6 = 0;
        }
        else if (kill_switch_input.dutyCyclePercentage >= MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
            LATBbits.LATB6 = 1;
        }
    }
    
    return -1;
}