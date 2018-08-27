/******************************************************************************

   FILE				: sci.h

   AUTHOR			: Axiom Manufacturing
						  Spencer Ruggles

	PURPOSE			: RS-232 routines for HCS12

 ******************************************************************************/

#define BAUD19	0x1A;					// 19200 baud rate 

#define LF 0x0A						// Line Feed ASCII code
#define CR 0x0D						// Carriage Return ASCII code

/********** Function Prototypes **********/
void ensci(void);						// enable COM port
void putch(byte);						// send a byte of data out COM port
void cputs(byte *);					// send a string of data out COM port
void outhex(byte);					// output HEX value to COM port
void DisplayATD(void);			// display formatted POT results
void ReadyToSend(void);				// wait for Tx Data Reg Empty flag to set
void echo_com(void);					// echo keybd data

void putchar(byte);					// wait for ReadyToSend and send a byte of data to COM port

byte char2hex(byte);					// convert input ASCII char to hex
byte hex2char(byte);					// convert hex input to ASCII char
byte SerialReady(void);				// checks for new input data on COM port
byte ReadSerialPort(void);			// reads data from serial port
byte GetSByte(void);					// waits for new data then reads data from COM port