/*
 * test.c
 *
 *  Created on: 13. lip 2018.
 *      Author: Lorango
 */

//#include "SDU_main_init.h"
#include "DSP2833x_Device.h"

int fun_1(int x){
	return x + 3;
}

void DSP_CLOCK_SETUP(void){
	/*
	 * Funkcija za podešavanje takta dsp-a.
	 */

	// 30 MHz * 10 / 2 = 150MHz
	EALLOW;
	SysCtrlRegs.PLLCR.bit.DIV = 10;
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
	EDIS;
}

void WATCHDOG_TIMER_SETUP(void){
	/*
	 * Funkcija za podešavanje watchdog timer-a.
	 */
	EALLOW;
	SysCtrlRegs.WDCR = 0x00AF;
	EDIS;
}

void WATCHDOG_TIMER_RESET(void){
	/*
	 * Funkcija za resetiranje watchdog timer-a.
	 */
	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;
	SysCtrlRegs.WDKEY = 0xAA;
	EDIS;
}

void delay_loop(long end){
	/*
	 * Delay funkcija
	 */
	long i;
	for (i=0; i<end;i++){
		WATCHDOG_TIMER_RESET();
		asm(" NOP");
	}
}
