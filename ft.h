#ifndef	_FT_H_
#define	_FT_H_

#define LOFF		1
#define LON		0

#define LEDS_OFF	LED1=LOFF; LED2=LOFF; LED3=LOFF; LED4=LOFF
#define LEDS_ON	LED1=LON; LED2=LON; LED3=LON; LED4=LON

// ************** Function Prototypes *********************
void tim0_dly(void);				// delay routine
void mcu_init(void);				// init MCU for functional test
void init_timer0(void);			// init timer for func test
void init_atd_ch0(void);		// init ATD for funct test
void read_pot(void);				// read pot
void func_test(void);			// func test proc

byte read_atd_ch0(void);		// return ATD value

#endif		// _FT_H_