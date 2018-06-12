/*
 * vjezba2.c
 *
 *  Created on: 20. ožu 2018.
 *      Author: L3PC07
 */

//
// 	SDU_lab_2_1
//

#include "DSP2833x_Device.h"					// Headeri

interrupt void Prvi_ISR(void);					// Deklaracija prekidne rutine Prvi_ISR
void delay_loop(long);							// Deklaracija funkcije delay_loop
int hex;

void main(void)
{
	EALLOW;
		SysCtrlRegs.WDCR = 0b0000000000101000;
	EDIS;

	EALLOW;
		GpioCtrlRegs.GPAMUX1.all = 0x0000;	// GPIO
		GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;  //OUTPUT
		GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
		GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;
		GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;
		GpioCtrlRegs.GPADIR.bit.GPIO15 = 0;
	EDIS;

	DINT;										// Onemoguèi sve prekide

	EALLOW;
	PieVectTable.XINT1 = &Prvi_ISR;                  //definiramo adresu prekidne rutine (funkcije)
	EDIS;

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;       //omoguci PIE jedinicu
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;       // XINT1 na INT1

	EALLOW;
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 17;	//  Postavljanje GPIO17 (PB1) kao izvora vanjskog prekida (XINT1)
	XIntruptRegs.XINT1CR.bit.POLARITY = 0;    	//	Generiraj prekid na rastuæem bridu
	XIntruptRegs.XINT1CR.bit.ENABLE = 1;		//  Omoguæi XINT1 prekid
	EDIS;

	IER |= 0x0001;

	EINT;
	ERTM;


	while (1)

	{


	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;
	SysCtrlRegs.WDKEY = 0xAA;
	EDIS;



	}

}

interrupt void Prvi_ISR (void)					// ISR, rutina koja se izvodi nakon pojave prekida


{

	// Programski kod prekidne rutine
	// Programski kod prekidne rutine
	// Programski kod prekidne rutine

	hex= GpioDataRegs.GPADAT.bit.GPIO12*1   + GpioDataRegs.GPADAT.bit.GPIO13*2 + GpioDataRegs.GPADAT.bit.GPIO14*4 + GpioDataRegs.GPADAT.bit.GPIO15*8;
	int b;
		for (b=0; b<hex; b++)
		{

	 GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	 delay_loop(900000);
	 GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	 delay_loop(900000);
		}
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;	// Potvrda prekida

}


void delay_loop (long end)						// Delay_loop funkcija
{
	long i;
	for (i=0; i<end; i++)
	{

		asm( " NOP" );

	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;
	SysCtrlRegs.WDKEY = 0xAA;
	EDIS;
	}

}


