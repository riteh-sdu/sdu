	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Sustavi digitalnog upravljanja
	// LV 8_1 - TI trening moduli
	// 31/01/2018
	//
	//	Zadatak:
	//		o	AD konverzija linija ADCIN_A0 i ADCIN_A1 (Frekvencija uzorkovanja potrebno je generirati pomoæu ePWM2: 50 kHz)
	//		o	ADCIN_A0 i ADCIN_A1 su spojeni na potenciometre VR1, VR2
	//		o	VR1 i VR2 naponski raspon je od 0 do 3.3 V
	//		o	Automatski start ADC pretvorbe poèinje kod ePWM2 periodiènog event-a
	//		o	ADC Prekidna servisna rutina èita rezultate ADC pretvorbe
	//		o	Rezultati pretvorbe na ADCINA0 i ADCINA1 se prikazuju na LD1-LD4
	//
	//	Komentari:	*
	//
	//
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include "DSP2833x_Device.h"
#include "IQmathLib.h"
#define GLOBAL_Q18

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

unsigned int Voltage_VR1;
unsigned int Voltage_VR2;

// Za nas dio zadatka strujena i naponska zastita
int i = 0;
_iq TH_H = 2000;
_iq MV_napon = 0;
_iq Buffer[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};
_iq16 N = _IQ(0.25);
_iq16 zbroj = 0;							// stavljeno u IQ16 format kako nebi doslo do overflowa




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

	ConfigCpuTimer(&CpuTimer0,150,100000);  // CPU - Timer0 at 100 milliseconds

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// CPU timer 0 => Oba dva prekida spojena na istu liniju; Moguæe je radi !!
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
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;		// ePWM_SOCA trigger - Omoguæava trigger signal za startanje od strane ePWM trigger signala
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 =	1;		// enable ADC interrupt for seq1 - Omoguæi prekid kod kraja seq1
	AdcRegs.ADCTRL2.bit.INT_MOD_SEQ1 =	0;		// interrupt after every EOS - Prekid se pojavljuje svaki put kod dovršetka seq1

	AdcRegs.ADCTRL3.bit.ADCCLKPS = 	3;			// set FCLK to 12.5 MHz => FCLK = HSPCLK / (2*ADCCLKPS)
	AdcRegs.ADCTRL3.bit.SMODE_SEL = 0;			// Sequential sampling mode

	AdcRegs.ADCMAXCONV.all = 0x0001;			// Sequencer1 æe napraviti dvije konverzije (Stavlja se broj konverzija - 1)

	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0;		// ADCINA0 postavljen za 1. SEQ1 konverziju
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 1;		// ACDINA1 postavljen za 2. SEQ1 konverziju


	while(1)
	{
	  		while (CpuTimer0.InterruptCount < 5);
	  		{
	  		// wait for 500 ms
			EALLOW;
			SysCtrlRegs.WDKEY = 0x55;	// service WD #1
			//SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
			EDIS;
	  		}

	  		//display_ADC(Voltage_VR1);

	  		while (CpuTimer0.InterruptCount < 10);
	  		{
	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		//SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	  		EDIS;
	  		}

	  		//display_ADC(Voltage_VR2);

	  		while (CpuTimer0.InterruptCount < 14);
	  		{
	  		EALLOW;
	  		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	  		//SysCtrlRegs.WDKEY = 0xAA; 	// service WD #2
	  		EDIS;
	  		}
	  		CpuTimer0.InterruptCount = 0;



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
	GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;	// service WD #1
	SysCtrlRegs.WDKEY = 0xAA;	// service WD #2
	EDIS;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}


void Setup_ePWM(void)
{

// ePWM2 æe generirati 50 kHz takt koji æe se koristiti kao "trigger" za SOCA, odnosno za pokretanje ADC konverzije

	EPwm2Regs.TBCTL.bit.CLKDIV = 0; 		// Faktor djeljenja ulaznog SYSCLKOUT 0 = 000 /1
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;		// Faktor djeljenja ulaznog SYSCLKOUT 5 = 000 /1 => TBCLK = 150 MHz /1/1 = 150 MHz
	EPwm2Regs.TBCTL.bit.CTRMODE = 0;		// Odabir "Count up" moda
	EPwm2Regs.TBCTL.bit.PRDLD = 0;			// Period shadow load 0 = load on CTR = 0
	EPwm2Regs.TBCTL.bit.PHSEN = 0;			// Phase disable
	//EPwm2Regs.TBCTL.bit.FREE_SOFT = 1x;
	EPwm2Regs.TBPRD = 2999;					// Gornja granica brojanja - TBPRD +1 = TPWM / (HSPCLKDIV*CLKDIV*TSYTCLK) = 20 us / 6.667 ns

	EPwm2Regs.ETPS.bit.SOCAPRD = 1;			// Generiraj SOCA trigger na prvom event-u
	EPwm2Regs.ETSEL.bit.SOCASEL = 0b001;	// Odabir Event-Triggera za ADC => Odabrano je se event-trigger generira kada je CTR=0
	EPwm2Regs.ETSEL.bit.SOCAEN = 1;			// Omoguèavanje SOCA "trigger" signala prema ADC-u

}

interrupt void adc_isr(void)
{

	Voltage_VR1=AdcMirror.ADCRESULT0;		// Spremanje rezultata konverzije u globalne varijable
	Voltage_VR2=AdcMirror.ADCRESULT1;
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;		// Reset ADC sequencer 1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;		// Clear interrupt flag ADC sequencer 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	// deklariranje varijabli
	int i_VN = 0; 							// brojaè za visoki napon
	int vrijeme_VN = 0;						// treba dobiti od ADC jedinice ??????

	// spremanje ulaznih podataka od ADC pretvorbe u buffer
	Buffer[3] = Buffer[2];
	Buffer[2] = Buffer[1];
	Buffer[1] = Buffer[0];
	//Buffer[0] = Voltage_VR1;
	Buffer[0] =  _IQ(Voltage_VR1);

	// Radimo Moving average filter
	zbroj = _IQtoIQ16(Buffer[3]) + _IQtoIQ16(Buffer[2]) + _IQtoIQ16(Buffer[1]) + _IQtoIQ16(Buffer[0]);
	MV_napon = _IQ16toIQ(_IQmpy(zbroj, N));

	// Visokonaponska zastita, moramo odrediti graniènu vrijednost (TH)????? dobiti od ADC jedinice ??????
	if (MV_napon > TH_H) {
		i_VN += 1;
	}
	if ((MV_napon < TH_H) && (i_VN > 0)) {
		i_VN -=1;
	}
	if (i_VN>=vrijeme_VN) {
		GpioDataRegs.GPASET.bit.GPIO9 = 1;
		//break;
	}

}

