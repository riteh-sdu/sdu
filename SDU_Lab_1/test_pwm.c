#include "DSP2833x_Device.h"

void Setup_ePWM(void)
{

	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;	// ePWM1A active
	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;	// ePWM2A active
	GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;	// ePWM3A activ
	GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0b01;  //Trip Zone TZ1
	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0; // Pull up resistor  TZ6

	//GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0b11;  //Trip Zone  TZ6
	//GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0; // Pull up resistor  TZ6

	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

	SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1; // ENCLK ePWM
	SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; // ENCLK ePWM
	SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; // ENCLK ePWM
	EDIS;

	EPwm1Regs.TBCTL.bit.CLKDIV =  0;	// CLKDIV = 1
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;	// HSPCLKDIV = 1
	EPwm1Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
	EPwm1Regs.AQCTLA.bit.CAU = 0b01;
	EPwm1Regs.AQCTLA.bit.CAD = 0b10;



	EPwm1Regs.TBPRD = 7500;	// 10KHz - PWM signal


	EPwm1Regs.DBCTL.bit.OUT_MODE = 0b11; // enable Dead-band module
	EPwm1Regs.DBCTL.bit.POLSEL = 0b10;
	EPwm1Regs.DBFED = 600; // FED
	EPwm1Regs.DBRED = 600; // RED

	EPwm2Regs.TBCTL.bit.CLKDIV =  0;	// CLKDIV = 1
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;	// HSPCLKDIV = 1
	EPwm2Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
	EPwm2Regs.AQCTLA.bit.CAU = 0b01;
	EPwm2Regs.AQCTLA.bit.CAD = 0b10;
	EPwm2Regs.TBPRD = 7500;			// 10KHz - PWM signal


	EPwm2Regs.DBCTL.bit.OUT_MODE = 0b11; // enable Dead-band module
	EPwm2Regs.DBCTL.bit.POLSEL = 0b10;
	EPwm2Regs.DBFED = 600; // FED
	EPwm2Regs.DBRED = 600; // RED

	EPwm3Regs.TBCTL.bit.CLKDIV =  0;	// CLKDIV = 1
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;	// HSPCLKDIV = 1
	EPwm3Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
	EPwm3Regs.AQCTLA.bit.CAU = 0b01;
	EPwm3Regs.AQCTLA.bit.CAD = 0b10;
	EPwm3Regs.TBPRD = 7500;		// 10KHz - PWM signal


	EPwm3Regs.DBCTL.bit.OUT_MODE = 0b11; // enable Dead-band module
	EPwm3Regs.DBCTL.bit.POLSEL = 0b10;
	EPwm3Regs.DBFED = 600; // FED
	EPwm3Regs.DBRED = 600; // RED



	EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;	// generate a syncout if CTR = 0
	EPwm1Regs.TBPHS.half.TBPHS = 0;

	EPwm2Regs.TBCTL.bit.PHSEN =1;		// enable phase shift for ePWM2
	EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;	// syncin = syncout
	EPwm2Regs.TBPHS.half.TBPHS = 0;

	EPwm3Regs.TBCTL.bit.PHSEN = 1;		// enable phase shift for ePWM2
	EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;	// syncin = syncout
	EPwm3Regs.TBPHS.half.TBPHS =0;

	EALLOW;
	EPwm1Regs.TZCTL.bit.TZA = 0b10;		// force ePWM1A to zero
	EPwm1Regs.TZCTL.bit.TZB = 0b10;		// force ePWM1B to zero

	EPwm2Regs.TZCTL.bit.TZA = 0b10;		// force ePWM2A to zero
	EPwm2Regs.TZCTL.bit.TZB = 0b10;		// force ePWM2B to zero

	EPwm3Regs.TZCTL.bit.TZA = 0b10;		// force ePWM3A to zero
	EPwm3Regs.TZCTL.bit.TZB = 0b10;		// force ePWM3B to zero

	EPwm1Regs.TZSEL.bit.OSHT6 = 1;	// select TZ6 as one shot over current source
	EPwm2Regs.TZSEL.bit.OSHT6 = 1;	// select TZ6 as one shot over current source
	EPwm3Regs.TZSEL.bit.OSHT6 = 1;	// select TZ6 as one shot over current source
	EDIS;

	/*EPwm1Regs.ETSEL.all = 0;
	EPwm1Regs.ETSEL.bit.INTEN = 1;		// interrupt enable for ePWM1
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;     // Enable event trigger
	EPwm1Regs.ETSEL.bit.SOCASEL = 0b010;	// or 0b001*/

	 EPwm1Regs.ETSEL.all = 0;
	 EPwm1Regs.ETSEL.bit.INTEN = 0b0;  // interrupt enable for ePWM1
	 EPwm1Regs.ETSEL.bit.SOCAEN = 0b1;
	 EPwm1Regs.ETSEL.bit.SOCASEL = 0b010;    // or 0b001
	 EPwm1Regs.ETPS.bit.SOCAPRD    = 0b1;
}
