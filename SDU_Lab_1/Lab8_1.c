	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Sustavi digitalnog upravljanja
	// LV 8_1 - TI trening moduli
	// 31/01/2018
	//
	//	Zadatak:
	//		o	AD konverzija linija ADCIN_A0 i ADCIN_A1 (Frekvencija uzorkovanja potrebno je generirati pomo�u ePWM2: 50 kHz)
	//		o	ADCIN_A0 i ADCIN_A1 su spojeni na potenciometre VR1, VR2
	//		o	VR1 i VR2 naponski raspon je od 0 do 3.3 V
	//		o	Automatski start ADC pretvorbe po�inje kod ePWM2 periodi�nog event-a
	//		o	ADC Prekidna servisna rutina �ita rezultate ADC pretvorbe
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
// deklariranje varijabli

_iq TH_HV = 2000;										// granicna vrijednost za visoki napon
_iq TH_LV = 1000;										// granicna vrijednost za niski napon
_iq TH_mzI = 80;										// granicna vrijednost za mali zateg struje
_iq TH_vzI = 50;										// granicna vrijednost za veliki zateg struje

_iq MV_napon = 0;										// vrijednost napona iz moving average filtra
_iq MV_struja_U = 0;									// vrijednost struje faze U iz moving average filtra
_iq MV_struja_V = 0;									// vrijednost struje faze V iz moving average filtra
_iq MV_struja_W = 0;									// vrijednost struje faze W iz moving average filtra

_iq Buffer[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)}; 			// buffer za napon
_iq Buffer_I_U[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};		// buffer za stuju faze U
_iq Buffer_I_V[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};		// buffer za stuju faze V
_iq Buffer_I_W[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};		// buffer za stuju faze W

// stavljeno u IQ16 format kako nebi doslo do overflowa

_iq16 N = _IQ(0.25);									// djelitelj za moving average filtar N = 1/4
_iq16 zbroj = 0;										// brojnik za MV filtar	napona
_iq16 zbroj_I_U = 0;									// brojnik za MV filtar struje faze U
_iq16 zbroj_I_V = 0;									// brojnik za MV filtar struje faze V
_iq16 zbroj_I_W = 0;									// brojnik za MV filtar struje faze W

int i_VN = 0, i_NN = 0; 								// broja�i za napone (visoki i niski)
int i_mz = 0, i_vz = 0;									// brojaci za struje (mali i veliki zateg)

int T_mz = 20, T_vz = 1000;								// vrijeme za strju sa malim i velikim zategom
// treba dobiti od ADC jedinice ??????
int vrijeme_VN = 100, vrijeme_NN = 10;					// vrijeme za visoki napon i niski napon

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

	Voltage_VR1=AdcMirror.ADCRESULT0;		// Spremanje rezultata konverzije u globalne varijable
	Voltage_VR2=AdcMirror.ADCRESULT1;
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;		// Reset ADC sequencer 1
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;		// Clear interrupt flag ADC sequencer 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;


	while(1){

		EALLOW;
		GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0b00;
		GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
		EDIS;

		GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;

		// Buffer za napon
		Buffer[3] = Buffer[2];
		Buffer[2] = Buffer[1];
		Buffer[1] = Buffer[0];
		Buffer[0] =  _IQ(Voltage_VR1);

		// buffer za struju faze U
		Buffer_I_U[3] = Buffer_I_U[2];
		Buffer_I_U[2] = Buffer_I_U[1];
		Buffer_I_U[1] = Buffer_I_U[0];
		Buffer_I_U[0] =  _IQ(Voltage_VR2);		// treba dobiti od ADC jedinice sa kojeg registra ocitavamo stuju faze U

		// buffer za struju faze V
		Buffer_I_V[3] = Buffer_I_V[2];
		Buffer_I_V[2] = Buffer_I_V[1];
		Buffer_I_V[1] = Buffer_I_V[0];
		Buffer_I_V[0] =  _IQ(Voltage_VR2);      // treba dobiti od ADC jedinice sa kojeg registra ocitavamo stuju faze V

		// buffer za struju faze W
		Buffer_I_W[3] = Buffer_I_W[2];
		Buffer_I_W[2] = Buffer_I_W[1];
		Buffer_I_W[1] = Buffer_I_W[0];
		Buffer_I_W[0] =  _IQ(Voltage_VR2);      // treba dobiti od ADC jedinice sa kojeg registra ocitavamo stuju faze W

		// Radimo Moving average filter

		// za napon
		zbroj = _IQtoIQ16(Buffer[3]) + _IQtoIQ16(Buffer[2]) + _IQtoIQ16(Buffer[1]) + _IQtoIQ16(Buffer[0]);
		MV_napon = _IQ16toIQ(_IQmpy(zbroj, N));

		// za struju faze U
		zbroj_I_U = _IQtoIQ16(Buffer_I_U[3]) + _IQtoIQ16(Buffer_I_U[2]) + _IQtoIQ16(Buffer_I_U[1]) + _IQtoIQ16(Buffer_I_U[0]);
		MV_struja_U = _IQ16toIQ(_IQmpy(zbroj_I_U, N));

		// za struju faze V
		zbroj_I_V = _IQtoIQ16(Buffer_I_V[3]) + _IQtoIQ16(Buffer_I_V[2]) + _IQtoIQ16(Buffer_I_V[1]) + _IQtoIQ16(Buffer_I_V[0]);
		MV_struja_V = _IQ16toIQ(_IQmpy(zbroj_I_V, N));

		// za struju faze W
		zbroj_I_W = _IQtoIQ16(Buffer_I_W[3]) + _IQtoIQ16(Buffer_I_W[2]) + _IQtoIQ16(Buffer_I_W[1]) + _IQtoIQ16(Buffer_I_W[0]);
		MV_struja_W = _IQ16toIQ(_IQmpy(zbroj_I_W, N));


		// Prenaponska zastita, moramo odrediti grani�nu vrijednost (TH)????? dobiti od ADC jedinice ??????
		if (MV_napon > TH_HV) {
			i_VN += 1;
		}
		if ((MV_napon < TH_HV) && (i_VN > 0)) {
			i_VN -=1;
		}
		if (i_VN >= vrijeme_VN) {
			GpioDataRegs.GPBSET.bit.GPIO34 = 1;
			break;
		}
		// Podnaponska zastita, moramo odrediti grani�nu vrijednost (TH)????? dobiti od ADC jedinice ??????
		if (MV_napon < TH_LV) {
			i_NN += 1;
		}
		if ((MV_napon > TH_LV) && (i_NN > 0)) {
			i_NN -=1;
		}
		if (i_VN >= vrijeme_VN) {
			// signaliziraj da je napon manji od donje granice
			break;
		}

		// za struju faze U
		// Strujna zastita, mali zateg
		if (MV_struja_U > TH_mzI){
			i_mz += 1;
		}
		if ((MV_struja_U < TH_mzI) && (i_mz > 0)){
			i_mz -= 1;
		}
		if (i_mz >= T_mz) {
			// signaliziraj da je struja ve�a od granice za mali zateg
			break;
		}
		// Strujna zastita, veliki zateg
		if (MV_struja_U > TH_vzI){
			i_vz += 1;
		}
		if ((MV_struja_U < TH_vzI) && (i_vz > 0)){
			i_vz -= 1;
		}
		if (i_vz >= T_vz) {
			// signaliziraj da je struja ve�a od granice za veliki zateg
			break;
		}

		// za struju faze V
		// Strujna zastita, mali zateg
		if (MV_struja_V > TH_mzI){
			i_mz += 1;
		}
		if ((MV_struja_V < TH_mzI) && (i_mz > 0)){
			i_mz -= 1;
		}
		if (i_mz >= T_mz) {
			// signaliziraj da je struja ve�a od granice za mali zateg
			break;
		}
		// Strujna zastita, veliki zateg
		if (MV_struja_V > TH_vzI){
			i_vz += 1;
		}
		if ((MV_struja_V < TH_vzI) && (i_vz > 0)){
			i_vz -= 1;
		}
		if (i_vz >= T_vz) {
			// signaliziraj da je struja ve�a od granice za veliki zateg
			break;
		}

		// za struju faze W
		// Strujna zastita, mali zateg
		if (MV_struja_W > TH_mzI){
			i_mz += 1;
		}
		if ((MV_struja_W < TH_mzI) && (i_mz > 0)){
			i_mz -= 1;
		}
		if (i_mz >= T_mz) {
			// signaliziraj da je struja ve�a od granice za mali zateg
			break;
		}
		// Strujna zastita, veliki zateg
		if (MV_struja_W > TH_vzI){
			i_vz += 1;
		}
		if ((MV_struja_W < TH_vzI) && (i_vz > 0)){
			i_vz -= 1;
		}
		if (i_vz >= T_vz) {
			// signaliziraj da je struja ve�a od granice za veliki zateg
			break;
		}
	}
}

