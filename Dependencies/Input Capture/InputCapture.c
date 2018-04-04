/*
* File:    InputCapture.h
* Author:  Zachary Downum
*/

#include "mcc_generated_files/mcc.h"
#include "InputCapture.h"

//FCY is based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define FCY ((double)_XTAL_FREQ / 2)

#define TIMER_PRESCALER 64
#define TIMER_FREQUENCY ((double)FCY / TIMER_PRESCALER)

#define true 1
#define false 0

#define RISING_EDGE_TRIGGER_SETTING 0b011
#define FALLING_EDGE_TRIGGER_SETTING 0b010


//this buffer will be used by the interrupt to store values used to
//calculate duty cycle % and frequency.
//It will also be used by the Update function to calculate the duty
//cycle and frequency and store those into the IC_Module's variables
//this is true of all buffers initialized in this file
IC_Buffer IC1_Buffer;
IC_Buffer IC2_Buffer;
IC_Buffer IC3_Buffer;
Count_Monitor_Buffer IC4_Buffer;
IC_Buffer IC5_Buffer;
IC_Buffer IC6_Buffer;

//One issue with storing the rising and falling times
//is that it is unknown when the user will call the Update function
//on an IC module.  If they call Update when the rising and falling
//times are mismatched (the rising time is from period 2, but the
//falling time is still from period 1), then the calculations performed
//using those values will be incorrect.

//Fortunately, each IC module has a built in buffer that can store up to
//4 values in a FIFO pipeline.  This can be used to ensure the three
//variables are updated atomically before they are used again by the
//main program.

//This interrupt stores all 3 major values (current rising and falling times,
//as well as the prior rising time) in one atomic operation (the same interrupt phase)
//so that there is never a race condition.  

//right after fallingTime is collected, a function will be called to move all
//of the data from the "buffering" variables into the 3 regular variables

//these regular variables are the ones that will be used by all of the IC functions

//in this way, all 3 variables are guaranteed to be taken from the same period
//(even if it is delayed by a period), which will mitigate race conditions
void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void)
{
    //On a rising edge, the buffer is not read from (but the data is still kept in
	//the buffer for later).  The only change is that it changes to falling-edge-trigger mode
    if (IC1CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        IC1CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC1CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
		//because each input compare module has a built-in 4 value FIFO buffer,
		//the risingTime persists in the buffer even after the rising edge was triggered
		//we now pull this value out and put it in risingTime, and store the previous
		//risingTime value to estimate the input signal's frequency
		IC1_Buffer.priorRisingTime = IC1_Buffer.risingTime;
        IC1_Buffer.risingTime = IC1BUF;
		
		//the falling edge is the other piece of the puzzle that allows
		//the frequency and period to be measured
		//fallingTime - risingTime = number of clock cycles the pwm is "on"
		//("on" time) / (period time) = duty cycle %
		//because this was the last value stored in the buffer, it must be the last value
		//to be retrieved from the buffer
        IC1_Buffer.fallingTime = IC1BUF;
        
        IC1CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS0bits.IC1IF = 0;
}

void IC1_Initialize(IC_Module* IC1_Module)
{   
    //disables the IC1 module while it is configured
    IC1CON1 = 0x0000;
    
    IC1_Module->dutyCyclePercentage = 0;
    IC1_Module->frequency = 0;
    
    //configures pin B4/RP4 as an input (unnecessary in this case, as RPI4
    //is only ever an input pin anyway)
	TRISBbits.TRISB4 = 1;
	Nop();
    
    //See page 174 in the PIC24FJ128GA204 family documentation to find the list
    //of all values that can be mapped to the remappable pins
    //RPINR7 is the register that maps IC1 module functionality to a particular
    //remappable pin, and 4 is RPI4's remappable pin number
    RPINR7bits.IC1R = 0b100;
    
    //clears the IC module's buffer of any previous data
    //ICxBUF contains the value of its associated timer at the point when the
    //input capture event occurred
    while (IC1CON1bits.ICBNE)
    {
        int junk = IC1BUF;
    }
    
    //turns timer1 off to configure it
    T1CON = 0b0000000000000000;
    //sets a 1:64 input clock prescaler, which increments the timer every
    //8th clock cycle (from Fcy by default)
    T1CONbits.TCKPS = 0b10;
    //sets this timer's clock source to Fcy
    T1CONbits.TCS = 0b0;
    //turns timer1 back on after it is configured
    T1CONbits.TON = 1;
    
    //desyncs the IC module from any other module as we do not want it to
    //operate in tandem with any other module
    IC1CON2bits.SYNCSEL = 0b00000;
    
    //the module uses timer1 (prescaler 1:8 based on Fcy) as its timing source
    IC1CON1bits.ICTSEL = 0b100;
    
    //sets the IC module to generate an interrupt on every capture event
    IC1CON1bits.ICI = 0b00;
    
    //configures the IC module to capture events on each rising edge
    //this is the IC module's initial configuration, which will not always be
    //the case
    IC1CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    
    //sets the IC1 module interrupt to a priority of 1 (lowest)
	//see page 89 in the PIC24FJ128GA202 documentation for the list of interrupt
	//vector registers
    IPC0bits.IC1IP = 1;
    
    //turns the flag off that is used to notify the PIC that the interrupt
    //has occurred and the interrupt service routine needs to be called
    IFS0bits.IC1IF = false;
    
    //enables the interrupt
    IEC0bits.IC1IE = true;
}

void IC1_Update(IC_Module* IC1_Module)
{
	//these are the basic properties of a standard PWM square wave signal
	int logicHighClockCycles = IC1_Buffer.fallingTime - IC1_Buffer.risingTime;
	int fullPeriodClockCycles = IC1_Buffer.risingTime - IC1_Buffer.priorRisingTime;
	
    //multiplied by 100, so 10.5 represents 10.5%
	IC1_Module->dutyCyclePercentage = (double) logicHighClockCycles / fullPeriodClockCycles * 100;
	
	double secondsPerPeriod = (double) fullPeriodClockCycles / TIMER_FREQUENCY;
	
	IC1_Module->frequency = (double) 1.0 / secondsPerPeriod;
}



void __attribute__ ((__interrupt__, auto_psv)) _IC2Interrupt(void)
{
    if (IC2CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        IC2CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC2CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
		IC2_Buffer.priorRisingTime = IC2_Buffer.risingTime;
        IC2_Buffer.risingTime = IC2BUF;
        IC2_Buffer.fallingTime = IC2BUF;
        
        IC2CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS0bits.IC2IF = 0;
}

void IC2_Initialize(IC_Module* IC2_Module)
{
    IC2CON1 = 0x0000;
    
    IC2_Module->dutyCyclePercentage = 0;
    IC2_Module->frequency = 0;
    
	TRISBbits.TRISB5 = 1;
	Nop();
    
	//the 5 represents RP5
    RPINR7bits.IC2R = 0b101;
    
    while (IC2CON1bits.ICBNE)
    {
        int junk = IC2BUF;
    }
    
    T1CON = 0b0000000000000000;
    T1CONbits.TCKPS = 0b10;
    T1CONbits.TCS = 0b0;
    T1CONbits.TON = 1;
    
    IC2CON2bits.SYNCSEL = 0b00000;
    IC2CON1bits.ICTSEL = 0b100;
    IC2CON1bits.ICI = 0b00;
    IC2CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
	
    IPC1bits.IC2IP = 1;
    IFS0bits.IC2IF = false;
    IEC0bits.IC2IE = true;
}

void IC2_Update(IC_Module* IC2_Module)
{
	int logicHighClockCycles = IC2_Buffer.fallingTime - IC2_Buffer.risingTime;
	int fullPeriodClockCycles = IC2_Buffer.risingTime - IC2_Buffer.priorRisingTime;
	
	IC2_Module->dutyCyclePercentage = ((double) logicHighClockCycles / fullPeriodClockCycles) * 100;
	
	double secondsPerPeriod = (double) fullPeriodClockCycles / TIMER_FREQUENCY;
	
	IC2_Module->frequency = (double) 1.0 / secondsPerPeriod;
}



void __attribute__ ((__interrupt__, auto_psv)) _IC3Interrupt(void)
{
    if (IC3CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        IC3CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC3CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
		IC3_Buffer.priorRisingTime = IC3_Buffer.risingTime;
        IC3_Buffer.risingTime = IC3BUF;
        IC3_Buffer.fallingTime = IC3BUF;
        
        IC3CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS2bits.IC3IF = 0;
}

void IC3_Initialize(IC_Module* IC3_Module)
{
    IC3CON1 = 0x0000;
    
    IC3_Module->dutyCyclePercentage = 0;
    IC3_Module->frequency = 0;
    
	TRISBbits.TRISB6 = 1;
	Nop();
    
	//the 6 represents RP6
    RPINR8bits.IC3R = 0b110;
    
    while (IC3CON1bits.ICBNE)
    {
        int junk = IC3BUF;
    }
    
    T1CON = 0b0000000000000000;
    T1CONbits.TCKPS = 0b10;
    T1CONbits.TCS = 0b0;
    T1CONbits.TON = 1;
    
    IC3CON2bits.SYNCSEL = 0b00000;
    IC3CON1bits.ICTSEL = 0b100;
    IC3CON1bits.ICI = 0b00;
    IC3CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
	
    IPC9bits.IC3IP = 1;
    IFS2bits.IC3IF = false;
    IEC2bits.IC3IE = true;
}

void IC3_Update(IC_Module* IC3_Module)
{
	int logicHighClockCycles = IC3_Buffer.fallingTime - IC3_Buffer.risingTime;
	int fullPeriodClockCycles = IC3_Buffer.risingTime - IC3_Buffer.priorRisingTime;
	
	IC3_Module->dutyCyclePercentage = ((double) logicHighClockCycles / fullPeriodClockCycles) * 100;
	
	double secondsPerPeriod = (double) fullPeriodClockCycles / TIMER_FREQUENCY;
	
	IC3_Module->frequency = (double) 1.0 / secondsPerPeriod;
}


//this interrupt will be different because it is tied to monitoring
//the number of pulses sent to the stepper motor
//it triggers only on rising edges, counts the number of pulses,
//and prevents further motion by setting a flag to notify
//the main process if the stepper is too far to the left or right
//This flag is updated automatically and usable any time
void __attribute__ ((__interrupt__, auto_psv)) _IC4Interrupt(void)
{
    if (LATAbits.LATA2 == 0)
    {
        IC4_Buffer.numberOfCounts--;
    }
    else
    {
        IC4_Buffer.numberOfCounts++;
    }
    
    IFS2bits.IC4IF = 0;
}

void IC4_Initialize(Count_Monitor* IC4_Module)
{
    IC4CON1 = 0x0000;
    
    IC4_Module->numberOfCounts = 0;
    
	TRISBbits.TRISB7 = 1;
	Nop();
    
	//the 7 represents RP7
    RPINR8bits.IC4R = 0b111;
    
    while (IC4CON1bits.ICBNE)
    {
        int junk = IC4BUF;
    }
    
    T1CON = 0b0000000000000000;
    T1CONbits.TCKPS = 0b10;
    T1CONbits.TCS = 0b0;
    T1CONbits.TON = 1;
    
    IC4CON2bits.SYNCSEL = 0b00000;
    IC4CON1bits.ICTSEL = 0b100;
    IC4CON1bits.ICI = 0b00;
    IC4CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
	
    IPC9bits.IC4IP = 1;
    IFS2bits.IC4IF = false;
    IEC2bits.IC4IE = true;
}

void IC4_Update(Count_Monitor* IC4_Module)
{
	
}



void __attribute__ ((__interrupt__, auto_psv)) _IC5Interrupt(void)
{
    if (IC5CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        IC5CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC5CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
		IC5_Buffer.priorRisingTime = IC5_Buffer.risingTime;
        IC5_Buffer.risingTime = IC5BUF;
        IC5_Buffer.fallingTime = IC5BUF;
        
        IC5CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS2bits.IC5IF = 0;
}

void IC5_Initialize(IC_Module* IC5_Module)
{
    IC5CON1 = 0x0000;
    
    IC5_Module->dutyCyclePercentage = 0;
    IC5_Module->frequency = 0;
    
    ANSBbits.ANSB9 = 0;
	TRISBbits.TRISB9 = 1;
	Nop();
    
	//the 8 represents RP8
    RPINR9bits.IC5R = 0b1000;
    
    while (IC5CON1bits.ICBNE)
    {
        int junk = IC5BUF;
    }
    
    T1CON = 0b0000000000000000;
    T1CONbits.TCKPS = 0b10;
    T1CONbits.TCS = 0b0;
    T1CONbits.TON = 1;
    
    IC5CON2bits.SYNCSEL = 0b00000;
    IC5CON1bits.ICTSEL = 0b100;
    IC5CON1bits.ICI = 0b00;
    IC5CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
	
    IPC9bits.IC5IP = 1;
    IFS2bits.IC5IF = false;
    IEC2bits.IC5IE = true;
}

void IC5_Update(IC_Module* IC5_Module)
{
	int logicHighClockCycles = IC5_Buffer.fallingTime - IC5_Buffer.risingTime;
	int fullPeriodClockCycles = IC5_Buffer.risingTime - IC5_Buffer.priorRisingTime;
	
	IC5_Module->dutyCyclePercentage = ((double) logicHighClockCycles / fullPeriodClockCycles) * 100;
	
	double secondsPerPeriod = (double) fullPeriodClockCycles / TIMER_FREQUENCY;
	
	IC5_Module->frequency = (double) 1.0 / secondsPerPeriod;
}



void __attribute__ ((__interrupt__, auto_psv)) _IC6Interrupt(void)
{
    if (IC6CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        IC6CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC6CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
		IC6_Buffer.priorRisingTime = IC6_Buffer.risingTime;
        IC6_Buffer.risingTime = IC6BUF;
        IC6_Buffer.fallingTime = IC6BUF;
        
        IC6CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS2bits.IC6IF = 0;
}

void IC6_Initialize(IC_Module* IC6_Module)
{
    IC6CON1 = 0x0000;
    
    IC6_Module->dutyCyclePercentage = 0;
    IC6_Module->frequency = 0;
    
	TRISBbits.TRISB11 = 1;
	Nop();
    
	//the 11 represents RP11
    RPINR9bits.IC6R = 0b1011;
    
    while (IC6CON1bits.ICBNE)
    {
        int junk = IC6BUF;
    }
    
    T1CON = 0b0000000000000000;
    T1CONbits.TCKPS = 0b10;
    T1CONbits.TCS = 0b0;
    T1CONbits.TON = 1;
    
    IC6CON2bits.SYNCSEL = 0b00000;
    IC6CON1bits.ICTSEL = 0b111;
    IC6CON1bits.ICI = 0b00;
    IC6CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
	
    IPC10bits.IC6IP = 1;
    IFS2bits.IC6IF = false;
    IEC2bits.IC6IE = true;
}

void IC6_Update(IC_Module* IC6_Module)
{
	int logicHighClockCycles = IC6_Buffer.fallingTime - IC6_Buffer.risingTime;
	int fullPeriodClockCycles = IC6_Buffer.risingTime - IC6_Buffer.priorRisingTime;
	
	IC6_Module->dutyCyclePercentage = ((double) logicHighClockCycles / fullPeriodClockCycles) * 100;
	
	double secondsPerPeriod = (double) fullPeriodClockCycles / TIMER_FREQUENCY;
	
	IC6_Module->frequency = (double) 1.0 / secondsPerPeriod;
}