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
 * Funkcija za podešavanje watchdog timer-a.
 */
void watchdog_timer_setup(void)
{
	EALLOW;
	//SysCtrlRegs.WDCR = 0b000000001101111;
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
 * Funkcija za podešavanje takta dsp-a i perifernih jedinica koje se koristi.
 */
void dsp_clock_setup(void)
{
	watchdog_timer_reset();

	EALLOW;
	SysCtrlRegs.PLLCR.bit.DIV = 0b1010;			// Skaliranje frekvencije oscilatora. (30 MHz * 10 = 300 MHz)
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 0b10;		// 300 MHz / 2 = 150MHz

	// pwm
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;		// ENCLK ePWM
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; 	// ENCLK ePWM
	SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; 	// ENCLK ePWM

	// adc
	SysCtrlRegs.HISPCP.bit.HSPCLK = 0; 		// Skaliranje frekvencije takta koji dolazi na adc jedinicu. (f / 1)
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1; 	// Omoguæi dovod sistemskog takta na adc jedinicu.

	//qep
	SysCtrlRegs.PCLKCR1.bit.EQEP1ENCLK = 1;  // eQEP1
	SysCtrlRegs.PCLKCR1.bit.EQEP2ENCLK = 1; // eQEP2


	// kom
	SysCtrlRegs.PCLKCR0.bit.SCIAENCLK=1;	// prosljedi takt na komunikaciju
	EDIS;
}

/*
 * Funkcija za ubrzavanje izvoðenja programa iz flash memorije.
 */
void dsp_flash_setup(void)
{
	watchdog_timer_reset();

	EALLOW;
	FlashRegs.FBANKWAIT.bit.PAGEWAIT = 5;	// Smanjuje vrijeme èekanja.
	FlashRegs.FBANKWAIT.bit.RANDWAIT = 5;	// Smanjuje vrijeme èekanja.
	FlashRegs.FOTPWAIT.bit.OTPWAIT = 8;		// Smanjuje vrijeme èekanja.
	FlashRegs.FOPT.bit.ENPIPE = 1;			// Ubrzava èitanje iz flesh-a (pipeline).
	EDIS;
}

/*
 * Funkcija koja podešava sve ulaze i izlaze dsp-a
 */
void dsp_muxio_setup(void)
{
	watchdog_timer_reset();

	EALLOW;
	// pwm
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  		//postavljanje pinova kao pwm (1A)
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  		//postavljanje pinova kao pwm (1B)
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;			//postavljanje pinova kao pwm (2A)
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;			//postavljanje pinova kao pwm (2B)
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;			//postavljanje pinova kao pwm (3A)
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;			//postavljanje pinova kao pwm (3B)

	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0b11;		// Trip Zone TZ6
	GpioCtrlRegs.GPADIR.bit.GPIO17 = 0;			// TZ6 kao Input
	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;			// Upali "pull-up"

	// qep
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;		//eQEP qA
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;		//eQEP 1B
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 1;		//eQEP index

	// kom
	GpioCtrlRegs.GPAMUX2.bit.GPIO28=0b1;
	GpioCtrlRegs.GPADIR.bit.GPIO28=0;

	GpioCtrlRegs.GPAMUX2.bit.GPIO29=0b1;
	GpioCtrlRegs.GPADIR.bit.GPIO29=1;
	EDIS;
}

/*
 * Delay funkcija
 */
void delay_loop(long end)
{
	long init_i;
	for (init_i=0; init_i<end; init_i++)
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
 * Deklariranje pinova kao izlazi. (Testni program.)
 */
void test_prog_2_setup(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
	GpioDataRegs.GPASET.bit.GPIO9 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
	GpioDataRegs.GPASET.bit.GPIO11 = 0;

	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
	GpioDataRegs.GPBSET.bit.GPIO34 = 1;

	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
	GpioDataRegs.GPBSET.bit.GPIO49 = 0;
	EDIS;
}

/*
 * Naizmjenicno treperenje svjetlecih dioda spojenih na prva dva pina.
 * (GPI09 i GPIO11)
 * (Testni program.)
 */
void test_prog_1_loop(void)
{
	//delay_loop(1000000);
	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
}

/*
 * Naizmjenicno treperenje svjetlecih dioda.
 * PH(GPI09 i GPIO11)
 * (Testni program.)
 */
void test_prog_2_loop(void)
{
	delay_loop(1000000);
	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
	GpioDataRegs.GPBTOGGLE.bit.GPIO49 = 1;
}




