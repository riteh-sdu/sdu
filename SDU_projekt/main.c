/*
 * main.c
 */

#include "DSP2833x_Device.h"
#include "IQmathLib.h"

#include "SDU_main_init.h"

#define GLOBAL_Q 18

int a = 1;

_iq b, c, rez;

int main(void){

	b = _IQ(10);
	c = _IQ(2);
	rez = _IQdiv(b, c);

	a = fun_1(a);
	// Podesavanje takta dsp-a.
	dsp_clock_setup();

	// Omogucavanje Watchdog timer-a.
	watchdog_timer_setup();

	// Deklariranje pinova kao izlazi. (Testni program.)
	test_prog_1_setup();

	while (1){
		watchdog_timer_reset();

		test_prog_1_loop();
	}

	return 0;
}
