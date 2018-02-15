/* 
 * File:                PWM.h
 * Author:              Zachary Downum
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#pragma once

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