/******************************************************************************
Standard Definitions

These defines allow for easier porting to other compilers. if porting change
these defines to the required values for the chosen compiler.
******************************************************************************/

#ifndef _cpu_h_
#define _cpu_h_

typedef unsigned char	tU08;   /*unsigned 8 bit definition */
typedef unsigned int		tU16;   /*unsigned 16 bit definition*/
typedef unsigned long	tU32;   /*unsigned 32 bit definition*/
typedef signed char		tS08;   /*signed 8 bit definition */
typedef int					tS16;   /*signed 16 bit definition*/
typedef long int			tS32;   /*signed 32 bit definition*/


#ifndef UINT8
#define	UINT8		tU08			/* unsigned 8-bit */	
#define	UINT16	tU16			/* unsigned 16-bit */
#define	UINT32	tU32			/* unsigned 32-bit */
#define	INT8		tS08			/* signed 8-bit */
#define	INT16		tS16			/* signed 16-bit */
#define	INT32		tS32			/* signed 32-bit */
#endif /* UINT8 */


#define stop()		{asm stop;}
//******************************************************************************
//common labels
#define OFF		0
#define ON		1
#define NO		0
#define YES		1

/************************* #defines ******************************************/
#define ALL_FULL_DRIVE		0x00   
#define ALL_REDUCED_DRIVE	0xFF

#define ALL_PULLS_OFF		0x00
#define ALL_PULLS_ON			0xFF

#define ALL_PULLED_UP		0x00   
#define ALL_PULLED_DOWN		0xFF

#define ALL_INPUTS			0x00
#define ALL_OUTPUTS			0xFF	

#define ALL_LOW				0x00
#define ALL_HIGH				0xFF

#define ms_Delay_Cnt			0x1F40		// count used in Delay_1ms routine

#define SW1						PT1AD_PT1AD4
#define SW2						PT1AD_PT1AD5
#define SW3						PT1AD_PT1AD6
#define SW4						PT1AD_PT1AD7

#define TRANSDEMO				1
#define TEMPDEMO				2
#define ODODEMO				4
#define FCDEMO					8

#define LED1					PTR_PTR0
#define LED2					PTR_PTR1
#define LED3					PTR_PTR2
#define LED4					PTR_PTR3


/************************* Prototypes ****************************************/

void Configure_Ports(void);		// configure ports
void CPMU_PEE_8MHz(void);			// PEE mode, CPUCLK - 16MHz, BUSCLK - 8MHz
void CPMU_PBE(void);					// PBE mode from ext 8MHz XTAL, CPUCLK - 8MHz, BUSCLK - 4MHz
void Tim0_Dly(void);					// starts T0_C7 and waits for C7 flag to set
void Init_Timer1(void);				// init Timer1
void Delay_1ms(void);				// add 1ms delay
void init_atd_ch0(void);			// init ATD CH0
void read_pot(void);					

void Delay(word count);
byte read_atd_ch0(void);



 // Global variables
extern byte kbi_state;

#endif  			// _cpu_h_

