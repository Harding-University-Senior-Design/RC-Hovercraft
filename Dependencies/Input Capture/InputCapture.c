void __attribute__ ((__interrupt__, auto_psv)) _IC1Interrupt(void)
{
    static int priorRisingTime = 0;
    static int risingTime = 0;
    static int fallingTime = 0;
    
    //This example will create a pseudo-PWM with the same duty cycle
    //and frequency of the input signal to port B5
    if (IC1CON1bits.ICM == RISING_EDGE_TRIGGER_SETTING)
    {
        //the rising edge is how the frequency/period of the signal will be
        //measured, so the prior one must be captured for comparison
		//risingTime - priorRisingTime = clock cycles of the pwm period
        priorRisingTime = risingTime;
        risingTime = IC1BUF;
        
        IC1CON1bits.ICM = FALLING_EDGE_TRIGGER_SETTING;
    }
    else if (IC1CON1bits.ICM == FALLING_EDGE_TRIGGER_SETTING)
    {
		//the falling edge is the other piece of the puzzle that allows
		//the frequency and period to be measured
		//fallingTime - risingTime = number of clock cycles the pwm is "on"
		//("on" time) / (period time) = duty cycle %
        fallingTime = IC1BUF;
        
        IC1CON1bits.ICM = RISING_EDGE_TRIGGER_SETTING;
    }
    
    IFS0bits.IC1IF = 0;
}