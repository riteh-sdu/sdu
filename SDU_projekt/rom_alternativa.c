/*
 * main.c
 */

#include "DSP2833x_Device.h"

#include "sdu_headers/init.h"

int main(void)
{
	dsp_flash_setup();			// Nerabi palit ako se ne vrti zi fleša
	dsp_clock_setup();			// Podesavanje takta dsp-a.
	watchdog_timer_setup();		// Omogucavanje Watchdog timer-a.

	test_prog_2_setup();		// Deklariranje pinova kao izlazi. (Testni program.)

	while (1)
	{
		watchdog_timer_reset();
		test_prog_2_loop();
	}

	return 0;
}
