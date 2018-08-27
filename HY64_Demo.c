/**********************************************************
*	HY64_Demo.c
*
*	Demonstration functions for DEMOHY64 board
*
**********************************************************/

#include <mc9s12xhy256.h>			// derivative information
#include "hy64_demo.h"				// demo functions header file
#include "lcddrv.h"					// LCD driver functions
#include "cpu.h"						// CPU fuctions


/**********************************************************
*
*	Trans_Demo
*	Run Transmission Gear Select and Compass Demo
*
*	control of demo passed thru global variable kbi_state 
*	modified in kbi_isr interrupt routine
**********************************************************/
void Trans_Demo(void){
	
	byte n;
	
	while(kbi_state & TRANSDEMO){
		for(n=0; n<8; n++){
			if(kbi_state & TRANSDEMO){
				Delay(250);				// wait 500ms
			   Set_Gear(ON,n);      // roll thru gear selector
				Set_Compass(ON,n);	// roll thru compass points
				LED1 ^= 1;			// toggle LED 
			} else {
				Set_Gear_Labels_All_Off();
				Set_Compass_All_Off();
				LED1 = 1;	
				break;
			}
		}
	}
}

/**********************************************************
*
*	Temp_Demo
*	Run Temperature Status, Volt Bar, & Temp Bar Demo
*
*	control of demo passed thru global variable kbi_state 
*	modified in kbi_isr interrupt routine
**********************************************************/
void Temp_Demo(void){
	word n;
	byte m, p;
			
	while(kbi_state & TEMPDEMO){
		for(n=0;n<999; n++){
			if(kbi_state & TEMPDEMO){
			
				m = ((n/4) & 0x03);
				p = ((n/4) & 0x01);
				Set_Temp_Digits(n);

				switch((n/6) & 0x01){			// select Temp scale indicator to turn on
					case 0:	DEG_F = ON;
								DEG_C = OFF;
								TEMP_DEC = ON;	
								break;
					case 1:	DEG_F = OFF;
								DEG_C = ON;	
								TEMP_DEC = OFF;	
								break;
				}

				switch(m){							// select Temp indicator label to turn on
					case 0:	INSIDE = ON;
								TEMP = OFF;
								break;
					case 1:	OUTSIDE = ON;
								INSIDE = OFF;
								break;
					case 2:	ENGINE = ON;
								OUTSIDE = OFF;
								break;
					case 3:	TEMP = ON;
								ENGINE = OFF;
								break;
				}

/*				q = ((n/4) & 0x07);
				switch(q) {							// set selection var for Volt & Temp bar
					case 0:	r = 0;
								break;
					case 1:	r = 1;
								break;
					case 2:	r = 2;
								break;
					case 3:	r = 3;
								break;
					case 4:	r = 4;
								break;
					case 5:	r = 5;
								break;
					case 6:	r = 6;
								break;
					case 7:	r = 0;
	 							break;
				}
				Set_Temp_Bar(ON, r);				// Turn  Voltage & Temp bars ON & OFF
				Set_Volt_Bar(ON, 6-r);	
*/
				LED2 ^= 1;							// toggle LED2
				Delay(100);
			} else {
				Set_Temp_Labels_All_Off();		// turn all temp digits & indicators off	
				LED2 = 1;	
				break;
			}
		}
	}
}


/**********************************************************
*
*	Odo_Demo	
*	Run Odometer / Trip LCD Segments Demo
*
*	control of demo passed thru global variable kbi_state 
*	modified in kbi_isr interrupt routine
**********************************************************/
void Odo_Demo(void){			// display fuel & clock segments
	word temp_ctr = 0;
	
	while(kbi_state & ODODEMO){
		for(temp_ctr=0; temp_ctr<16; temp_ctr++){
			if(kbi_state & ODODEMO){
				LED3 ^= 1;								// toggle LED3
				Delay(250);								// Delay to enhance display readability
				
				Set_FC_Labels_All_Off();			// ODO indicators off

				switch((temp_ctr/2) & 0x03){
					case 0: SEG_P40 = ON; break;	// ODO indicator on
					case 1: SEG_P41 = ON; break;	// TRIPA indicator on
					case 2: SEG_P42 = ON; break;	// TRIPB indicator on
					case 3: SEG_R4  = ON; break;	// ODO decimal on	
				}
				
				switch ((temp_ctr/4) & 0x01){
					case 0: SEG_P34 = ON; break;	// MILES indicator on
					case 1: SEG_P35 = ON; break;	// KM indicator on
				}

				if((temp_ctr>0) && (temp_ctr<10)) Set_Digit_13(temp_ctr);
					else Set_Digit_13(0);
				if((temp_ctr>1) && (temp_ctr<11)) Set_Digit_12(temp_ctr-1);
					else Set_Digit_12(0);
				if((temp_ctr>2) && (temp_ctr<12)) Set_Digit_11(temp_ctr-2);
					else Set_Digit_11(0);
				if((temp_ctr>3) &&(temp_ctr<13)) Set_Digit_10(temp_ctr-3);
					else Set_Digit_10(0);
				if((temp_ctr>4) && (temp_ctr<14)) Set_Digit_9(temp_ctr-4);
					else Set_Digit_9(0);
				if((temp_ctr>5) && (temp_ctr<15)) Set_Digit_8(temp_ctr-5);
					else Set_Digit_8(0);
		
			} else {
				Set_Odo_Digits_Off();				// ODO digits all off
				Set_FC_Labels_All_Off();			// ODO indicators off
				LED3 = 1;	
				break;
			}
		}
	}
}


/**********************************************************
*
*	Fuel_Clk_Demo
*	Run Fuel/Clock/Status LCD Segments Demo
*
*	control of demo passed thru global variable kbi_state 
*	modified in kbi_isr interrupt routine
**********************************************************/
void Fuel_Clk_Demo(void){			// display fuel / clock segments
//	word temp_ctr, local_state = 0;
	word n;
		
	if(kbi_state & FCDEMO){
	   Set_Cruise(ON);
	   Set_Traction(ON);
		} 
		
	while(kbi_state & FCDEMO){
		for(n=0; n<0x270F; n++){
			if(kbi_state & FCDEMO){
				Set_Fuel_Clock_Digits(n);
				Set_Fuel_Clock_Labels((n/6) & 0x0F);
				Set_Status_Labels((n/12) & 0x03); 
				LED4 ^= 1;							// toggle LED4
				Delay(100);
			} else {									// turn Fuel/Clock/Status labels off
				Set_Fuel_Clock_Digits_Off();	
			   Set_Cruise(OFF);
			   Set_Traction(OFF);
				Set_Cruise_State(OFF);
				Set_Traction_State(OFF);
				Set_TPMS_State(OFF);
				LED4 = 1;	
				break;
			}
		}
	}
}
