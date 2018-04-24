// 
// 	SDU_lab_1_1
//

#include "DSP2833x_Device.h"	// Headeri
#include "test_1.h"

void delay_loop(long);	// Delay funkcija

void main(void)
{
  	EALLOW;
	SysCtrlRegs.WDCR = 0x00AF;
	EDIS;

	EALLOW;
	SysCtrlRegs.PLLCR.bit.DIV = 10; //x10
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2; ///2
	EDIS;

	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
	GpioDataRegs.GPASET.bit.GPIO9 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
	GpioDataRegs.GPASET.bit.GPIO11 = 0;
	EDIS;

	    while (1)
	    {
	    	RESET_WATCHDOG_TIMER;

			delay_loop(1000000);
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;

			delay_loop(1000000);
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
	    }
}

void delay_loop(long end)
{
	// Delay funkcija
	long i;
	for (i=0; i<end;i++)
	{
		asm(" NOP");
		RESET_WATCHDOG_TIMER;
	}
}



