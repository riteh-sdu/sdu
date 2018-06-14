/*
 * adc.c
 *
 *  Created on: 14. lip 2018.
 *      Author: Lorango
 */

#include "DSP2833x_Device.h"
//#include "IQmathLib.h"

#include "../sdu_headers/init.h"

/*
 * Funkcija za podešavanje adc registara.
 */
void adc_setup(void)
{
	watchdog_timer_reset();

	EALLOW;
    // ADC Control Register 1
    AdcRegs.ADCTRL1.all = 0;
    AdcRegs.ADCTRL1.bit.RESET = 0b0;    // Reset 0 = no effect
    AdcRegs.ADCTRL1.bit.SUSMOD = 0b00;  // Free run
    AdcRegs.ADCTRL1.bit.ACQ_PS = 0b000;	// Acquisition time prescale
    AdcRegs.ADCTRL1.bit.CPS = 0b0;      // Conversion prescale ADCCLK = FCLK / 1
    AdcRegs.ADCTRL1.bit.CONT_RUN = 0b0; // Continuous run
    AdcRegs.ADCTRL1.bit.SEQ_OVRD = 0b0; // Sequencer pointer resets to “initial state” at end of MAX_CONVn
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 0b1; // Cascaded mode

    // ADC Control Register 2
    AdcRegs.ADCTRL2.all = 0;
    AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ = 0b1;    // Cascaded mode only (0 = no action, 1 = start by ePWM signal)
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0b1;         // Immediate reset SEQ1 to "initial state"
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 0b0;         // Software trigger-start SEQ1
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0b1;     // Interrupt Enable (SEQ1)
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 = 0b0;     // Interrupt Mode (SEQ1)
    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 0b1;   // Start by ePWM trigger (0 or 1)
    AdcRegs.ADCTRL2.bit.EXT_SOC_SEQ1 = 0b0;     // External SOC (SEQ1)

    // If we do not use sequencer 2 because we are in “Cascaded Mode”, these bits are “don’t care’s.
    AdcRegs.ADCTRL2.bit.RST_SEQ2 = 0b1;         // Immediate reset SEQ2 to "initial state"
    AdcRegs.ADCTRL2.bit.SOC_SEQ2 = 0b1;         // Software trigger-start SEQ2
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ2 = 0b1;     // Interrupt Enable (SEQ2)
    AdcRegs.ADCTRL2.bit.INT_MOD_SEQ2 = 0b0;     // Interrupt Mode (SEQ2)
    AdcRegs.ADCTRL2.bit.EPWM_SOCB_SEQ2 = 0b1;   // Start by ePWM trigger (0 or 1)

    // ADC Control Register 3
    AdcRegs.ADCTRL3.all = 0;
    AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0b11;       // Powered up
    AdcRegs.ADCTRL3.bit.ADCPWDN = 0b1;          // Powered up
    AdcRegs.ADCTRL3.bit.ADCCLKPS = 0b110;       // (110 = 6) FCLK = HSPCLK / (2*ADCCLKPS) -> 12.5MHz
    AdcRegs.ADCTRL3.bit.SMODE_SEL = 0b1;        // Sequential sampling mode

    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0b0010;  // (0010 = 3) Number of conversions
    AdcRegs.ADCREFSEL.bit.REF_SEL = 0b00;       // Internal reference (default)
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0b0100;   // ADC-A4 Ifb-Ret, ADC-B4 Ifb-U
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0b1110;   // ADC-B6 Ifb-V
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0b0001; // ADC-A1 Vfb-Bus
	EDIS;
}
