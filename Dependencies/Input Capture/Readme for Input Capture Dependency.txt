This dependency's main purpose is to create an abstraction layer between the specific registers used by the PIC to manage the IC modules that interpret input signals.  The dependency achieves this by allowing the user to call an update function to automatically calculate the duty cycle and frequency of the incoming signal.  This abstracts away the need for knowledge of how the internal register values are used to calculate the duty cycle and frequency of the incoming signal.

This dependency is ONLY intended for use with Microchip's PIC24FJ128GA202 microcontroller.  Use with any other microcontroller is not guaranteed to work--and may actually damage the component.

Up to 6 pins are assigned modules in this dependency.  Each IC module can be initialized independently, so you only have to use the number of modules you need.

RPI4 (Pin 