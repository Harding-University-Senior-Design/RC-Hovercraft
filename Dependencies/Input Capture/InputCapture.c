/*
* File:    InputCapture.h
* Author:  Zachary Downum
*/


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

//

//right after fallingTime is collected, a function will be called to move all
//of the data from the "buffering" variables into the 3 regular variables

//these regular variables are the ones that will be used by all of the IC functions

//in this way, all 3 variables are guaranteed to be taken from the same period
//(even if it is delayed by a period), which will mitigate race conditions
void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void)
{
    static int priorRisingTime = 0;
    static int risingTime = 0;
    static int fallingTime = 0;
    
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
		priorRisingTime = risingTime;
        risingTime = IC1BUF;
		
		//the falling edge is the other piece of the puzzle that allows
		//the frequency and period to be measured
		//fallingTime - risingTime = number of clock cycles the pwm is "on"
		//("on" time) / (period time) = duty cycle %
		//because this was the last value stored in the buffer, it must be the last value
		//to be retrieved from the buffer
        fallingTime = IC1BUF;
        
        IC1CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS0bits.IC1IF = 0;
}

void IC1_Initialize(void)
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

void IC1_Update(IC_Module* IC1_Module)
{
	int logicHighClockCycles = 
}