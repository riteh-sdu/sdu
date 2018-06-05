//
// 	SDU_lab_1_1
//


#include "DSP2833x_Device.h"	// Headeri

void delay_loop(long);	// Delay funkcija


void main(void)
{

	EALLOW;
	SysCtrlRegs.WDCR = 0b0000000000101000;
	EDIS;
/// upis pomocu HEX
/// EALLOW
/// SysCtrlRegs.WDCR = 0x0068;
/// EDIS

	SysCtrlRegs.PLLCR.bit.DIV = 10;     // pomnozeno ulaznih 30MHz s 10
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;  // podjeljen s 2 =300/2 = 150MHz

	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  //postavljanje pinova kao pwm (1A)
		GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  //postavljanje pinova kao pwm (1B)

		SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

		SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;

		EPwm1Regs.TBCTL.bit.CLKDIV =  1;	// CLKDIV = 2
		EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;	// HSPCLKDIV = 2
		EPwm1Regs.TBCTL.bit.CTRMODE = 2;	// up - down mode
		EPwm1Regs.AQCTLA.all= 0x0006;; // EPwm1Regs.AQCTLA.bit.CAU = 0b01;
									// EPwm1Regs.AQCTLA.bit.CAD = 0b10;
		EPwm1Regs.TBPRD = 18750;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 00;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 00;//postavljanje GPIO pina LEDice
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 00;
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 00;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;   // postavljanje pina GPIO9 kao OUTPUT
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
	EDIS;

	    while (1)

	    {

	    GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1; //promjeni stanje pina
	    delay_loop(900000);	//pozivanje funkcije delay
	    GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	    GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
		delay_loop(900000);	//pozivanje funkcije delay

			    GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
			    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
				delay_loop(900000);	//pozivanje funkcije delay

					    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
					    GpioDataRegs.GPBTOGGLE.bit.GPIO49 = 1;
						delay_loop(900000);	//pozivanje funkcije delay

							    GpioDataRegs.GPBTOGGLE.bit.GPIO49 = 1;
							    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
								delay_loop(900000);	//pozivanje funkcije delay

															    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
															    GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
																delay_loop(900000);	//pozivanje funkcije delay

																							    GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;

	    }

}

void delay_loop(long end)
{
  long i;
  for (i=0;i<end;i++)
  {
	  EALLOW;
	  SysCtrlRegs.WDKEY = 0x55;
	  SysCtrlRegs.WDKEY = 0xAA;
      EDIS;
  }
	            // Delay funkcija

}
