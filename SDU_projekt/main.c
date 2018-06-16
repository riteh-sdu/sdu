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
#include "sdu_headers/pr2.h"

#define GLOBAL_Q 18

int a = 1;


//_iq b=0, c=0, rez=0, zer=0;

int main(void)
{
	//dsp_flash_setup();			// Nerabi palit ako se ne vrti zi fleša
	dsp_clock_setup();			// Podesavanje takta dsp-a.
	watchdog_timer_setup();		// Omogucavanje Watchdog timer-a.
	adc_setup();
	interrupt_setup_adc();
	pwm_setup();
	kom_setup();
	qep_setup();

	test_prog_1_setup();		// Deklariranje pinova kao izlazi. (Testni program.)

	a = fun_1(a);
	//fun_offset_adc();

	a = a + a;

	while (1)
	{
		watchdog_timer_reset();
		test_prog_1_loop();

		adc_loop();
		pwm_loop();

		/*
		 *  Po meni bi trebalo napisat kod ki ne koristi u sebi beskonaènu petlju.
		 *  Jer ako se (ne šalje/ ne prima) poruka progrm zaglavi i watchdog resetira dsp.
		 */
		//kom_loop();

		qep_loop();
	}

	return 0;
}
