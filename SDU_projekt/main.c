/*
 * main.c
 */

#include "DSP2833x_Device.h"
#include "IQmathLib.h"

#include "sdu_headers/init.h"
#include "sdu_headers/adc.h"
#include "sdu_headers/pwm.h"
#include "sdu_headers/kom.h"
#include "sdu_headers/qep.h"
#include "sdu_headers/pr1.h"
#include "sdu_headers/pr2.h"

#define GLOBAL_Q 18

int main(void)
{
	watchdog_timer_setup();		// Omogucavanje Watchdog timer-a.
	dsp_clock_setup();			// Podesavanje takta dsp-a.
	dsp_flash_setup();			// Nerabi palit ako se ne vrti zi fleša

	dsp_muxio_setup();

	pwm_setup();
	adc_setup();
	interrupt_setup_adc_2();
	pr1_setup();
	qep_setup();
	kom_setup();

	fun_offset_adc();

	test_prog_1_setup();		// Deklariranje pinova kao izlazi. (Testni program.)

	while (1)
	{
		watchdog_timer_reset();
		test_prog_1_loop();

		pwm_loop();
		adc_loop();
		qep_loop();

		pr1_loop();

		kom_loop();


		delay_loop(1000000);
	}

	return 0;
}
