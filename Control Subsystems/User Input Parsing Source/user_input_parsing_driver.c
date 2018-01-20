/*
* File:    user_input_parsing_driver.c
* Author:  Zachary Downum
*/

#include "mcc_generated_files/mcc.h"

//FCY is based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define XTAL_PERIOD ((double)1.0 / _XTAL_FREQ)
#define FCY (_XTAL_FREQ / 2)

#include <stdlib.h>
#include <stdio.h>
#include <libpic30.h>
#include <xc.h>

#include "PWM.h"

#define RISING_EDGE_TRIGGER_SETTING 0b011
#define FALLING_EDGE_TRIGGER_SETTING 0b010

//the PWM_Module must, unfortunately, be a global variable in this example
//because it must be accessable inside of the IC interrupt (a system interrupt
//that takes no arguments)
PWM_Module testMotor;

//this interrupt service routine is where all of the logic for what happens
//when the IC module is triggered should go
//it is assumed that the input signal will be a 3.3V square wave (PWM)
void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void)
{
    static double signalPeriod = -1;
    static double signalDutyCycle = -1;
    static int priorRisingTime = -1;
    static int risingTime = -1;
    static int fallingTime = -1;
    
    //This method will result in an initial duty-cycle that is non-zero
    //However, this is only for a very brief moment of time and should not
    //impact overall performance (and is only on startup)
    if (IC1CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        //the rising edge is how the frequency/period of the signal will be
        //measured, so the prior one must be captured for comparison
        priorRisingTime = risingTime;
        risingTime = IC1BUF;

        if (priorRisingTime >= 0)
        {
            //the number of clock cycles per signal period = risingTime - priorRisingTime
            //(e.g. the time between rising edges)
            //signal period = (number of clock cycles per period) * (time per clock cycle)
            //these calculations have been condensed to one operation to reduce the number
            //of variables (and thus PIC memory) used
            signalPeriod = (risingTime - priorRisingTime) * XTAL_PERIOD;

            //sets the PWM frequency to that of the signal's frequency
            testMotor.frequency = (double)1.0 / signalPeriod;
            
            //time the signal was "on" = fallingTime - priorRisingTime
            //(the reason that it is priorRisingTime and not risingTime is because
            //priorRisingTime is the rising edge associated with the fallingTime value)
            //duty cycle = (time the signal was "on") / (signal period)
            testMotor.dutyCycle = (fallingTime - priorRisingTime) / signalPeriod;
        }
        
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

    //sets pin RP2 to be an input pin
    TRISBbits.TRISB2 = 1;
    
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

void PWM_Module_Initialize(PWM_Module* motor)
{
    motor->Initialize = PWM_LeftMotorInitialize;
    motor->GetCurrentDutyCycle = PWM_GetLeftMotorDutyCycle;
    motor->GetCurrentFrequency = PWM_GetLeftMotorFrequency;
    motor->UpdateDutyCycle = PWM_UpdateLeftMotorDutyCycle;
    motor->UpdateFrequency = PWM_UpdateLeftMotorFrequency;
}

int main(void)
{
    SYSTEM_Initialize();
    PIC_Initialize();

    //testMotor should be guaranteed to be initialized before the
    //start of the IC module interrupt
    PWM_Module_Initialize(&testMotor);
    testMotor.Initialize();

    Example_IC_Initialize();

    while(true)
    {
        //theoretically, assuming an input signal frequency of 60Hz
        //(which is approximately that of the actual remote controller)
        //then the delay between updates should be no smaller than
        //1 / 60 seconds (~16.7 ms).  Any smaller and the "update" will
        //not be using any new information
        __delay_ms(50);
        testMotor.UpdateDutyCycle(&testMotor);
        testMotor.UpdateFrequency(&testMotor);
    }
    
    return -1;
}