#include <hidef.h>					// common defines and macros
#include <mc9s12xhy256.h>			// derivative information
#include "lcddrv.h"					// lcd driver functions
#include "cpu.h"						// cpu setup functions
#include "hy64_demo.h"				// demo board functions
#include "LCD_digit_display_defines.h"
#include "sci_hc12.h"				// SCI functions


byte kbi_state = 0;					// global var


void main(void) {

	byte n;
	byte bd_str[] = "\nDEMO9S12XHY256, AXM-0509, SCI Test \n";

	EnableInterrupts;
	
	CPMU_PEE_8MHz();					// configure clock module
	Configure_Ports();				// setup IO ports
	Init_Timer1();						// init TIM1 as free-running
	init_atd_ch0();					// init ATD
	LCD_Init();							// init LCD module
	
	ensci();								// init SCI

//	cputs(bd_str);						// send string to SCI port
	for(n=0; n<0x30; n++){
		cputs(bd_str);						// send string to SCI port
	}

	Set_Logo(ON, OFF, OFF, OFF, OFF);	// turn on logo icons
	Display_On();						// turn on all LCD elements
	
	for(;;) {
		read_pot();					// get POT value from ATD
//		PTS_PTS3 ^= 1;				// toggles PTS3/TXCAN0 output pin 
//		PTR_PTR1 ^= 1;				// toggles PTR1/TXCAN1 output pin 
//		PTU ^= 0xFF;				// toggle PTU output pins
//		PTV ^= 0xFF;				// toggle PTU output pins
	}
}