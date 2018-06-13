/*
 * main.c
 */

#include "DSP2833x_Device.h"
#include "SDU_main_init.h"

int a = 1;

int main(void){
	a = fun_1(a);
	// Podesavanje takta dsp-a.
	DSP_CLOCK_SETUP();

	// Omogucavanje Watchdog timer-a.
	WATCHDOG_TIMER_SETUP();

	// Deklariranje pinova kao izlazi. (Testni program.)
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;
	GpioDataRegs.GPASET.bit.GPIO9 = 0;
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
	GpioDataRegs.GPASET.bit.GPIO11 = 0;
	EDIS;

	while (1 == 1){
			WATCHDOG_TIMER_RESET();

			// Naizmjenicno treperenje svjetlecih dioda spojenih na prva dva pina. (Testni program.)
			delay_loop(1000000);
			GpioDataRegs.GPASET.bit.GPIO9 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;

			delay_loop(1000000);
			GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
			GpioDataRegs.GPASET.bit.GPIO11 = 1;
	}

	return 0;
}
