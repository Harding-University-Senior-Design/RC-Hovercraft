//set the specific PIC to be used so xc.h includes the proper
//headers automatically
//Not necessary if using MPLAB X IDE because the proper PIC is
//recognized automatically.  It is only included as a failsafe.
#define __PIC24FJ128GA202__
#define FCY (_XTAL_FREQ / 2)

#include <xc.h>
#include <stdlib.h>
#include <libpic30.h>


//aliases for additional layers of abstraction
#define true 1
#define false 0



//basic initialization for all pins
void PIC_Initialization(void)
{
    //changes all Port B pins to digital
    ANSB = 0x0000;
    Nop();
    
    //changes all Port B pins to output (except for RP4, which is an input only pin)
    TRISB = 0x0000;
    Nop();
}

//uses OC1's PWM functionality and the RB0 pin
void Example_PWM_Initialize(void)
{
    //disables OC1 while the pwm is set up
    OC1CON1 = 0x0000;
    
    //sets the remappable pin RP0 to output the value of OC1 (the output compare
    //pin that is being used to generate the PWM)
    //See page 174 in the PIC24FJ128GA204 family documentation to find the list
    //of all values that can be mapped to the remappable pins
    RPOR0 = 13;
    
    //OCxR / OCxRS = duty cycle %
    OC1R = 0x0031;
    //sets the period
    OC1RS = 0x0063;
    
    //sets OC1 as the synchronization source
    OC1CON2 = 0x001F;
    OC1CON2bits.OCTRIG = 0;
    
    //sets the pwm timer to the system clock (Fcy)
    //(bits set to 111)
    OC1CON1bits.OCTSEL = 0b111;
    
    //turns on edge-aligned pwm functionality (bits set to 110)
    OC1CON1bits.OCM = 0b110;
}

int main(void)
{
	Example_PWM_Initialize();
	
	//gradually alternates the PWM from a low to high duty cycle,
	//then back to a low duty cycle
	while (true)
    {
        int i = 0;
        for (i = 0; i <= 0x0063; i++)
        {
            OC1R++;
            __delay_ms(10);
        }

        __delay_ms(1000);
        
        for (i = 0; i <= 0x0063; i++)
        {
            OC1R--;
            __delay_ms(10);
        }
		
        __delay_ms(1000);
    }
    
    return EXIT_SUCCESS;
}