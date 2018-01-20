/*
* File:    Input_Capture_Example.c
* Author:  Zachary Downum
*/

#include "mcc_generated_files/mcc.h"

//FCY is based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define FCY (_XTAL_FREQ / 2)

#include <stdlib.h>
#include <stdio.h>
#include <libpic30.h>
#include <xc.h>

#define RISING_EDGE_TRIGGER_SETTING 0b011
#define FALLING_EDGE_TRIGGER_SETTING 0b010

double dutyCycleOfInputSignal = 0;
double frequencyOfInputSignal = 0;

//this interrupt service routine is where all of the logic for what happens
//when the IC module is triggered should go
//it is assumed that the input signal will be a 3.3V square wave (PWM)
void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void)
{
    static bool sufficientDataCollected = false;
    static int priorRisingTime = 0;
    static int risingTime = 0;
    static int fallingTime = 0;
    
    //This example will create a pseudo-PWM with the same duty cycle
    //and frequency of the input signal to port B5
    if (IC1CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        //the rising edge is how the frequency/period of the signal will be
        //measured, so the prior one must be captured for comparison
        priorRisingTime = risingTime;
        risingTime = IC1BUF;
        
        LATBbits.LATB5 = 1;
        
        IC1CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC1CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
        fallingTime = IC1BUF;
        
        LATBbits.LATB5 = 0;
        
        IC1CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS0bits.IC1IF = 0;
}

//basic initialization for all pins
void PIC_Initialize(void)
{
    //changes all pins to digital
    ANSB = 0x0000;
    Nop();
    
    //changes all pins to output (except for RP4, which is an input only pin)
    TRISB = 0x0000;
    Nop();
}

void Example_IC_Initialize(void)
{
    //disables the IC1 module while it is configured
    IC1CON1 = 0x0000;
    
    //See page 174 in the PIC24FJ128GA204 family documentation to find the list
    //of all values that can be mapped to the remappable pins
    //RPINR7 is the register that maps IC1 module functionality to a particular
    //remappable pin, and 2 is RP2's remappable pin number
    RPINR7bits.IC1R = 0b10;
    
    //clears the IC module's buffer of any previous data
    //ICxBUF contains the value of its associated timer at the point when the
    //input capture event occurred
    while (IC1CON1bits.ICBNE)
    {
        int junk = IC1BUF;
    }
    
    //desyncs the IC module from any other module as we do not want it to
    //operate in tandem with any other module
    IC1CON2bits.SYNCSEL = 0b00000;
    
    //the module uses the system clock (Fcy) as its timing source
    IC1CON1bits.ICTSEL = 0b111;
    
    //sets the IC module to generate an interrupt on every capture event
    IC1CON1bits.ICI = 0b00;
    
    //configures the IC module to capture events on each rising edge
    //this is the IC module's initial configuration, which will not always be
    //the case
    IC1CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    
    //sets the IC1 module interrupt to a priority of 1 (lowest)
    IPC0bits.IC1IP = 1;
    
    //turns the flag off that is used to notify the PIC that the interrupt
    //has occurred and the interrupt service routine needs to be called
    IFS0bits.IC1IF = false;
    
    //enables the interrupt
    IEC0bits.IC1IE = true;
}

int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialize();
    Example_IC_Initialize();
    
    //sets pin B2 to input (rather than its default output configuration)
    TRISBbits.TRISB2 = 1;
    
    while(true)
    {
    }
    
    return -1;
}