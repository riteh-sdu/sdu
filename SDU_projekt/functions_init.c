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

void dsp_clock_setup(void){
	/*
	 * Funkcija za podešavanje takta dsp-a.
	 */

	// 30 MHz * 10 / 2 = 150MHz
	EALLOW;
	SysCtrlRegs.PLLCR.bit.DIV = 10;
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
	EDIS;
}

void watchdog_timer_setup(void){
	/*
	 * Funkcija za podešavanje watchdog timer-a.
	 */

	EALLOW;
	SysCtrlRegs.WDCR = 0x00AF;
	EDIS;
}

void watchdog_timer_reset(void){
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
		watchdog_timer_reset();
		asm(" NOP");
	}
}

void test_prog_1_setup(void){
	/*
	 * Deklariranje pinova kao izlazi. (Testni program.)
	 */

	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
	GpioDataRegs.GPASET.bit.GPIO9 = 0;

	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
	GpioDataRegs.GPASET.bit.GPIO11 = 1;
	EDIS;
}

void test_prog_1_loop(void){
	/*
	 * Naizmjenicno treperenje svjetlecih dioda spojenih na prva dva pina.
	 * (GPI09 i GPIO11)
	 * (Testni program.)
	 */

	delay_loop(1000000);
	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
}






