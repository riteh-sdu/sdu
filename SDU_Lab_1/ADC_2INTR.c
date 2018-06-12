/*
 * ADC_1.c
 *
 *  Created on: 10. svi 2018.
 *      Author: Daniel
 */

#include "DSP28x_Project.h"
#include "DSP2833x_Device.h"
#include "IQmathLib.h"
#define GLOBAL_Q18

#define RED 600
#define FED 600
#define PERIOD_REGISTER 7500 // 10KHz - PWM signal

unsigned long Ifb_Ret;
unsigned long Ifb_U;
unsigned long Ifb_V;
unsigned long Vfb_Bus;

unsigned long Ifb_Ret_offset;
unsigned long Ifb_U_offset;
unsigned long Ifb_V_offset;
unsigned long Vfb_Bus_offset;

void Setup_ePWM(void);
interrupt void int_rut(void);

//naš dio interupt 2

_iq Vfb_Bus_offs = _IQ(257);

_iq TH_HV = _IQ(2000);										// granicna vrijednost za visoki napon
_iq TH_LV = _IQ(1000);										// granicna vrijednost za niski napon
_iq TH_mzI = _IQ(200);										// granicna vrijednost za mali zateg struje
_iq TH_vzI = _IQ(100);										// granicna vrijednost za veliki zateg struje

_iq MV_napon = _IQ(0);										// vrijednost napona iz moving average filtra
_iq MV_struja_I_pov = _IQ(0);								// vrijednost struje faze W iz moving average filtra
_iq MV_struja_U = _IQ(0);									// vrijednost struje faze U iz moving average filtra
_iq MV_struja_V = _IQ(0);									// vrijednost struje faze V iz moving average filtra

_iq Buffer_Vfb[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)}; 			// buffer za napon
_iq Buffer_I_pov[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};		// buffer za stuju
_iq Buffer_I_U[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};			// buffer za stuju faze U
_iq Buffer_I_V[4] = {_IQ(0),_IQ(0),_IQ(0),_IQ(0)};			// buffer za stuju faze V

_iq16 N = _IQ(0.25);										// djelitelj za moving average filtar N = 1/4
_iq16 zbroj_Vfb = _IQ(0);									// brojnik za MV filtar	napona
_iq16 zbroj_I_pov = _IQ(0);									// brojnik za MV filtar povratne struje
_iq16 zbroj_I_U = _IQ(0);									// brojnik za MV filtar struje faze U
_iq16 zbroj_I_V = _IQ(0);									// brojnik za MV filtar struje faze V

int i_VN = 0, i_NN = 0; 									// brojaèi za napone (visoki i niski)
int i_mz = 0, i_vz = 0;										// brojaci za struje (mali i veliki zateg)

int T_mz = 20, T_vz = 1000;									// vrijeme za strju sa malim i velikim zategom
// treba dobiti od ADC jedinice ??????
long int vrijeme_VN = 1000, vrijeme_NN = 100;				// vrijeme za visoki napon i niski napon

int test_VN = 0, test_MN = 0;								// detekcija visokog i niskog napona
int test_Imz_U = 0; test_Imz_V = 0; test_Imz_W = 0;			// detekcija struje sa malim zategom
int test_Ivz_U = 0; test_Ivz_V = 0; test_Ivz_W = 0;			// detekcija struja sa velikim zategom


void Interupt_2(void){
	// Buffer za napon
	Buffer_Vfb[3] = Buffer_Vfb[2];
	Buffer_Vfb[2] = Buffer_Vfb[1];
	Buffer_Vfb[1] = Buffer_Vfb[0];
	Buffer_Vfb[0] =  _IQ(Vfb_Bus)- Vfb_Bus_offs;

	// buffer za povratnu struju
	Buffer_I_pov[3] = Buffer_I_pov[2];
	Buffer_I_pov[2] = Buffer_I_pov[1];
	Buffer_I_pov[1] = Buffer_I_pov[0];
	Buffer_I_pov[0] =  _IQ(Ifb_Ret-Ifb_Ret_offset);

	// buffer za struju faze U
	Buffer_I_U[3] = Buffer_I_U[2];
	Buffer_I_U[2] = Buffer_I_U[1];
	Buffer_I_U[1] = Buffer_I_U[0];
	Buffer_I_U[0] =  _IQ(Ifb_U-Ifb_U_offset);

	// buffer za struju faze V
	Buffer_I_V[3] = Buffer_I_V[2];
	Buffer_I_V[2] = Buffer_I_V[1];
	Buffer_I_V[1] = Buffer_I_V[0];
	Buffer_I_V[0] =  _IQ(Ifb_V-Ifb_V_offset);

	// Radimo Moving average filter

	// za napon DC linka
	zbroj_Vfb = _IQtoIQ16(Buffer_Vfb[3]) + _IQtoIQ16(Buffer_Vfb[2]) + _IQtoIQ16(Buffer_Vfb[1]) + _IQtoIQ16(Buffer_Vfb[0]);
	MV_napon = _IQ16toIQ(_IQmpy(zbroj_Vfb, N));

	// za povratnu struju
	zbroj_I_pov = _IQtoIQ16(Buffer_I_pov[3]) + _IQtoIQ16(Buffer_I_pov[2]) + _IQtoIQ16(Buffer_I_pov[1]) + _IQtoIQ16(Buffer_I_pov[0]);
	MV_struja_I_pov = _IQ16toIQ(_IQmpy(zbroj_I_pov, N));

	// za struju faze U
	zbroj_I_U = _IQtoIQ16(Buffer_I_U[3]) + _IQtoIQ16(Buffer_I_U[2]) + _IQtoIQ16(Buffer_I_U[1]) + _IQtoIQ16(Buffer_I_U[0]);
	MV_struja_U = _IQ16toIQ(_IQmpy(zbroj_I_U, N));

	// za struju faze V
	zbroj_I_V = _IQtoIQ16(Buffer_I_V[3]) + _IQtoIQ16(Buffer_I_V[2]) + _IQtoIQ16(Buffer_I_V[1]) + _IQtoIQ16(Buffer_I_V[0]);
	MV_struja_V = _IQ16toIQ(_IQmpy(zbroj_I_V, N));

	// minimalna vrijednost napona DC linka
	if (MV_napon < Vfb_Bus_offs){
		MV_napon = _IQ(0);								// pitati profesora za vrijednost ispod offseta
	}
	// minimalna vrijednost povratne struje
	if (MV_struja_I_pov < _IQ(Ifb_Ret_offset)){
		MV_struja_I_pov = _IQ(0);
	}
	// minimalna vrijednost struje faze U
	if (MV_struja_U < _IQ(Ifb_U_offset)){
		MV_struja_U = _IQ(0);
	}
	// minimalna vrijednost struje faze V
	if (MV_struja_V < _IQ(Ifb_V_offset)){
		MV_struja_V = _IQ(0);
	}

	// Prenaponska zastita, moramo odrediti graniènu vrijednost (TH)????? dobiti od ADC jedinice ??????
	if (MV_napon <= TH_HV){
		test_VN = 0;
		if (i_VN > 0) {
			i_VN -=1;
		}
	}
	else{
		i_VN += 1;
	}
	if (i_VN >= vrijeme_VN) {
		// signaliziraj da je napon veci od gornje granice
		test_VN = 1;
	}

	// Podnaponska zastita
	if (MV_napon > TH_LV){
		test_MN = 0;
		if (i_NN > 0){
			i_NN -= 1;
		}
	}
	else{
		i_NN += 1;
	}
	if (i_NN > vrijeme_VN){
		// signaliziraj da je napon manji od donje granice
		test_MN = 1;
	}

	// NASTAVAK SA STRUJOM


}


void main(void)
{
    InitSysCtrl();
    Setup_ePWM();

    EALLOW;
    SysCtrlRegs.PLLCR.bit.DIV = 0b1010;     // CLKIN (30MHz) * 10 = 300MHz
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 0b10;   // 10b (/2) -> 300MHz/2 = 150MHz
    SysCtrlRegs.HISPCP.bit.HSPCLK = 0b000;  // 000b (/1) -> HSPLCK = 150MHz
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0b1; // ADCENCLK enable

    // Podesavanje ADC registara
    // ADC Control Register 1
    AdcRegs.ADCTRL1.all = 0;
    AdcRegs.ADCTRL1.bit.RESET = 0b0;    // Reset 0 = no effect
    AdcRegs.ADCTRL1.bit.SUSMOD = 0b00;  // Free run
    AdcRegs.ADCTRL1.bit.ACQ_PS = 0b000; // Acquisition time prescale
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
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0b0001;   // ADC-A1 Vfb-Bus

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

    DELAY_US(1000);

    // Izracun offset vrijednosti
    int i;
    for(i = 0; i < 1024; i++)
    {
        Ifb_Ret_offset = AdcMirror.ADCRESULT0 + Ifb_Ret_offset;
        Ifb_U_offset = AdcMirror.ADCRESULT1 + Ifb_U_offset;
        Ifb_V_offset = AdcMirror.ADCRESULT3 + Ifb_V_offset;
        Vfb_Bus_offset = AdcMirror.ADCRESULT4 + Vfb_Bus_offset;
        DELAY_US(100);
    }

    // Izracun offset vrijednosti
    Ifb_Ret_offset = Ifb_Ret_offset >> 10;
    Ifb_U_offset = Ifb_U_offset >> 10;
    Ifb_V_offset = Ifb_V_offset >> 10;
    Vfb_Bus_offset = Vfb_Bus_offset >> 10;

/*
    Ifb_Ret = AdcMirror.ADCRESULT0;
    Ifb_U = AdcMirror.ADCRESULT1;
    Ifb_V = AdcMirror.ADCRESULT3;
    Vfb_Bus = AdcMirror.ADCRESULT4;
*/
    while(1)
    {
        Ifb_Ret = AdcMirror.ADCRESULT0;
        Ifb_U = AdcMirror.ADCRESULT1;
        Ifb_V = AdcMirror.ADCRESULT3;
        Vfb_Bus = AdcMirror.ADCRESULT4;
    }
}

interrupt void int_rut(void)
    {
	Interupt_2();
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;     // Clear Interrupt Flag ADC Sequencer 1
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0b1;     // Immediate reset SEQ1 to "initial state"
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;        // PIE Interrupt Acknowledge Register
    }

// PWM SETUP POCETAK
void Setup_ePWM(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.all = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // ePWM1A active
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1; // ePWM2A active
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1; // ePWM3A activ
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0b01;  //Trip Zone TZ1
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0; // Pull up resistor  TZ6

    //GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0b11;  //Trip Zone  TZ6
    //GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0; // Pull up resistor  TZ6

    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;

    SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1; // ENCLK ePWM
    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; // ENCLK ePWM
    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; // ENCLK ePWM
    EDIS;

    EPwm1Regs.TBCTL.bit.CLKDIV =  0;    // CLKDIV = 1
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0;  // HSPCLKDIV = 1
    EPwm1Regs.TBCTL.bit.CTRMODE = 2;    // up - down mode
    EPwm1Regs.AQCTLA.bit.CAU = 0b01;
    EPwm1Regs.AQCTLA.bit.CAD = 0b10;

    EPwm1Regs.TBPRD = PERIOD_REGISTER;  // 10KHz - PWM signal

    EPwm1Regs.DBCTL.bit.OUT_MODE = 0b11; // enable Dead-band module
    EPwm1Regs.DBCTL.bit.POLSEL = 0b10;
    EPwm1Regs.DBFED = FED; // FED
    EPwm1Regs.DBRED = RED; // RED

    EPwm2Regs.TBCTL.bit.CLKDIV =  0;    // CLKDIV = 1
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0;  // HSPCLKDIV = 1
    EPwm2Regs.TBCTL.bit.CTRMODE = 2;    // up - down mode
    EPwm2Regs.AQCTLA.bit.CAU = 0b01;
    EPwm2Regs.AQCTLA.bit.CAD = 0b10;
    EPwm2Regs.TBPRD = PERIOD_REGISTER;          // 10KHz - PWM signal

    EPwm2Regs.DBCTL.bit.OUT_MODE = 0b11; // enable Dead-band module
    EPwm2Regs.DBCTL.bit.POLSEL = 0b10;
    EPwm2Regs.DBFED = FED; // FED
    EPwm2Regs.DBRED = RED; // RED

    EPwm3Regs.TBCTL.bit.CLKDIV =  0;    // CLKDIV = 1
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0;  // HSPCLKDIV = 1
    EPwm3Regs.TBCTL.bit.CTRMODE = 2;    // up - down mode
    EPwm3Regs.AQCTLA.bit.CAU = 0b01;
    EPwm3Regs.AQCTLA.bit.CAD = 0b10;
    EPwm3Regs.TBPRD = PERIOD_REGISTER;      // 10KHz - PWM signal

    EPwm3Regs.DBCTL.bit.OUT_MODE = 0b11; // enable Dead-band module
    EPwm3Regs.DBCTL.bit.POLSEL = 0b10;
    EPwm3Regs.DBFED = FED; // FED
    EPwm3Regs.DBRED = RED; // RED

    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;   // generate a syncout if CTR = 0
    EPwm1Regs.TBPHS.half.TBPHS = 0;

    EPwm2Regs.TBCTL.bit.PHSEN =1;       // enable phase shift for ePWM2
    EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;   // syncin = syncout
    EPwm2Regs.TBPHS.half.TBPHS = 0;

    EPwm3Regs.TBCTL.bit.PHSEN = 1;      // enable phase shift for ePWM2
    EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;   // syncin = syncout
    EPwm3Regs.TBPHS.half.TBPHS =0;

    EALLOW;
    EPwm1Regs.TZCTL.bit.TZA = 0b10;     // force ePWM1A to zero
    EPwm1Regs.TZCTL.bit.TZB = 0b10;     // force ePWM1B to zero

    EPwm2Regs.TZCTL.bit.TZA = 0b10;     // force ePWM2A to zero
    EPwm2Regs.TZCTL.bit.TZB = 0b10;     // force ePWM2B to zero

    EPwm3Regs.TZCTL.bit.TZA = 0b10;     // force ePWM3A to zero
    EPwm3Regs.TZCTL.bit.TZB = 0b10;     // force ePWM3B to zero

    EPwm1Regs.TZSEL.bit.OSHT6 = 1;  // select TZ6 as one shot over current source
    EPwm2Regs.TZSEL.bit.OSHT6 = 1;  // select TZ6 as one shot over current source
    EPwm3Regs.TZSEL.bit.OSHT6 = 1;  // select TZ6 as one shot over current source

    EPwm1Regs.TZCLR.all = 0x0004U;
    EPwm2Regs.TZCLR.all = 0x0004U;
    EPwm3Regs.TZCLR.all = 0x0004U;
    EDIS;

    /*EPwm1Regs.ETSEL.all = 0;
    EPwm1Regs.ETSEL.bit.INTEN = 1;      // interrupt enable for ePWM1
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;     // Enable event trigger
    EPwm1Regs.ETSEL.bit.SOCASEL = 0b010;    // or 0b001
    */

    EPwm1Regs.ETSEL.all = 0;
    EPwm1Regs.ETSEL.bit.INTEN = 0b0;  // interrupt enable for ePWM1
    EPwm1Regs.ETSEL.bit.SOCAEN = 0b1;
    EPwm1Regs.ETSEL.bit.SOCASEL = 0b010;    // or 0b001
    EPwm1Regs.ETPS.bit.SOCAPRD    = 0b1;

    EPwm1Regs.CMPA.half.CMPA = 0.5*7500;
    EPwm2Regs.CMPA.half.CMPA = 0.5*7500;
    EPwm3Regs.CMPA.half.CMPA = 0.5*7500;
}

