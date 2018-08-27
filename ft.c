#include <hidef.h>				// common defines and macros
#include <mc9s12hy64.h>			// derivative information
#include "sci_hc12.h"			// SCI routines
#include "ft.h"					// header file
#include "cpu.h"					// MCU setup functions


void func_test(void) {

//	byte val, st;
//	byte ledx[] = {0x0E, 0x0D, 0x0B, 0x07, 0x0B, 0x0D};
	byte bd_str[] = "\nDEMO9S12HY64, AXM-0480 \n";
//	EnableInterrupts;				// enable interrupts

										// BUSCLK default @ 8MHz
//	cpmu_pbe();						// sel ext osc and bypass PLL
//	CPMU_PEE_8MHz();					// PEE mode 8MHz bus clk

	mcu_init();						// setup target MCU
	init_atd_ch0();				// setup ATD for con
	ensci();							// init SCI

	cputs(bd_str);

	for(;;) {

		read_pot();					// get POT value from ATD
		PTS_PTS3 ^= 1;				// toggles PTS3/TXCAN output pin 
		PTU ^= 0xFF;				// toggle PTU output pins
		PTV ^= 0xFF;				// toggle PTU output pins
	}
}


// Setup MCU internals
void mcu_init(void){
	PTR = 0x0F;						// LED[4:0] off
	DDRR = 0x0F;					// output

	PTS_PTS3 = 0x0;				// init PTS3
	DDRS_DDRS3 = 0x01;			// PTS3 is output

	PTU = 0x66;						// init PTU
	PTV = 0x66;						// init PTV
	DDRU = 0xFF;					// PTU output
	DDRV = 0xFF;					// PTV output

	PER1AD = 0xF0;					// enable pull-ups on PAD[7:0]

	ATDDIENL = 0xF0;				// enable KeyWake Up on PAD[7:4]
	PIF1AD = 0xF0;					// clear pending interrupt flags
	PIE1AD = 0xF0;					// enable int on PTAD[7:0]
}


// init timer
void init_timer0(void){
	PTTRR_PTTRR5 = 1;				// IOC[7] routed to PTR1
	TIM0_TIOS = 0x80;				// setup IOC[7] as output compare
	TIM0_TCTL1 = 0x40;			// toggle IOC[7] output on compare
	TIM0_TSCR2_PR = 0x3;			// timer runs at bus clk div 8
	TIM0_TC7 = 0xFFFF;			// load IOC7 timer count - 261 ms

	TIM0_TSCR1_TEN = 1;			// enable timer0

}


// Start TPM and wait for timer overflow
void tim0_dly(void){
	while(!TIM0_TFLG1_C7F);
	TIM0_TFLG1_C7F = 1;
}


// init_atd - setup CH0 for continuous conversion
void init_atd_ch0(void){

	ATDCTL1 = 0x00;				// 8-bit results, select AN0
	ATDCTL3 = 0xA0;				//	right justify results, 4 conversions per sequence
	ATDCTL4 = 0x00;				// fATDCLK = 2.67MHz, 4 clks per sample
	ATDCTL5 = 0x20;				// continuous conversion
	ATDSTAT0_SCF = 0x01;			// clear conversion complete flag
}


// Read ATD
byte read_atd_ch0(void){
	byte val;

	val = ATDDR0L;					// get ADT result
	return (val);					// return
}


// Read POT
void read_pot(void){
	byte val, st;

	val = read_atd_ch0();				// get POT value from ATD

	if(val < 0x2A)
		st = 0;
	else if((val < 0x54) && (val >0x2A))
		st = 1;
	else if((val < 0x7E) && (val >0x54))
		st = 2;
	else if((val < 0xA8) && (val >0x7E))
		st = 3;
	else if((val < 0xD2) && (val >0xA8))
		st = 4;
	else if(val >0xD2)
		st = 5;

	Set_Temp_Bar(ON, st);				// Turn  Voltage & Temp bars ON & OFF
	Set_Volt_Bar(ON, 6 - st);	
}

/*
// SCI ISR - echoes keyboard keystrokes
void interrupt 20 sci_isr(void){
	DisableInterrupts;
	echo_com();						// echo keybd data
	EnableInterrupts;
}


// Key Wakeup ISR
void interrupt 24 kbi_isr(void){
 	byte state;
	DisableInterrupts;

	state = (PIF1AD >> 4);		// capture interrupt status and shift to low nibble
	switch(state){
		case 1:	while(!SW1);	// wait for SW to rise
					LED1 ^= 1;		// toggle LED1
					break;
		case 2:	while(!SW2);	// wait for SW to rise
					LED2 ^= 1;		// toggle LED2
					break;
		case 4:	while(!SW3);	// wait for SW to rise
					LED3 ^= 1;		// toggle LED3
					break;
		case 8:	while(!SW4);	// wait for SW to rise
					LED4 ^= 1;		// toggle LED4
					break;
		default: break;
	}
	PIF1AD = 0xF0;					// clear interrupt flags
	EnableInterrupts;
} */
