/******************************************************************************

	FILE				: sci.c
   
	AUTHOR			: Axiom Manufacturing    					     
						  Spencer Ruggles
						  
	PURPOSE			: RS-232 routines for HCS12
 
 *****************************************************************************/
 

#include <mc9s12xhy256.h>				// derivative information
#include "sci_hc12.h"					// sci header file

/*******************************************************************************
ensci()

	configures COM port for 19200,8,N,1 operation
	SCI baud rate register (SCIxBD) is written with value determined by 
	     SCIxBD = busclk/(16*baud) => SCIxBD = 0x0D -> 19.2 kHz
********************************************************************************/
void ensci(void){
	PT1AD_PT1AD2 = 1;						// RS-232 on/off control
	DDR1AD_DDR1AD2 = 1;					// pin is output	
	
	SCI0BD = BAUD19;						// 19.2 kHz
	SCI0CR1 &= ~SCI0CR1_M_MASK & ~SCI0CR1_PE_MASK;	// 8,N,1
	SCI0CR2 |= SCI0CR2_TE_MASK | SCI0CR2_RE_MASK;	// SCI Tx/Rx enabled
	SCI0CR2_RIE = 1;						// enable Rx Data Ready interrupt
}

/*******************************************************************************
ReadyToSend

	wait for COM port Tx Data Reg Empty flag to set indicating tx is 
	ready for a byte of data

	no TIMEOUT feature is implemented
********************************************************************************/
void ReadyToSend(void) {
	while((SCI0SR1 & 0x80) == 0);		// loop until TDRE flag is set
}

/*******************************************************************************
putchar
	check ReadyToSend then output byte of data on COM port
	
	no TIMEOUE feature implemented
********************************************************************************/
void putchar(byte sbyte) {
	ReadyToSend();							// wait for TDRE flag to set

	SCI0DRL = sbyte;						// send data byte on COM1
	while(!SCI0SR1_TC);					// wait for Tx Complete flag to set
}

/*******************************************************************************
putch

	send a single byte of data out COM port with CR/LF detection.
	
********************************************************************************/
void putch(byte ch){
	if(ch == '\n'){	 					// if \n send CR/LF
		putchar(CR);
		putchar(LF);
	} else {
		putchar(ch);						// send data byte
	}
}

/*******************************************************************************
cputs

	sends a string of data out COM port
	
	INPUT:		*sptr		pointer to data string
********************************************************************************/
void cputs(byte *sptr) {
	while(*sptr) {						// if more data
		putch(*sptr);					// send data to selected COM port w/ CR/LF
		++sptr;							// incr pointer
	}
}

/*******************************************************************************
SerialReady

	returns 0 if new serial data avaiable on selected COM port
	
	INPUT:		port		0 = SCI1, 1 = SCI2
	OUTPUT: 		status	0 = data available,  1 = no data avaiable
********************************************************************************/
byte SerialReady(void){

	if(SCI0SR1 & 0x20) return 0;	// return 0 if new data available
	else return 1;						// return 1 if no data available
}

/*******************************************************************************
ReadSerialPort

	reads a single byte from selected serial port
	
********************************************************************************/
byte ReadSerialPort(void){
	byte sbyte;							// temp var

	sbyte = SCI0DRL;					// get COM1 data
	return sbyte;						// return COM data 
}

/*******************************************************************************
GetSByte

	wait for Rx Data Ready Flag (RDRF) to set calls ReadSerialPort to get data
	from selected serial port
	
	INPUT:		port		0 = SCI1, 1 = SCI2
	OUTPUT: 		returns the data read from selected COM port 
********************************************************************************/
byte GetSByte(void){
	
	while(SerialReady());	 		// wait for new input data
	return(ReadSerialPort());		// read input data on selected COM port
}

/*******************************************************************************
hex2char


	input: 4-bit hex value in low nibble of input
	outpt: ASCII representation of input
********************************************************************************/
byte hex2char(byte hval) {
	if(hval < 0x0A) {
		hval += 0x30;						// convert high nibble numeric to ASCII text 
	} else {
		hval += 0x37;						// convert high nibble alpha to ASCII text
	}
	return hval;							// return ASCII char
}

/*******************************************************************************
char2hex

	converts an input  value to its hex value.  ONLY CONVERTS ASCII NUMBERS. 
	
	input: 8-bit ASCII char 
	outpt: hex byte
********************************************************************************/
byte char2hex(byte cval){
	if(cval > 0x30 && cval < 0x3A){
		cval -= 0x30;						// convert char to hex
	}
	return cval;							// return hex val
}

/*******************************************************************************
outhex

	formats 8-bit hex value into ASCII for display on selected COM port
	
	input: 8-bit hex value 
	outpt: ASCII representation of input to selected COM port
********************************************************************************/
void outhex(byte hval){
	// convert HEX value input to ASCII text output

	unsigned char unib,lnib;			// temp variable for upper and lower nibbles
	
	unib = (hval >> 4);					// high nibble
	lnib = hval & 0x0F;					// low nibble

	putch(hex2char(unib));				// send char out selected COM port
	putch(hex2char(lnib));				// send char out selected COM port
}  									

/*******************************************************************************
cls
	
	clears COM dispay by sending 80 lines of CR/LF
	
********************************************************************************/
void cls(void){
	byte n;									// temp variable
	
	for(n=0;n<80;++n){
		putchar('\n');				// send CR/LF 
	}
}
/*******************************************************************************
DisplayPOT
	
	formats POT analog result for display
	
	INPUT:		port		0=SCI1, 1=SCI2
	outpt: 		none
*******************************************************************************
void DisplayATD(void){
	fputs(0, ATDstr);						// send formatted string
	outhex(0,ADCRL);						// send results
}*/


/*******************************************************************************
Echo_Com
	
	echoes keystrokes in terminal window
	
	INPUT:		none
	outpt: 		none
*******************************************************************************/
void echo_com(void){

	putch(GetSByte());			// echo keybd data
	
}