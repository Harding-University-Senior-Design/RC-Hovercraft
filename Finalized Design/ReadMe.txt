You will want to include all files in the Dependencies folder in the final project along with these driver files.
The reason the Dependencies folder is not included is to keep it in one place so that changes can be made easily.

This will accomplish the following:
	Kill Switch for both the Lift and Propulsion Engines
	Control the Servo that Manipulates the Propulsion Engine Throttle (to control the speed of the hovercraft)
	Control the Servo that Manipulates the Propulsion System's Rudder (to control the direction of the hovercraft)

	
All Dependencies and the Main Driver programs have been configured to have the internal oscillator run at Focs = 8MHz.
Running at frequencies other than the recommended 8MHz may have unknown effects on PWM generation or Input Capture functionality.