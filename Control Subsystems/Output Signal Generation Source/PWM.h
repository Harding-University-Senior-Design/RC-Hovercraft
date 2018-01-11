/* 
 * File:                PWM.h
 * Author:              Zachary Downum
 * Comments:            This is the header file to handle all PWM generation
 *                      This will handle two PWMs specifically:  the left and
 *                      right propulsion motors.  No other PWMs need to be
 *                      generated at this time.
 * 
 *                      The left motor PWM will be generated using the OC1
 *                      module and the right motor PWM will be generated using
 *                      the OC2 module.  In order to avoid confusion, all
 *                      variables will be abstracted to include "LeftMotor"
 *                      or "RightMotor" if it is unclear which it is referring
 *                      to.  The left motor PWM will be sent to RP0, and the
 *                      right motor PWM will be sent to RP1
 * 
 * 
 * Revision history: 
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
    double duty_cycle_percentage;
    //in Hertz
    int frequency;
    
    void (*Initialize)(struct PWM_Module*);
    
    double (*GetCurrentDutyCycle)();
    void (*UpdateDutyCycle)(const struct PWM_Module*);
    
    double (*GetCurrentFrequency)();
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
#define Left_Motor_OC_CON1 OC1CON1
#define Left_Motor_OC_CON1bits OC1CON1bits
#define Left_Motor_OC_CON2 OC1CON2
#define Left_Motor_OC_CON2bits OC1CON2bits
#define Left_Motor_RP RPOR0
#define Left_Motor_Remappable_Pin_Reference 13
#define Left_Motor_OCR OC1R
#define Left_Motor_OCRS OC1RS

#define Right_Motor_OC_CON1 OC2CON1
#define Right_Motor_OC_CON1bits OC2CON1bits
#define Right_Motor_OC_CON2 OC2CON2
#define Right_Motor_OC_CON2bits OC2CON2bits
#define Right_Motor_RP RPOR1
#define Right_Motor_Remappable_Pin_Reference 14
#define Right_Motor_OCR OC2R
#define Right_Motor_OCRS OC2RS

//Initializes the left motor's PWM signal using the OC1 module and the RP0 pin
void PWM_LeftMotorInitialize(PWM_Module* left_motor);
//Initializes the left motor's PWM signal using the OC2 module and the RP1 pin
void PWM_RightMotorInitialize(PWM_Module* right_motor);

//returns OCR / OCRS for the motor's PWM module, representing the duty cycle %
double PWM_GetLeftMotorDutyCycle(void);
double PWM_GetRightMotorDutyCycle(void);

//calculates the frequency using OCRS and Tcy with the equation in Example 15-1
double PWM_GetLeftMotorFrequency(void);
double PWM_GetRightMotorFrequency(void);

//sets the proper OCR register based on the duty_cycle_percentage variable
//within the PWM_Module struct
void PWM_UpdateLeftMotorDutyCycle(const PWM_Module* left_motor);
void PWM_UpdateRightMotorDutyCycle(const PWM_Module* right_motor);

//alters the proper OCR and OCRS registers based on the frequency variable
//within the PWM_Module struct (in Hz)
void PWM_UpdateLeftMotorFrequency(const PWM_Module* left_motor);
void PWM_UpdateRightMotorFrequency(const PWM_Module* right_motor);