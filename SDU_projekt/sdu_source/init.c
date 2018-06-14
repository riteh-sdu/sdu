/*
 * test.c
 *
 *  Created on: 13. lip 2018.
 *      Author: Lorango
 */

//#include "SDU_main_init.h"
#include "DSP2833x_Device.h"

int fun_1(int x)
{
	return x + 3;
}

/*
 * Funkcija za podešavanje takta dsp-a.
 */
void dsp_clock_setup(void)
{
	EALLOW;
	SysCtrlRegs.PLLCR.bit.DIV = 10;			// Skaliranje frekvencije oscilatora. (30 MHz * 10 = 300 MHz)
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;		// 300 MHz / 2 = 150MHz

	SysCtrlRegs.HISPCP.bit.HSPCLK = 0; 		// Skaliranje frekvencije takta koji dolazi na adc jedinicu. (f / 1)
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1; 	// Omoguæi dovod sistemskog takta na adc jedinicu.
	EDIS;
}

/*
 * Funkcija za ubrzavanje izvoðenja programa iz flash memorije.
 */
void dsp_flash_setup(void)
{
	EALLOW;
	FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;	// Smanjuje vrijeme èekanja.
	FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;	// Smanjuje vrijeme èekanja.
	FlashRegs.FOTPWAIT.bit.OTPWAIT = 8;		// Smanjuje vrijeme èekanja.
	FlashRegs.FOPT.bit.ENPIPE = 1;			// Ubrzava èitanje iz flesh-a (pipeline).
	EDIS;
}

/*
 * Funkcija za podešavanje watchdog timer-a.
 */
void watchdog_timer_setup(void)
{
	EALLOW;
	SysCtrlRegs.WDCR = 0x00AF;
	EDIS;
}

/*
 * Funkcija za resetiranje watchdog timer-a.
 */
void watchdog_timer_reset(void)
{
	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;
	SysCtrlRegs.WDKEY = 0xAA;
	EDIS;
}

/*
 * Delay funkcija
 */
void delay_loop(long end)
{
	long i;
	for (i=0; i<end;i++)
	{
		watchdog_timer_reset();
		asm(" NOP");
	}
}

/*
 * Deklariranje pinova kao izlazi. (Testni program.)
 */
void test_prog_1_setup(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
	GpioDataRegs.GPASET.bit.GPIO9 = 0;

	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
	GpioDataRegs.GPASET.bit.GPIO11 = 1;
	EDIS;
}

/*
 * Naizmjenicno treperenje svjetlecih dioda spojenih na prva dva pina.
 * (GPI09 i GPIO11)
 * (Testni program.)
 */
void test_prog_1_loop(void)
{
	delay_loop(1000000);
	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
}






