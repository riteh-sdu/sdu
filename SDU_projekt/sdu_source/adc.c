/*
 * adc.c
 *
 *  Created on: 14. lip 2018.
 *      Author: Lorango
 */

#include "DSP2833x_Device.h"

#include "../sdu_headers/init.h"
#include "../sdu_headers/adc.h"
#include "../sdu_headers/pr1.h"
#include "../sdu_headers/pr2.h"

unsigned long Ifb_Ret = 0;
unsigned long Ifb_U = 0;
unsigned long Ifb_V = 0;
unsigned long Vfb_Bus = 0;

unsigned long Ifb_Ret_offset = 0;
unsigned long Ifb_U_offset = 0;
unsigned long Ifb_V_offset = 0;
unsigned long Vfb_Bus_offset = 0;

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
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0b0001; 	// ADC-A1 Vfb-Bus
	EDIS;
}

/*
 * Funkcija u glavnoj petlji adc-a.
 */
void adc_loop(void)
{
	watchdog_timer_reset();
}

/*
 * Funkcija za podešavanje prekida. (adc)
 * Interrupt-i nisu dobro podešeni i to dovodi do restartiranja dsp-a.
 * Ne koristit.
 */
void interrupt_setup_adc(void)
{
	watchdog_timer_reset();

    DINT;
    IFR = 0x0000;       // No pending interrupts
    IER = 0x0000;       // Maskable interrupts disabled

    PieCtrlRegs.PIECTRL.bit.ENPIE = 0;

    // Interrupt Flag Register (IFR)
    PieCtrlRegs.PIEIFR1.all = 0x0000;
    PieCtrlRegs.PIEIFR2.all = 0x0000;
    PieCtrlRegs.PIEIFR3.all = 0x0000;
    PieCtrlRegs.PIEIFR4.all = 0x0000;
    PieCtrlRegs.PIEIFR5.all = 0x0000;
    PieCtrlRegs.PIEIFR6.all = 0x0000;
    PieCtrlRegs.PIEIFR7.all = 0x0000;
    PieCtrlRegs.PIEIFR8.all = 0x0000;
    PieCtrlRegs.PIEIFR9.all = 0x0000;
    PieCtrlRegs.PIEIFR10.all = 0x0000;
    PieCtrlRegs.PIEIFR11.all = 0x0000;
    PieCtrlRegs.PIEIFR12.all = 0x0000;

    // Interrupt Enable Register (IER)
    PieCtrlRegs.PIEIER1.all = 0x0000;
    PieCtrlRegs.PIEIER2.all = 0x0000;
    PieCtrlRegs.PIEIER3.all = 0x0000;
    PieCtrlRegs.PIEIER4.all = 0x0000;
    PieCtrlRegs.PIEIER5.all = 0x0000;
    PieCtrlRegs.PIEIER6.all = 0x0000;
    PieCtrlRegs.PIEIER7.all = 0x0000;
    PieCtrlRegs.PIEIER8.all = 0x0000;
    PieCtrlRegs.PIEIER9.all = 0x0000;
    PieCtrlRegs.PIEIER10.all = 0x0000;
    PieCtrlRegs.PIEIER11.all = 0x0000;
    PieCtrlRegs.PIEIER12.all = 0x0000;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;      // Enable the PIE
    PieVectTable.SEQ1INT = &int_rut;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    IER |= 0x1;                             // Enable interrupt core line 1 (INT1).
    EINT;                                   // Enable control – interrupts (EINT)
    ERTM;                                   // Enable debug – interrupts (ERTM)
    EDIS;
}

/*
 * Alternativni setup za interrapti.
 * Ne dovodi do resetiranja dsp-a. (To je dobra stvar).
 * Ispravnost ispravne funkcionalnosti se još mora testirat.
 * (Prema debugging alatu pokreæe se prekidna rutina.)
 */
void interrupt_setup_adc_2(void)
{
	watchdog_timer_reset();
    DINT;		// Onemoguci prekide

	EALLOW;
	PieVectTable.SEQ1INT = &int_rut;  // prekidna rutina prv_isr
	EDIS;

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1; // omoguci pie jedinicu
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1; // xint1 na seq1int

	IER |= 0x0001;

    EINT;		// Omoguci prekide
    ERTM;

}

/*
 * Prekidna rutina. (adc)
 */
interrupt void int_rut(void)
{
    Ifb_Ret = AdcMirror.ADCRESULT0;
    Ifb_U = AdcMirror.ADCRESULT1;
    Ifb_V = AdcMirror.ADCRESULT3;
    Vfb_Bus = AdcMirror.ADCRESULT4;

	pr1_interrupt();						// Regulacija brzine vrtnje.
	pr2_interrupt();						// Zaštita.

    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;     // Clear Interrupt Flag ADC Sequencer 1
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0b1;     // Immediate reset SEQ1 to "initial state"
    PieCtrlRegs.PIEACK.bit.ACK1 = 1; 		// PIE Interrupt Acknowledge Register
}

/*
 * Izracun offset vrijednosti. (adc)
 */
void fun_offset_adc(void)
{
    //DELAY_US(1000);

    // Izracun offset vrijednosti
    int adc_i;
    for(adc_i = 0; adc_i < 1024; adc_i++)
    {
    	watchdog_timer_reset();
        Ifb_Ret_offset = AdcMirror.ADCRESULT0 + Ifb_Ret_offset;
        Ifb_U_offset = AdcMirror.ADCRESULT1 + Ifb_U_offset;
        Ifb_V_offset = AdcMirror.ADCRESULT3 + Ifb_V_offset;
        Vfb_Bus_offset = AdcMirror.ADCRESULT4 + Vfb_Bus_offset;
        //DELAY_US(100);
    }

    // Izracun offset vrijednosti
    Ifb_Ret_offset = Ifb_Ret_offset >> 10;
    Ifb_U_offset = Ifb_U_offset >> 10;
    Ifb_V_offset = Ifb_V_offset >> 10;
    Vfb_Bus_offset = Vfb_Bus_offset >> 10;
}


