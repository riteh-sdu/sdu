// 
// 	SDU_lab_1_1
//

#include "DSP2833x_Device.h"	// Headeri
#include "SDU_main_init.h"

void delay_loop(long);	// Delay funkcija

void main(void)
{
	// Omogucavanje Watchdog timer-a.
	WATCHDOG_TIMER_SETUP

	// Podesavanje takta dsp-a.
	DSP_CLOCK_SETUP

	// Deklariranje pinova kao izlazi. (Testni program.)
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
	    	WATCHDOG_TIMER_RESET;

	    	// Naizmjenicno Blinkanje prva dva pina. (Testni program.)
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
		WATCHDOG_TIMER_RESET;
	}
}



