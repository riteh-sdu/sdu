/*
 * main.c
 */

#include "DSP2833x_Device.h"
#include "IQmathLib.h"

#include "sdu_headers/init.h"
#include "sdu_headers/adc.h"

#define GLOBAL_Q 18

int a = 1;

//_iq b=0, c=0, rez=0, zer=0;

int main(void)
{
	dsp_flash_setup();
	dsp_clock_setup();			// Podesavanje takta dsp-a.
	watchdog_timer_setup();		// Omogucavanje Watchdog timer-a.
	adc_setup();
	interrupt_setup_adc();

	test_prog_1_setup();		// Deklariranje pinova kao izlazi. (Testni program.)

	//b = _IQ(10);
	//c = _IQ(2);
	//rez = _IQdiv(b, c);
	//zer = _IQmpy(b, c);

	a = fun_1(a);
	fun_offset_adc();

	while (1)
	{
		watchdog_timer_reset();
		test_prog_1_loop();

		adc_loop();
	}

	return 0;
}
