/*
 * File:   kill_switch_driver.c
 * Author: Zachary Downum
 */

#include "mcc_generated_files/mcc.h"

//FCY is based off _XTAL_FREQ, the current system clock
//(see system_configuration.h)
#define FCY (_XTAL_FREQ / 2)

#include <stdlib.h>
#include <libpic30.h>
#include <xc.h>

#include "PWM.h"
#include "InputCapture.h"

//these were experimentally derived, so these may not be the optimal values
#define SWITCH_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)5.563
#define SWITCH_MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)12.453
#define SWITCH_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE (double)((SWITCH_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE + SWITCH_MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE) / 2)

//these were experimentally derived, so these may not be the optimal values
#define THROTTLE_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)5.563
#define THROTTLE_MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE (double)12.453
#define THROTTLE_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE (double)((THROTTLE_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE + THROTTLE_MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE) / 2)

#define STEERING_MIN_INPUT_SIGNAL_DUTY_CYCLE (double)7.462
#define STEERING_MAX_INPUT_SIGNAL_DUTY_CYCLE (double)13.813
#define STEERING_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE (double)((STEERING_MIN_INPUT_SIGNAL_DUTY_CYCLE + STEERING_MAX_INPUT_SIGNAL_DUTY_CYCLE) / 2)

//Setting the INCREMENT_ADJUSTMENT_FACTOR to 100 achieves an output duty cycle that goes from 0% to 100%
//make the INCREMENT_ADJUSTMENT_FACTOR smaller to make the maximum output duty cycle % smaller
//make the INCREMENT_ADJUSTMENT_FACTOR larger to make the maximum output duty cycle % larger (not recommended as 100% should be the absolute max)
#define THROTTLE_INCREMENT_ADJUSTMENT_FACTOR 10
#define PROPULSION_THROTTLE_CYCLE_INCREMENT ((double)(THROTTLE_MAXIMUM_INPUT_SIGNAL_DUTY_CYCLE - THROTTLE_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE) / THROTTLE_INCREMENT_ADJUSTMENT_FACTOR)
#define PROPULSION_THROTTLE_SERVO_OFFSET 1.8

#define STEERING_INCREMENT_ADJUSTMENT_FACTOR 100
#define PROPULSION_STEERING_CYCLE_INCREMENT ((double)(STEERING_MAX_INPUT_SIGNAL_DUTY_CYCLE - STEERING_MIN_INPUT_SIGNAL_DUTY_CYCLE) / STEERING_INCREMENT_ADJUSTMENT_FACTOR)

//this dead zone will be used to prevent the user from turning the propulsion motor without intentionally
//moving the left joystick to the left or right.
#define DEAD_ZONE_OFFSET 10

#define ROUNDING_OFFSET 0.5

#define COUNTS_FOR_90_DEGREE_TURN 706
#define COUNTS_FOR_180_DEGREE_TURN 1412

//basic initialization for all pins
void PIC_Initialization(void)
{
    //changes all pins to digital
    ANSA = 0x0000;
    ANSB = 0x0000;
	Nop();
    
    //changes all pins to output (except for RPI4, which is an input only pin)
    TRISA = 0x0000;
    TRISB = 0x0000;
    Nop();
}

void IC_Module_Initialize(IC_Module* kill_switch_input, IC_Module* propulsion_throttle_servo_input, IC_Module* propulsion_direction_motor_input, IC_Module* propulsion_brake_input, Count_Monitor* stepper_motor_counter_input)
{
    kill_switch_input->Initialize = IC1_Initialize;
    kill_switch_input->Update = IC1_Update;
    
    propulsion_throttle_servo_input->Initialize = IC2_Initialize;
    propulsion_throttle_servo_input->Update = IC2_Update;
	
	propulsion_direction_motor_input->Initialize = IC3_Initialize;
	propulsion_direction_motor_input->Update = IC3_Update;
	
	propulsion_brake_input->Initialize = IC5_Initialize;
	propulsion_brake_input->Update = IC5_Update;
    
    stepper_motor_counter_input->Initialize = IC4_Initialize;
    stepper_motor_counter_input->Update = IC4_Update;
	
	
}

void PWM_Module_Initialize(PWM_Module* propulsion_thrust_servo_output, PWM_Module* duration_to_turn_propulsion_engine_output)
{
    propulsion_thrust_servo_output->Initialize = PWM_OC1_Initialize;
    propulsion_thrust_servo_output->GetDutyCycle = PWM_Get_OC1_DutyCycle;
    propulsion_thrust_servo_output->GetFrequency = PWM_Get_OC1_Frequency;
    propulsion_thrust_servo_output->UpdateDutyCycle = PWM_Update_OC1_DutyCycle;
    propulsion_thrust_servo_output->UpdateFrequency = PWM_Update_OC1_Frequency;
    
    duration_to_turn_propulsion_engine_output->Initialize = PWM_OC2_Initialize;
    duration_to_turn_propulsion_engine_output->GetDutyCycle = PWM_Get_OC2_DutyCycle;
    duration_to_turn_propulsion_engine_output->GetFrequency = PWM_Get_OC2_Frequency;
    duration_to_turn_propulsion_engine_output->UpdateDutyCycle = PWM_Update_OC2_DutyCycle;
    duration_to_turn_propulsion_engine_output->UpdateFrequency = PWM_Update_OC2_Frequency;
}

void Kill_Switch_Initialize(void)
{
	//A0 and A1 are used to enable/disable the relays to power on the lift and propulsion engines
	LATAbits.LATA0 = 0;
	LATAbits.LATA1 = 0;
}

int main(void)
{
	double averagedPropulsionThrottleDutyCycle = SWITCH_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE;
	double averagedPropulsionSteeringDutyCycle = STEERING_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE;
    double averagedKillSwitchDutyCycle = SWITCH_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE;
    double averagedBrakeSwitchDutyCycle = SWITCH_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE;
    double previousPositionOfPropulsionMotor = 0;
	
    SYSTEM_Initialize();
    PIC_Initialization();
	Kill_Switch_Initialize();
    
    IC_Module kill_switch_input;
    IC_Module propulsion_throttle_servo_input;
	IC_Module propulsion_direction_motor_input;
	IC_Module propulsion_brake_input;
    Count_Monitor stepper_motor_counter_input;
    IC_Module_Initialize(&kill_switch_input, &propulsion_throttle_servo_input, &propulsion_direction_motor_input, &propulsion_brake_input, &stepper_motor_counter_input);
    
    PWM_Module propulsion_throttle_servo_output;
    PWM_Module turn_propulsion_engine_output;
    PWM_Module_Initialize(&propulsion_throttle_servo_output, &turn_propulsion_engine_output);
	
    kill_switch_input.Initialize(&kill_switch_input);
    propulsion_throttle_servo_input.Initialize(&propulsion_throttle_servo_input);
    propulsion_direction_motor_input.Initialize(&propulsion_direction_motor_input);
	propulsion_brake_input.Initialize(&propulsion_brake_input);
    stepper_motor_counter_input.Initialize(&stepper_motor_counter_input);
    
    propulsion_throttle_servo_output.Initialize(&propulsion_throttle_servo_output);
    turn_propulsion_engine_output.Initialize(&turn_propulsion_engine_output);
    
    propulsion_throttle_servo_output.frequency = 50;
    propulsion_throttle_servo_output.UpdateFrequency(&propulsion_throttle_servo_output);
    
    turn_propulsion_engine_output.frequency = 400;
    turn_propulsion_engine_output.UpdateFrequency(&turn_propulsion_engine_output);
    turn_propulsion_engine_output.dutyCyclePercentage = 0;
    turn_propulsion_engine_output.UpdateDutyCycle(&turn_propulsion_engine_output);
    __delay_ms(1000);
    
    while(true)
    {
		kill_switch_input.Update(&kill_switch_input);
        averagedKillSwitchDutyCycle = (100 * averagedKillSwitchDutyCycle + kill_switch_input.dutyCyclePercentage) / 101;
		propulsion_direction_motor_input.Update(&propulsion_direction_motor_input);
        averagedPropulsionSteeringDutyCycle = (averagedPropulsionSteeringDutyCycle + propulsion_direction_motor_input.dutyCyclePercentage) / 2;
		propulsion_throttle_servo_input.Update(&propulsion_throttle_servo_input);
		averagedPropulsionThrottleDutyCycle = (averagedPropulsionThrottleDutyCycle + propulsion_throttle_servo_input.dutyCyclePercentage) / 2;
        
        //this is to regulate the duty cycle that is sent to the servo so that it falls within the acceptable range for
        //the servo that is being used by the project.
        //this duty cycle should be approximately between 5% and 15% (with 10% being directly in the center, or 90 degrees of motion in a 180 degree servo)
        propulsion_throttle_servo_output.dutyCyclePercentage = (averagedPropulsionThrottleDutyCycle - THROTTLE_MINIMUM_INPUT_SIGNAL_DUTY_CYCLE) / PROPULSION_THROTTLE_CYCLE_INCREMENT + PROPULSION_THROTTLE_SERVO_OFFSET;
        
        //This is here to account for minor variations that put the input duty cycle above or below
        //the minimum or maximum input signal duty (which could cause undefined behavior on the output signal)
        //if, for whatever reason, the duty cycle that is sent to the servo is below 0% or above 100%, this will
        //regulate the duty cycle to remain within acceptable values
        if (propulsion_throttle_servo_output.dutyCyclePercentage < 0)
        {
            propulsion_throttle_servo_output.dutyCyclePercentage = 0;
        }
        else if (propulsion_throttle_servo_output.dutyCyclePercentage > 100)
        {
            propulsion_throttle_servo_output.dutyCyclePercentage = 100;
        }
        
        propulsion_throttle_servo_output.UpdateDutyCycle(&propulsion_throttle_servo_output);
        
        //This is here to account for minor variations that put the input duty cycle above or below
        //the minimum or maximum input signal duty (which could cause undefined behavior on the output signal)
        //this is a binary interpretation of an input signal that could have multiple values, treating it like the switch it represents
        if (averagedKillSwitchDutyCycle < SWITCH_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE || (LATBbits.LATB4 == 1 && LATBbits.LATB5 == 1 && LATBbits.LATB6 == 1 && LATBbits.LATB8 == 1))
        {
            LATAbits.LATA0 = 1;
            LATAbits.LATA1 = 1;
        }
        else if (averagedKillSwitchDutyCycle >= SWITCH_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
            LATAbits.LATA0 = 0;
            LATAbits.LATA1 = 0;
        }
		
		propulsion_brake_input.Update(&propulsion_brake_input);
        averagedBrakeSwitchDutyCycle = (100 * averagedBrakeSwitchDutyCycle + propulsion_brake_input.dutyCyclePercentage) / 101;
		
        
		int discreteLocation = 0;
        if (averagedBrakeSwitchDutyCycle < SWITCH_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
			//represents a leftward turn of the propulsion engine
			//at the moment, the frequency of the PWM signal * the number of counts per trigger = 800 (counts per rotation) * rotations per second
			//this will allow for smooth movement of the stepper motor
			//if PWM frequency * counts per trigger > 800 * RPS, then the user input will experience a delay in controlling the stepper motor
			//if PWM frequency * counts per trigger < 800 * RPS, then the user input will cause jerking in the stepper motor response
			double preciseLocation = (averagedPropulsionSteeringDutyCycle - STEERING_MIN_INPUT_SIGNAL_DUTY_CYCLE) / PROPULSION_STEERING_CYCLE_INCREMENT;
            
            if (preciseLocation < 0)
            {
                preciseLocation = 0;
            }
            else if (preciseLocation > 100)
            {
                preciseLocation = 100;
            }
            
            if (preciseLocation < previousPositionOfPropulsionMotor + 2 && preciseLocation > previousPositionOfPropulsionMotor - 2)
            {
                preciseLocation = previousPositionOfPropulsionMotor;
            }
            else
            {
                previousPositionOfPropulsionMotor = preciseLocation;
            }
            
			//changes the values of preciseLocation from 0-100 to a -200 to 200 scale with a dead zone on either end of the controller
			preciseLocation = (preciseLocation - 50) * 16;
            

            discreteLocation = (int)(preciseLocation);
			
			if (discreteLocation >= -32 && discreteLocation <= 32)
			{
				discreteLocation = 0;
			}
			//these are made 201 so that when they are averaged out with the current numberOfCounts, it will eventually reach 200 and -200
			else if (discreteLocation > COUNTS_FOR_90_DEGREE_TURN + 1)
			{
				discreteLocation = COUNTS_FOR_90_DEGREE_TURN + 1;
			}
			else if (discreteLocation < -COUNTS_FOR_90_DEGREE_TURN - 1)
			{
				discreteLocation = -COUNTS_FOR_90_DEGREE_TURN - 1;
			}
        }
        else if (averagedBrakeSwitchDutyCycle >= SWITCH_MIDPOINT_INPUT_SIGNAL_DUTY_CYCLE)
        {
			if (stepper_motor_counter_input.numberOfCounts >= 0)
			{
				discreteLocation = COUNTS_FOR_180_DEGREE_TURN;
			}
			else if (stepper_motor_counter_input.numberOfCounts < 0)
			{
				discreteLocation = -COUNTS_FOR_180_DEGREE_TURN;
			}
        }
        
        int desiredLocation = ((double)discreteLocation + stepper_motor_counter_input.numberOfCounts) / 2;
        
        while (desiredLocation != stepper_motor_counter_input.numberOfCounts)
        {
            stepper_motor_counter_input.Update(&stepper_motor_counter_input);
            desiredLocation = ((double)discreteLocation + stepper_motor_counter_input.numberOfCounts) / 2;

            if (stepper_motor_counter_input.numberOfCounts > desiredLocation && stepper_motor_counter_input.allowClockwiseMotion == 1)
            {
                //This is the enable bit for the stepper motor responsible for turning the propulsion engine to control direction
                LATAbits.LATA2 = 0;

                turn_propulsion_engine_output.dutyCyclePercentage = 20;
                turn_propulsion_engine_output.UpdateDutyCycle(&turn_propulsion_engine_output);
            }
            //represents a rightward turn of the propulsion engine
            else if (stepper_motor_counter_input.numberOfCounts < desiredLocation && stepper_motor_counter_input.allowCounterClockwiseMotion == 1)
            {
                //This is the enable bit for the stepper motor responsible for turning the propulsion engine to control direction
                LATAbits.LATA2 = 1;

                turn_propulsion_engine_output.dutyCyclePercentage = 20;
                turn_propulsion_engine_output.UpdateDutyCycle(&turn_propulsion_engine_output);
            }
            //the motor holds its position
            else
            {
                turn_propulsion_engine_output.dutyCyclePercentage = 0;
                turn_propulsion_engine_output.UpdateDutyCycle(&turn_propulsion_engine_output);
            }
        }
    }
    
    return -1;
}