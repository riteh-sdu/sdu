/*
 * main.c
 */

#include "DSP2833x_Device.h"
#include "SDU_main_init.h"

int a = 1;

int main(void){
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
