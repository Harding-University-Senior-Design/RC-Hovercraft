# Remote Controlled Hovercraft Senior Design Project (Harding University)
## Summary
This is a collection of PIC code (primarily in C), Multisim schematics, and Protomat designs for the RC Hovercraft Senior Design Project

## Example Folders
- PWM Generation
- Using Input Capture to Analyze an Input PWM Signal
- Using Variables within the PIC to Alter an Output PWM Signal

## Within the Main Project Folder (Work in Progress)
- Input Parsing Subsystem (a framework to utilize input capture)
  * Measuring the time between an input PWM's rising and falling edges to calculate its duty cycle
  * Storing this duty cycle value (as a double) to be used by other functions for thrust and steering calculations
- Output Signal Generation Subsystem (a framework to utilize Output Compare PWM generation)
  * PWM.h
    * The header file for the main struct used to manipulate the motor PWMs and all supporting functions
    * The struct default initialization is a 20kHz, 0% duty cycle PWM, the duty cycle and frequency of which can then be managed by the programmer
  * PWM.c
    * The implementation of all supporting functions for the struct representing the motor's PWM modules
  * output_signal_generation_driver.c
    * Example uses of the Output Signal Generation framework, including how to properly abstract away the need for direct management of internal PIC registers related to PWM generation
    * Shows how PWMs can be generated using the framework to control a two-motor configuration (for the hovercraft's propulsion system)
- Kill Switch Subsystem (a framework to manage the hovercraft's kill switch)
  * Automated shutdown of all PWMs for the Propulsion System
  * Turns off the throttle of the Lift System's engine
  * Functionality to turn on the Lift System engine's throttle and resume generation of PWMs when kill switch is no longer engaged
