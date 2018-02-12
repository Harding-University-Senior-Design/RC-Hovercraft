/* 
 * File:                PWM.h
 * Author:              Zachary Downum
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#pragma once

//aliases for additional layers of abstraction
#define true 1
#define false 0

typedef struct PWM_Module PWM_Module;

struct PWM_Module
{
    //e.g. a 90.5% duty cycle would be represented as 90.5
    double dutyCyclePercentage;
    //in Hertz
    int frequency;
    
    void (*Initialize)(struct PWM_Module*);
    
    double (*GetDutyCycle)();
    void (*UpdateDutyCycle)(const struct PWM_Module*);
    
    double (*GetFrequency)();
    void (*UpdateFrequency)(const struct PWM_Module*);
};

//see Table 11-4 in the PIC24FJ128GA202 documentation for more codes
//for remappable pin settings (starting on page 174)
//The left motor's maps to OC1
//The right motor's maps to OC2

//The order of these aliases is as follows:
//OC_CON1 --    main configuration register for the motor's PWM/OC module
//OC_CON1bits --    direct access to the CON1 register's bits
//OC_CON2 --    secondary config register for the motor's PWM/OC module
//OC_CON2bits --    direct access to the CON2 register's bits
//RP -- the configuration register for the remappable pin used by the PWM module
//RP_Reference --   the reference code sent to the remappable pin
//                              to sync it with a module
//                              (13 = OC1 for the left motor,
//                              14 = OC2 for the right motor)
//OCR --    The register that controls what portion of the period the PWM will
//          generate a logic high voltage (based on calculations using OCRS)
//OCRS --   The register that controls the period of the PWM (based on
//          calculations using the Tcy value)
//          See Example 15-1 (page 215) for calculation information
//          Essentially, OCxR / OCxRS = duty cycle % for the PWM module
#define OC1_RP RPOR0bits.RP0R
#define OC1_Remappable_Pin_Reference 13

#define OC2_RP RPOR0bits.RP1R
#define OC2_Remappable_Pin_Reference 14

#define OC3_RP RPOR1bits.RP2R
#define OC3_Remappable_Pin_Reference 15

#define OC4_RP RPOR1bits.RP3R
#define OC4_Remappable_Pin_Reference 16

#define OC5_RP RPOR4bits.RP9R
#define OC5_Remappable_Pin_Reference 17

#define OC6_RP RPOR5bits.RP10R
#define OC6_Remappable_Pin_Reference 18

//Functions to generate a PWM signal using the OC1 module and the RP0 pin
void PWM_OC1_Initialize(PWM_Module* OC1_PWM_module);
//returns OCR / OCRS for the motor's PWM module, representing the duty cycle %
double PWM_Get_OC1_DutyCycle(void);
//calculates the frequency using OCRS and Tcy with the equation in Example 15-1
double PWM_Get_OC1_Frequency(void);
//sets the proper OCR register based on the dutyCyclePercentage variable
//within the PWM_Module struct
void PWM_Update_OC1_DutyCycle(const PWM_Module* OC1_PWM_module);
//alters the proper OCR and OCRS registers based on the frequency variable
//within the PWM_Module struct (in Hz)
void PWM_Update_OC1_Frequency(const PWM_Module* OC1_PWM_module);


//Functions to generate a PWM signal using the OC2 module and the RP1 pin
void PWM_OC2_Initialize(PWM_Module* OC2_PWM_module);
double PWM_Get_OC2_DutyCycle(void);
double PWM_Get_OC2_Frequency(void);
void PWM_Update_OC2_DutyCycle(const PWM_Module* OC2_PWM_module);
void PWM_Update_OC2_Frequency(const PWM_Module* OC2_PWM_module);

//Functions to generate a PWM signal using the OC3 module and the RP2 pin
void PWM_OC3_Initialize(PWM_Module* OC3_PWM_module);
double PWM_Get_OC3_DutyCycle(void);
double PWM_Get_OC3_Frequency(void);
void PWM_Update_OC3_DutyCycle(const PWM_Module* OC3_PWM_module);
void PWM_Update_OC3_Frequency(const PWM_Module* OC3_PWM_module);

//Functions to generate a PWM signal using the OC4 module and the RP3 pin
void PWM_OC4_Initialize(PWM_Module* OC4_PWM_module);
double PWM_Get_OC4_DutyCycle(void);
double PWM_Get_OC4_Frequency(void);
void PWM_Update_OC4_DutyCycle(const PWM_Module* OC4_PWM_module);
void PWM_Update_OC4_Frequency(const PWM_Module* OC4_PWM_module);

//Functions to generate a PWM signal using the OC5 module and the RP9 pin
void PWM_OC5_Initialize(PWM_Module* OC5_PWM_module);
double PWM_Get_OC5_DutyCycle(void);
double PWM_Get_OC5_Frequency(void);
void PWM_Update_OC5_DutyCycle(const PWM_Module* OC5_PWM_module);
void PWM_Update_OC5_Frequency(const PWM_Module* OC5_PWM_module);

//Functions to generate a PWM signal using the OC6 module and the RP10 pin
void PWM_OC4_Initialize(PWM_Module* OC6_PWM_module);
double PWM_Get_OC6_DutyCycle(void);
double PWM_Get_OC6_Frequency(void);
void PWM_Update_OC6_DutyCycle(const PWM_Module* OC6_PWM_module);
void PWM_Update_OC6_Frequency(const PWM_Module* OC6_PWM_module);