/*
 * pwm.c
 *
 *  Created on: 3. tra 2018.
 *      Author: L3PC07
 */
#include "DSP2833x_Device.h"	// Headeri

void delay_loop(long);	// Delay funkcija


void main(void)
{



/// EALLOW
/// SysCtrlRegs.WDCR = 0x0068;  //watchdog ukljucen
/// EDIS

	SysCtrlRegs.PLLCR.bit.DIV = 10;     // pomnozeno ulaznih 30MHz s 10
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;  // podjeljen s 2 =300/2 = 150MHz

	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  //postavljanje pinova kao pwm (1A)
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  //postavljanje pinova kao pwm (1B)
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;	//postavljanje pinova kao pwm (2A)
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;	//postavljanje pinova kao pwm (2B)
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;	//postavljanje pinova kao pwm (3A)
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;	//postavljanje pinova kao pwm (3B)
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;	// ENCLK ePWM
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; // ENCLK ePWM
	SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; // ENCLK ePWM

		EPwm1Regs.TBCTL.bit.CLKDIV =  0;	// CLKDIV = 1
		EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;	// HSPCLKDIV = 1
		EPwm1Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
		EPwm1Regs.AQCTLA.all= 0x0060; // EPwm1Regs.AQCTLA.bit.CAU = 0b01; // EPwm1Regs.AQCTLA.bit.CAD = 0b10;
		EPwm1Regs.AQCTLB.all= 0x0090;
		EPwm1Regs.TBPRD = 7500; //PERIOD_REGISTER  TBPRD = 1875
		EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD / 2;  //promjena duty cyclea
		EPwm1Regs.DBCTL.all=0x000B; //postavlanje registra za mrtvo vrijeme
		EPwm1Regs.DBRED=600; // fclk*Risedelay
		EPwm1Regs.DBFED=600; // Fclk*falldelay
	
		EPwm2Regs.TBCTL.bit.CLKDIV =  0;	// CLKDIV = 1
		EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;	// HSPCLKDIV = 1
		EPwm2Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
		EPwm2Regs.AQCTLA.all= 0x0060; // EPwm1Regs.AQCTLA.bit.CAU = 0b01; // EPwm1Regs.AQCTLA.bit.CAD = 0b10;
		EPwm2Regs.AQCTLB.all= 0x0090;
		EPwm2Regs.TBPRD = 7500; //PERIOD_REGISTER  TBPRD = 1875
		EPwm2Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD / 2; //promjena duty cyclea
		EPwm2Regs.DBCTL.all=0x000B; //postavlanje registra za mrtvo vrijeme
		EPwm2Regs.DBRED=600; // fclk*Risedelay
		EPwm2Regs.DBFED=600; // Fclk*falldelay

		EPwm3Regs.TBCTL.bit.CLKDIV =  0;	// CLKDIV = 1
		EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;	// HSPCLKDIV = 1
		EPwm3Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
		EPwm3Regs.AQCTLA.all= 0x0060; // EPwm1Regs.AQCTLA.bit.CAU = 0b01; // EPwm1Regs.AQCTLA.bit.CAD = 0b10;
		EPwm3Regs.AQCTLB.all= 0x0090;
		EPwm3Regs.TBPRD = 7500; //PERIOD_REGISTER TBPRD = 1875
		EPwm3Regs.CMPA.half.CMPA = EPwm1Regs.TBPRD / 2; //promjena duty cyclea
		EPwm3Regs.DBCTL.all=0x000B; //postavlanje registra za mrtvo vrijeme
		EPwm3Regs.DBRED=600; // fclk*Risedelay
		EPwm3Regs.DBFED=600; // Fclk*falldelay

		EPwm1Regs.TZCTL.bit.TZA = 0b10;		// force ePWM1A to zero
		EPwm1Regs.TZCTL.bit.TZB = 0b10;		// force ePWM1B to zero

		EPwm2Regs.TZCTL.bit.TZA = 0b10;		// force ePWM2A to zero
		EPwm2Regs.TZCTL.bit.TZB = 0b10;		// force ePWM2B to zero

		EPwm3Regs.TZCTL.bit.TZA = 0b10;		// force ePWM3A to zero
		EPwm3Regs.TZCTL.bit.TZB = 0b10;		// force ePWM3B to zero

		EPwm1Regs.TZSEL.bit.OSHT6 = 1;	// select TZ6 as one shot over current source
		EPwm2Regs.TZSEL.bit.OSHT6 = 1;	// select TZ6 as one shot over current source
		EPwm3Regs.TZSEL.bit.OSHT6 = 1;	// select TZ6 as one shot over current source

		EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;	// generate a syncout if CTR = 0
		EPwm1Regs.TBPHS.half.TBPHS = 0;

		EPwm2Regs.TBCTL.bit.PHSEN =1;		// enable phase shift for ePWM2
		EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;	// syncin = syncout
		EPwm2Regs.TBPHS.half.TBPHS = 0;

		EPwm3Regs.TBCTL.bit.PHSEN = 1;		// enable phase shift for ePWM3
		EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;	// syncin = syncout
		EPwm3Regs.TBPHS.half.TBPHS =0;
	EDIS;

	    while (1)

	    {

	EALLOW;
	  SysCtrlRegs.WDKEY = 0x55;
	  SysCtrlRegs.WDKEY = 0xAA;
	EDIS;

	    }
}

