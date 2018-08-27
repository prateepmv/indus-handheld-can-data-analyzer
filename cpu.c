#include <hidef.h>					// common defines and macros
#include <mc9s12xhy256.h>			// derivative information
#include "cpu.h"						// CPU routines
#include "hy64_demo.h"				// demo board routines
#include "lcddrv.h"					// LCD drivers routines
#include "sci_hc12.h"				// sci header file



/**********************************************************
*	CPMU_PBE
*
*	Setup clock module for PLL Bypass, External.  For DEMOHY64
*	using 8MH XTAL, 
*	Core Clk - 8MHz, Bus Clk - 4MHz
*********************************************************
void CPMU_PBE(void){
	CPMUOSC_OSCE = 1;				// enable ext OSC
	while(!CPMUFLG_UPOSC);		// wait for ext osc to stabilize
	CLKSEL_PLLSEL = 0;			// select ext oscillator
}*/

/**********************************************************
*	CPMU_PEE_8MHz
*
*	Setup clock module for PLL Engaged using external XTAL
*	Input - 8MHz XTAL 
*	VCOCLK - 64MHz, Core Clk - 16MHz, Bus Clk - 8MHz
*	Fpll = 2 * Fosc * (SYNDIV + 1) / ((REFDV + 1) * (2 * POSTDIV))
**********************************************************/
void CPMU_PEE_8MHz(void){
	SYNR_VCOFRQ = 0x01;			// fvco set to 48 - 64MHz output
	REFDV_REFFRQ = 0x02;			// pll reference set to 6 - 12 MHz (Fref = 8MHz)
	REFDV_REFDIV = 0x00;			// pll input is ext osc
	SYNR_SYNDIV = 0x03;			// pll feedback = vcoclk/8
	POSTDIV_POSTDIV = 0x02;		// core clk = fvco/4

//	CPMUOSC_OSCE = 1;				// enable ext OSC
	while(!CRGFLG_LOCK);			// wait for ext osc to stabilize and pll to lock
	CLKSEL_PLLSEL = 1;			// select PLL output clock
	CRGFLG = 0xFF;					// clear CMPMU int flags - not needed but good practice
}


/**********************************************************
*	Tim0_Dly
*
*	starts TIM0_C7 and waits for C7 flag to set, requires
*	user config TIM0 before calling
**********************************************************/
void Tim0_Dly(void){
	while(!TIM0_TFLG1_C7F); 
	TIM0_TFLG1_C7F = 1;
}


/**********************************************************
*	Init_Timer1
*
*	initialize TIM1 as free-running timer counting at bus
*	clock rate, configures T1_C7 as output compare
**********************************************************/
void Init_Timer1(void){
	TIM1_TIOS = 0x80;				// setup T1C7 as output compare
	TIM1_TCTL1 &= 0x3C;			// pin is GPIO
	TIM1_TSCR2_PR = 0x0;			// timer runs at bus clk rate
	TIM1_TSCR1_TEN = 1;			// enable timer1 as free-running
}

/**********************************************************
*	Delay_1ms
*
*	Uses Timer1, Ch7 to produce a 1ms Delay.  
* 	Get current TIM1 cnt and adds delay offset then waits 
*	for compare.  OVF cleared on exit from routine  
**********************************************************/
void Delay_1ms(void){
	dword t_count;
	
	t_count = TIM1_TCNT;			// get TIM1 count
	t_count += ms_Delay_Cnt;	// add offset count
	TIM1_TC7 = (word)t_count;			// load new mod value
	while(!TIM1_TFLG1_C7F);		// wait for timer flag to set
	while(TIM1_TFLG1_C7F){		// while timer flag is set
		TIM1_TFLG1_C7F = 1;			// clear flag
	}
}

/**********************************************************
*	Delay
*
*	add program Delay defined by user input.  Calls Delay_1ms()
*
*	input		- Delay in ms
**********************************************************/
void Delay(word count){
	word m;

	for(m=0; m<count; m++){
		Delay_1ms();
	}
}

// setup mcu port initial states
void Configure_Ports(void){

  //***** CORE PORTS ******* 

  // core interrupts  
  // This register register was renamed. On early D, A and H families it is called INTCR,
  //   on all other families it is called IRQCR.  
  
  	IRQCR = 0;		// IRQ only responds to falling edge   
  								//	- note IRQE is write once  								

  //include this line to clear the X-bit in order to enable the XIRQ function     
  //  asm ANDCC #~BIT6;	

//  	RDRIV = ALL_REDUCED_DRIVE;	// drive level 					// core ports reduced drive  
  	PUCR 	= ALL_PULLS_ON; 		// Enable Pull devices 	// Note: only pull-ups available on core ports  

  // configure port data and direction  
  // PORT A  
  	PORTA  = ALL_LOW;	   
  	DDRA   = ALL_INPUTS;
  
  // PORT B  
  	PORTB  = ALL_HIGH;	   					 		
  //****** PIM PORTS ******* 

    // PORT R
  	PPSR 	= ALL_PULLED_UP;			// Configure Pull-polarity  
  	PERR 	= ALL_PULLS_ON;         // Enable Pull devices  
//	RDRR 	= ALL_REDUCED_DRIVE;    // drive level  
  	PTR 	= 0x0F;						// PTR[7:4] low, PTR[3:0] high  
  	DDRR 	= 0x0F;						// PTR[3:0] output

    // PORT T  
  	PPST 	= ALL_PULLED_DOWN;      // Configure Pull-polarity  
  	PERT 	= ALL_PULLS_ON;         // Enable Pull devices
//  	RDRT 	= ALL_REDUCED_DRIVE;    // drive level  
  	PTT  	= ALL_LOW;              // port data  
  	DDRT 	= ALL_INPUTS;           // port direction 

    // PORT S  
  	PPSS 	= ALL_PULLED_DOWN;      // Configure Pull-polarity  
  	PERS 	= ALL_PULLS_ON;	        // Enable Pull devices  
    //	PERS 	= BIT7|BIT6|BIT5|BIT4;					// S12 EVB - no pull devices on port S0-3 due to avoid   
  																	// conflict with connections to RS232 transceiver  
//  	RDRS 	= ALL_REDUCED_DRIVE;    // drive level  
   WOMS 	= 0x00;                 // Configure wired-or outputs  
  	PTS 	= ALL_LOW;	            // port data  

//  	DDRS 	= ALL_INPUTS;           // port direction  
  	DDRS 	= DDRS_DDRS3_MASK | DDRS_DDRS1_MASK;	// PTS[3,1] output

/*
    // PORT P  - Also LCD port!
  	PPSP 	= ALL_PULLED_UP;        // Configure Pull-polarity  
  	PERP 	= ALL_PULLS_ON;         // Enable Pull devices  
  	RDRP 	= ALL_REDUCED_DRIVE;    // drive level  
  	PTP 	= ALL_LOW;		          // port data  
  	DDRP 	= ALL_INPUTS;           // port direction  
*/  	
// These not recognized in CW header files  	
//  	PIFP 	= 0xFF;  // Clear all interrupt flags  
//  	PIEP 	= 0x00;  // Enable interrupts  

/*  
    // PORT H  - also LCD port!
  	PPSH 	= ALL_PULLED_DOWN;      // Configure Pull-polarity  
  	PERH 	= ALL_PULLS_ON;         // Enable Pull devices  
   	RDRH 	= ALL_REDUCED_DRIVE;    // drive level  
  	PTH   = ALL_LOW;	            // port data  
*/  	
  	// PORT U  
  	PPSU 	= ALL_PULLED_DOWN;      // Configure Pull-polarity  
  	PERU 	= ALL_PULLS_ON;         // Enable Pull devices  
  	PTU  	= ALL_LOW;	            // port data  
//   DDRU 	= DDRU_DDRU7_MASK | DDRU_DDRU6_MASK ;	  // port direction // two debug outputs  
	DDRU = ALL_OUTPUTS;				// PTU[7:0] outputs
	  	
  	// PORT V  
  	PPSV 	= ALL_PULLED_DOWN;      // Configure Pull-polarity  
   PERV 	= ALL_PULLS_ON;         // Enable Pull devices  
  	PTV   = ALL_LOW;	            // port data  
//   DDRV 	= DDRV_DDRV7_MASK | DDRV_DDRV6_MASK;	 // port direction   // two debug outputs  
	DDRV = ALL_OUTPUTS;				// PTV[7:0] outputs
  	


/**********************************************************
*	PORT AD SETUP
*
*	configure port as input w/ pull-ups enabled,  
*	key-wakeup enabled on PT1AD[7:4] for PBSW use
*  NOTE: to use the digital input function the ATDDIEN bit must be set  
*
*	NOTE: ATD port pull-ups are active for both digital and 
*	analog inputs !  
***********************************************************/
	ATDDIEN	= 0xF0;					// enable PT1AD[7:4] as digital input

	PER1AD 	= ALL_PULLS_ON;		// Port AD Pull Enable  
//	RDR1AD 	= ALL_REDUCED_DRIVE;	// Port AD Reduced Drive Register
	PT1AD 	= ALL_HIGH;				// Port AD I/O Register
	DDR1AD 	= ALL_INPUTS;			// Port AD Data Direction Register
	
	PIF1AD	= 0xF0;					// clear spurious interrupts
	PIE1AD	= 0xF0;					// enable Port AD interrupts on PT1AD[7:4]


//**** ECKL options **** 
   
  // ECLK available on PortH.2, 
  //  - note, keeping port H configured for reduced drive 
  //		 will minimise ECLK generated noise  							
  #ifdef ECLK_OUT
  // ECLKDIV = 1 to 32; DEFINED IN TARGET.H 
  // select one:  
     ECLKCTL = NECLK;                       // ECLKx2 on. ECLK              off  
  //   ECLKCTL = (ECLKDIV-1);               // ECLKx2 on.  ECLK/(ECLKDIV)     on  
  //   ECLKCTL = (ECLKDIV-1)|DIV16;         // ECLKx2 on.  ECLK/(ECLKDIV*16)  on  
  #endif
  }


// init_atd - setup CH0 for continuous conversion
void init_atd_ch0(void){

	ATDCTL1 = 0x00;				// 8-bit results, select AN0
	ATDCTL3 = 0xA0;				//	right justify results, 4 conversions per sequence
	ATDCTL4 = 0x00;				// fATDCLK = 2.67MHz, 4 clks per sample
	ATDCTL5 = 0x20;				// continuous conversion
	ATDSTAT0_SCF = 0x01;			// clear conversion complete flag
}


// Read ATD and return value read
byte read_atd_ch0(void){
	byte val;

	val = ATDDR0L;					// get ADT result
	return (val);					// return
}


// Read POT and display output on LCD TEMP / VOLT bars
void read_pot(void){
	byte val, st;

	val = read_atd_ch0();				// get POT value from ATD

	if(val < 0x24)
		st = 0;
	else if((val < 0x48) && (val > 0x25))
		st = 1;
	else if((val < 0x6C) && (val > 0x48))
		st = 2;
	else if((val < 0x90) && (val > 0x6C))
		st = 3;
	else if((val < 0xB4) && (val > 0x91))
		st = 4;
	else if((val < 0xD8) && (val > 0xB5))
		st = 5;
	else if(val > 0xD9)
		st = 6;

	Set_Temp_Bar(ON, st);				// Turn  Voltage & Temp bars ON & OFF
	Set_Volt_Bar(ON, st);	
/*	switch(st){
		case (0):
			break;
		case (1):
			LEDS_OFF;				// LEDs OFF
			LED1 = LON;				// LED1  ON
			break;
		case (2):
			LEDS_OFF;				// LEDs OFF
			LED2 = LON;				// LED2  ON
			break;
		case (3):
			LEDS_OFF;				// LEDs OFF
			LED3 = LON;				// LED3  ON
			break;
		case (4):
			LEDS_OFF;				// LEDs OFF
			LED4 = LON;				// LED4  ON
			break;
		case (5):
			LEDS_ON;					// LEDs ON
			break;
	}*/
}


#pragma CODE_SEG NON_BANKED

/**********************************************************
*	sci_isr
*
*	interrupt handler for SCI module interrupts
**********************************************************/
void interrupt 20 sci_isr(void){
	DisableInterrupts;
	echo_com();						// echo keybd data
	EnableInterrupts;
}


/**********************************************************
*	kbi_isr
*
*	interrupt handler for PBSW interrupt
**********************************************************/
void interrupt 24 kbi_isr(void){
	byte int_flag = 0;
	byte st = 0, n = 0;
	
//	DisableInterrupts;
	int_flag = (PIF1AD >> 4);		// capture interrupt status and shift to low nibble
	PIF1AD = 0xF0;						// clear PBSW interrupts

/*	while(st<0x08){
		if(SW1 && SW2 && SW3 && SW4){ 
			st ++;						// incr count if all PBSW's high
		} else st = 0;
	}

	for(n=0; n<0x1F; n++){
		st = 0;
		while(!st){							// sw debounce, loop until all PBSW's released
			if(SW1 && SW2 && SW3 && SW4){ 
				st = 1;	
		}
	}*/
	Delay(50);							// delay to debounce switch
	EnableInterrupts;
	
	if(int_flag & 0x01) st = 1;	// SW1 pressed
	if(int_flag & 0x02) st = 2;	// SW2 pressed 
	if(int_flag & 0x04) st = 3;	// SW3 pressed
	if(int_flag & 0x08) st = 4;	// SW4 pressed 

	switch(st){
		case 1: LED1 ^= 1; break;
		case 2: LED2 ^= 1; break;
		case 3: LED3 ^= 1; break;
		case 4: LED4 ^= 1; break;
	}

/*	switch(st){
		case 1:	kbi_state = 0;					// clear state bits
					kbi_state ^= TRANSDEMO;		// set state bit for transmission demo
					Trans_Demo();					// run Trans demo
					break;

		case 2:	kbi_state = 0;					// clear state bits
					kbi_state ^= TEMPDEMO;		// set state bit for temp demo
					Temp_Demo();					// run TEMP demo
					break;

		case 3:	kbi_state = 0;					// clear state bits
					kbi_state ^= ODODEMO;		// set state bit for odometer demo
					Odo_Demo();						// run ODO demo
					break;	

		case 4:	kbi_state = 0;					// clear state bits
					kbi_state ^= FCDEMO;			// set state bit for Fuel/Clk demo
					Fuel_Clk_Demo();				// run Fuel / Clock / Status demo
					break;
	}
*/
}


#pragma CODE_SEG DEFAULT


