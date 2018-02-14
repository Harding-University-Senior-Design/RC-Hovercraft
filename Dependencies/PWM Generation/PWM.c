/*
 * File:   PWM.c
 * Author: Zachary Downum
 */

#include "mcc_generated_files/mcc.h"

//FCY and TCY are based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define FCY (_XTAL_FREQ / 2)
#define TCY ((double)1.0 / FCY)

#include <xc.h>
#include <stdlib.h>
#include <libpic30.h>
#include "PWM.h"

#define PWM_ROUNDING_OFFSET 0.5

void PWM_OC1_Initialize(PWM_Module* OC1_PWM_module)
{	
    //disables the left motor's OC while the pwm is set up
    OC1CON1 = 0x0000;
    
	//sets the pin to be digital and an output to ensure PWM is generated properly on
	//the pin
	ANSBbits.ANSB0 = 0;
	TRISBbits.TRISB0 = 0;
	Nop();
    
    //sets the remappable pin RP0 to output the value of OC1 (the output compare
    //pin that is being used to generate the PWM)
    OC1_RP = OC1_Remappable_Pin_Reference;
    
    //sets the default duty cycle percentage to 0 to reflect the register values
    OC1_PWM_module->dutyCyclePercentage = 0;
    //sets the default period to 15000 (Hz) to reflect the register values
    OC1_PWM_module->frequency = 15000;
    
    OC1_PWM_module->UpdateFrequency(OC1_PWM_module);
    OC1_PWM_module->UpdateDutyCycle(OC1_PWM_module);
    
    //sets itself as the synchronization source
    OC1CON2 = 0b00011111;
    
    //sets the pwm timer to the peripheral clock (Fcy)
    OC1CON1bits.OCTSEL = 0b111;
    
    //turns on edge-aligned pwm functionality
    OC1CON1bits.OCM = 0b110;
}

double PWM_Get_OC1_DutyCycle(void)
{
	//OCR is the number of clock cycles that the module sends and active high for
	//OCRS is the number of clock cycles for the entire period
	//Therefore, OCR / OCRS = % duty cycle
    return (double)OC1R / OC1RS;
}

double PWM_Get_OC1_Frequency(void)
{
	//The period (in seconds) is defined as (OCRS + 1) * TCY in the data sheet
	//therefore 1 / ((OCRS + 1) * TCY) is the frequency
    double frequency = (double)1.0 / ((OC1RS + 1) * TCY);
    
    return frequency;
}

void PWM_Update_OC1_DutyCycle(const PWM_Module* OC1_PWM_module)
{
	//As stated before, OCR / OCRS = % duty cycle
	//Therefore, (duty cycle percentage) * OCRS / 100 = number of logic high cycles
	//This is new value is stored into OCR to edit the duty cycle, OCRS does not change
	//Because the frequency does not change
    int logicHighClockCycles = (OC1_PWM_module->dutyCyclePercentage * OC1RS / 100) + PWM_ROUNDING_OFFSET;
    OC1R = logicHighClockCycles;
}

void PWM_Update_OC1_Frequency(const PWM_Module* OC1_PWM_module)
{
	//Using the 1 / ((OCRS + 1) * TCY) equation for frequency, we first solved for OCRS
	//Then we plugged in the new frequency value to calculate the new OCRS value
	//UpdateDutyCycle is called to recalculate OCR after the new value of OCRS is stored
    double totalClockCycles = (double)1.0 / (OC1_PWM_module->frequency * TCY) - 1;
    OC1RS = (int)totalClockCycles;
    
    OC1_PWM_module->UpdateDutyCycle(OC1_PWM_module);
}


//See the OC1 module comments for why these commands were executed in this way
void PWM_OC2_Initialize(PWM_Module* OC2_PWM_module)
{
    OC2CON1 = 0x0000;
    
    ANSBbits.ANSB1 = 0;
	TRISBbits.TRISB1 = 0;
    Nop();
    
    OC2_RP = OC2_Remappable_Pin_Reference;
    
    OC2_PWM_module->dutyCyclePercentage = 0;
    OC2_PWM_module->frequency = 15000;
    
    OC2_PWM_module->UpdateFrequency(OC2_PWM_module);
    OC2_PWM_module->UpdateDutyCycle(OC2_PWM_module);
    
    OC2CON2 = 0b00011111;
    
    OC2CON1bits.OCTSEL = 0b111;
    
    OC2CON1bits.OCM = 0b110;
}

double PWM_Get_OC2_DutyCycle(void)
{
    return (double)OC2R / OC2RS;
}

double PWM_Get_OC2_Frequency(void)
{    
    double frequency = (double)1.0 / ((OC2RS + 1) * TCY);
    
    return frequency;
}

void PWM_Update_OC2_DutyCycle(const PWM_Module* OC2_PWM_module)
{
    int logicHighClockCycles = (OC2_PWM_module->dutyCyclePercentage * OC2RS / 100) + PWM_ROUNDING_OFFSET;
    OC2R = logicHighClockCycles;
}

void PWM_Update_OC2_Frequency(const PWM_Module* OC2_PWM_module)
{    
    double totalClockCycles = (double)1.0 / (OC2_PWM_module->frequency * TCY) - 1;
    OC2RS = (int)totalClockCycles;
    
    OC2_PWM_module->UpdateDutyCycle(OC2_PWM_module);
}



//See the OC1 module comments for why these commands were executed in this way
void PWM_OC3_Initialize(PWM_Module* OC3_PWM_module)
{
    OC3CON1 = 0x0000;
    
    ANSBbits.ANSB2 = 0;
	TRISBbits.TRISB2 = 0;
    Nop();
    
    OC3_RP = OC3_Remappable_Pin_Reference;
    
    OC3_PWM_module->dutyCyclePercentage = 0;
    OC3_PWM_module->frequency = 15000;
    
    OC3_PWM_module->UpdateFrequency(OC3_PWM_module);
    OC3_PWM_module->UpdateDutyCycle(OC3_PWM_module);
    
    OC3CON2 = 0b00011111;
    
    OC3CON1bits.OCTSEL = 0b111;
    
    OC3CON1bits.OCM = 0b110;
}

double PWM_Get_OC3_DutyCycle(void)
{
    return (double)OC3R / OC3RS;
}

double PWM_Get_OC3_Frequency(void)
{    
    double frequency = (double)1.0 / ((OC3RS + 1) * TCY);
    
    return frequency;
}

void PWM_Update_OC3_DutyCycle(const PWM_Module* OC3_PWM_module)
{
    int logicHighClockCycles = (OC3_PWM_module->dutyCyclePercentage * OC3RS / 100) + PWM_ROUNDING_OFFSET;
    OC3R = logicHighClockCycles;
}

void PWM_Update_OC3_Frequency(const PWM_Module* OC3_PWM_module)
{    
    double totalClockCycles = (double)1.0 / (OC3_PWM_module->frequency * TCY) - 1;
    OC3RS = (int)totalClockCycles;
    
    OC3_PWM_module->UpdateDutyCycle(OC3_PWM_module);
}



//See the OC1 module comments for why these commands were executed in this way
void PWM_OC4_Initialize(PWM_Module* OC4_PWM_module)
{
    OC4CON1 = 0x0000;
    
    ANSBbits.ANSB3 = 0;
	TRISBbits.TRISB3 = 0;
    Nop();
    
    OC4_RP = OC4_Remappable_Pin_Reference;
    
    OC4_PWM_module->dutyCyclePercentage = 0;
    OC4_PWM_module->frequency = 15000;
    
    OC4_PWM_module->UpdateFrequency(OC4_PWM_module);
    OC4_PWM_module->UpdateDutyCycle(OC4_PWM_module);
    
    OC4CON2 = 0b00011111;
    
    OC4CON1bits.OCTSEL = 0b111;
    
    OC4CON1bits.OCM = 0b110;
}

double PWM_Get_OC4_DutyCycle(void)
{
    return (double)OC4R / OC4RS;
}

double PWM_Get_OC4_Frequency(void)
{    
    double frequency = (double)1.0 / ((OC4RS + 1) * TCY);
    
    return frequency;
}

void PWM_Update_OC4_DutyCycle(const PWM_Module* OC4_PWM_module)
{
    int logicHighClockCycles = (OC4_PWM_module->dutyCyclePercentage * OC4RS / 100) + PWM_ROUNDING_OFFSET;
    OC4R = logicHighClockCycles;
}

void PWM_Update_OC4_Frequency(const PWM_Module* OC4_PWM_module)
{    
    double totalClockCycles = (double)1.0 / (OC4_PWM_module->frequency * TCY) - 1;
    OC4RS = (int)totalClockCycles;
    
    OC4_PWM_module->UpdateDutyCycle(OC4_PWM_module);
}



//See the OC1 module comments for why these commands were executed in this way
void PWM_OC5_Initialize(PWM_Module* OC5_PWM_module)
{
    OC5CON1 = 0x0000;
    
    ANSBbits.ANSB9 = 0;
	TRISBbits.TRISB9 = 0;
	Nop();
    
    OC5_RP = OC5_Remappable_Pin_Reference;
    
    OC5_PWM_module->dutyCyclePercentage = 0;
    OC5_PWM_module->frequency = 15000;
    
    OC5_PWM_module->UpdateFrequency(OC5_PWM_module);
    OC5_PWM_module->UpdateDutyCycle(OC5_PWM_module);
    
    OC5CON2 = 0b00011111;
    
    OC5CON1bits.OCTSEL = 0b111;
    
    OC5CON1bits.OCM = 0b110;
}

double PWM_Get_OC5_DutyCycle(void)
{
    return (double)OC5R / OC5RS;
}

double PWM_Get_OC5_Frequency(void)
{    
    double frequency = (double)1.0 / ((OC5RS + 1) * TCY);
    
    return frequency;
}

void PWM_Update_OC5_DutyCycle(const PWM_Module* OC5_PWM_module)
{
    int logicHighClockCycles = (OC5_PWM_module->dutyCyclePercentage * OC5RS / 100) + PWM_ROUNDING_OFFSET;
    OC5R = logicHighClockCycles;
}

void PWM_Update_OC5_Frequency(const PWM_Module* OC5_PWM_module)
{    
    double totalClockCycles = (double)1.0 / (OC5_PWM_module->frequency * TCY) - 1;
    OC5RS = (int)totalClockCycles;
    
    OC5_PWM_module->UpdateDutyCycle(OC5_PWM_module);
}



//See the OC1 module comments for why these commands were executed in this way
void PWM_OC6_Initialize(PWM_Module* OC6_PWM_module)
{
    OC6CON1 = 0x0000;
    
	TRISBbits.TRISB10 = 0;
    Nop();
    
    OC6_RP = OC6_Remappable_Pin_Reference;
    
    OC6_PWM_module->dutyCyclePercentage = 0;
    OC6_PWM_module->frequency = 15000;
    
    OC6_PWM_module->UpdateFrequency(OC6_PWM_module);
    OC6_PWM_module->UpdateDutyCycle(OC6_PWM_module);
    
    OC6CON2 = 0b00011111;
    
    OC6CON1bits.OCTSEL = 0b111;
    
    OC6CON1bits.OCM = 0b110;
}

double PWM_Get_OC6_DutyCycle(void)
{
    return (double)OC6R / OC6RS;
}

double PWM_Get_OC6_Frequency(void)
{    
    double frequency = (double)1.0 / ((OC6RS + 1) * TCY);
    
    return frequency;
}

void PWM_Update_OC6_DutyCycle(const PWM_Module* OC6_PWM_module)
{
    int logicHighClockCycles = (OC6_PWM_module->dutyCyclePercentage * OC6RS / 100) + PWM_ROUNDING_OFFSET;
    OC6R = logicHighClockCycles;
}

void PWM_Update_OC6_Frequency(const PWM_Module* OC6_PWM_module)
{    
    double totalClockCycles = (double)1.0 / (OC6_PWM_module->frequency * TCY) - 1;
    OC6RS = (int)totalClockCycles;
    
    OC6_PWM_module->UpdateDutyCycle(OC6_PWM_module);
}