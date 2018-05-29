	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Sustavi digitalnog upravljanja
	// LV 8_2 - TI trening moduli
	// 31/01/2018
	//
	//	Zadatak:
	//		o	Pomo�u potenciometara je potrebno upravljati brzinom LED broja�a
	//		o	Frekvencija promjene LED-ica treba biti izme�u 50 Hz i 1 kHz
	//		o
	//
	//	Komentari:	*
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "DSP2833x_Device.h"

// external function prototypes
extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void InitCpuTimers(void);
extern void ConfigCpuTimer(struct CPUTIMER_VARS *, float, float);
extern void display_ADC(int);
extern void InitAdc(void);

// Prototype statements for functions found within this file.
void Gpio_select(void);
void Setup_ePWM(void);

interrupt void cpu_timer0_isr(void);
interrupt void adc_isr(void);

float Voltage_VR1;

void main(void)
 {


	InitSysCtrl();							// Basic Core Init from DSP2833x_SysCtrl.c

	EALLOW;
   	SysCtrlRegs.WDCR= 0x00AF;				// Re-enable the watchdog
   	EDIS;									// 0x00AF  to NOT disable the Watchdog, Prescaler = 64

	DINT;									// Disable all interrupts

	Gpio_select();							// GPIO9, GPIO11, GPIO34 and GPIO49 as output
					    					// to 4 LEDs at Peripheral Explorer)
	Setup_ePWM();
	InitPieCtrl();							// basic setup of PIE table; from DSP2833x_PieCtrl.c

	InitPieVectTable();						// default ISR's in PIE

	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.ADCINT = &adc_isr;
	EDIS;

	InitCpuTimers();						// basic setup CPU Timer0, 1 and 2

	ConfigCpuTimer(&CpuTimer0,150,1000000);


	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// CPU timer 0 => Oba dva prekida spojena na istu liniju; Mogu�e je radi !!
	PieCtrlRegs.PIEIER1.bit.INTx6 = 1;		// ADC

	IER |=1;

	EINT;
	ERTM;

	CpuTimer0Regs.TCR.bit.TSS = 0;				// start timer0


	InitAdc();

	AdcRegs.ADCTRL1.all = 0;					// RESET
	AdcRegs.ADCTRL1.bit.SEQ_CASC =	1;			// Cascade Mode
	AdcRegs.ADCTRL1.bit.CONT_RUN = 0;			// Single run mode - zaustavlja se na kraju sekvence
	AdcRegs.ADCTRL1.bit.ACQ_PS = 7;				// 7 = 8 x ADC - Duljina sampling prozora
	AdcRegs.ADCTRL1.bit.CPS = 0;				// Divide by 1 => ADCCLK = FCLK / 1


	AdcRegs.ADCTRL2.all = 0;
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;		// ePWM_SOCA trigger - Omogu�ava trigger signal za startanje od strane ePWM trigger signala
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 =	1;		// enable ADC interrupt for seq1 - Omogu�i prekid kod kraja seq1
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 =	0;		// interrupt after every EOS - Prekid se pojavljuje svaki put kod dovr�etka seq1

	AdcRegs.ADCTRL3.bit.ADCCLKPS = 	3;			// set FCLK to 12.5 MHz => FCLK = HSPCLK / (2*ADCCLKPS)
	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0;			// Sequential sampling mode

	AdcRegs.ADCMAXCONV.all = 0x0001;			// Sequencer1 �e napraviti dvije konverzije (Stavlja se broj konverzija - 1)

	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;		// ADCINA0 postavljen za 1. SEQ1 konverziju
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;		// ACDINA1 postavljen za 2. SEQ1 konverziju



	while(1)
	{

			ConfigCpuTimer(&CpuTimer0,150,200000-190000*Voltage_VR1);			// Konfiguriranje broja�a izme�u 50Hz-1kHz
			CpuTimer0Regs.TCR.bit.TSS = 0;				// Restart Timera

	  		while (CpuTimer0.InterruptCount == 0)
	  		{

	  		GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
	  		GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

	  		EALLOW;
			SysCtrlRegs.WDKEY = 0x55;	// service WD #1
			SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
			EDIS;
	  		}


	  		while (CpuTimer0.InterruptCount == 1)
	  		{

	  		GpioDataRegs.GPASET.bit.GPIO9 = 1;
	  		GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	  		EDIS;
	  		}

	  		while (CpuTimer0.InterruptCount == 2)
	  		{

	  		GpioDataRegs.GPASET.bit.GPIO9 = 1;
	  		GpioDataRegs.GPASET.bit.GPIO11 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	  		EDIS;
	  		}

	  		while (CpuTimer0.InterruptCount == 3)
	  		{

	  		GpioDataRegs.GPASET.bit.GPIO9 = 1;
	  		GpioDataRegs.GPASET.bit.GPIO11 = 1;
	  		GpioDataRegs.GPBSET.bit.GPIO34 = 1;
	  		GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;

	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	  		EDIS;
	  		}


	  		while (CpuTimer0.InterruptCount == 4)
	  		{

	  		GpioDataRegs.GPASET.bit.GPIO9 = 1;
	  		GpioDataRegs.GPASET.bit.GPIO11 = 1;
	  		GpioDataRegs.GPBSET.bit.GPIO34 = 1;
	  		GpioDataRegs.GPBSET.bit.GPIO49 = 1;

	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	   		EDIS;
	  		}


	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	  		EDIS;

}
}

void Gpio_select(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0;		// GPIO15 ... GPIO0 = General Puropse I/O
	GpioCtrlRegs.GPAMUX2.all = 0;		// GPIO31 ... GPIO16 = General Purpose I/O
	GpioCtrlRegs.GPBMUX1.all = 0;		// GPIO47 ... GPIO32 = General Purpose I/O
	GpioCtrlRegs.GPBMUX2.all = 0;		// GPIO63 ... GPIO48 = General Purpose I/O
	GpioCtrlRegs.GPCMUX1.all = 0;		// GPIO79 ... GPIO64 = General Purpose I/O
	GpioCtrlRegs.GPCMUX2.all = 0;		// GPIO87 ... GPIO80 = General Purpose I/O

	GpioCtrlRegs.GPADIR.all = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;	// peripheral explorer: LED LD1 at GPIO9
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;	// peripheral explorer: LED LD2 at GPIO11

	GpioCtrlRegs.GPBDIR.all = 0;		// GPIO63-32 as inputs
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;	// peripheral explorer: LED LD3 at GPIO34
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; // peripheral explorer: LED LD4 at GPIO49
	GpioCtrlRegs.GPCDIR.all = 0;		// GPIO87-64 as inputs
	EDIS;
}

interrupt void cpu_timer0_isr(void)
{

	CpuTimer0.InterruptCount++;
	if (CpuTimer0.InterruptCount == 5)
	{
	CpuTimer0.InterruptCount = 0;
	}
	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	SysCtrlRegs.WDKEY = 0xAA;	// service WD #2
	EDIS;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}


void Setup_ePWM(void)
{

// ePWM2 �e generirati 50 kHz takt koji �e se koristiti kao "trigger" za SOCA, odnosno za pokretanje ADC konverzije

	EPwm2Regs.TBCTL.bit.CLKDIV = 0; 		// Faktor djeljenja ulaznog SYSCLKOUT 0 = 000 /1
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;		// Faktor djeljenja ulaznog SYSCLKOUT 5 = 000 /1 => TBCLK = 150 MHz /1/1 = 150 MHz
	EPwm2Regs.TBCTL.bit.CTRMODE = 0;		// Odabir "Count up" moda
	EPwm2Regs.TBCTL.bit.PRDLD = 0;			// Period shadow load 0 = load on CTR = 0
	EPwm2Regs.TBCTL.bit.PHSEN = 0;			// Phase disable
	//EPwm2Regs.TBCTL.bit.FREE_SOFT = 1x;
	EPwm2Regs.TBPRD = 2999;					// Gornja granica brojanja - TBPRD +1 = TPWM / (HSPCLKDIV*CLKDIV*TSYTCLK) = 20 us / 6.667 ns

	EPwm2Regs.ETPS.bit.SOCAPRD = 1;			// Generiraj SOCA trigger na prvom event-u
	EPwm2Regs.ETSEL.bit.SOCASEL = 0b001;	// Odabir Event-Triggera za ADC => Odabrano je se event-trigger generira kada je CTR=0
	EPwm2Regs.ETSEL.bit.SOCAEN = 1;			// Omogu�avanje SOCA "trigger" signala prema ADC-u

}

interrupt void adc_isr(void)
{
	float ADC_range = 4095;
	Voltage_VR1=AdcMirror.ADCRESULT0/ADC_range;		// Spremanje rezultata konverzije u range (0-1)
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;				// Reset ADC sequencer 1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;				// Clear interrupt flag ADC sequencer 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

