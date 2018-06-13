/*
 * main.c
 */

#include "DSP2833x_Device.h"
#include "IQmathLib.h"

#include "SDU_main_init.h"

#define GLOBAL_Q 18

int a = 1;

_iq b=0, c=0, rez=0, zer=0;

int main(void){
	// Podesavanje takta dsp-a.
	dsp_clock_setup();

	// Omogucavanje Watchdog timer-a.
	watchdog_timer_setup();

	// Deklariranje pinova kao izlazi. (Testni program.)
	test_prog_1_setup();

	b = _IQ(10);
	c = _IQ(2);
	rez = _IQdiv(b, c);
	zer = _IQmpy(b, c);

	a = fun_1(a);

	while (1){
		watchdog_timer_reset();

		test_prog_1_loop();
	}

	return 0;
}
