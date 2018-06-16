#include "DSP2833x_Device.h"	// Headeri
//#include "DSP28x_Project.h"
//#include "SDU_main_init.h"
#include "IQmathLib.h"
#define GLOBAL_Q 18
#define PI 3.14159265358979323846;
_iq w=0;
_iq sum=0;
_iq un=0;
_iq ws;
_iq wpids;

void delay_loop2(long);	// Delay funkcija

extern void Setup_ePWM(void);
//extern void InitSysCtrl(void);



void main(void)
{
	// Podesavanje takta dsp-a.
	//DSP_CLOCK_SETUP;
	Setup_ePWM();

	// Omogucavanje Watchdog timer-a.
	//WATCHDOG_TIMER_SETUP;
	float omega=1600;
	sum=_IQ(0);//da suma bude 0 postaviti u nekoj pocetnoj petlji
	wpids=_IQ(0);
	while(1){
		EALLOW;
					SysCtrlRegs.WDKEY = 0x55;
					SysCtrlRegs.WDKEY = 0xAA;
		EDIS;

		_iq ws=_IQmpy(_IQ(omega),_IQdiv(_IQ(3.14159265358979323846),_IQ(30)));   	//pretvorba zadane brzine iz o/min u rad/s
		_iq dw=_IQ(ws)-_IQ(w);     						//error broja okretaja (zadani - trenutni)

		//PI regulator
		_iq sum=sum+_IQ(dw)*_IQ(0.0001);

		if (sum>1){                            			//anti windup limitira vrijednost integrala
			sum=1;
		}
		_iq wpid =_IQmpy(_IQ(2),ws)+_IQmpy(_IQ(2),dw)+_IQmpy(_IQ(20),sum); 	//izlaz PI regulatora u rad/s
		if (wpid>_IQ(376)){
	           wpid=_IQ(376);
	      }
		if (wpid<_IQ(-376)){
		           wpid=_IQ(-376);
		      }

		if(wpid>(wpids+_IQ(0.015))){
			wpid=wpids+_IQ(0.015)	;
		}
		 else if(wpid<(wpids-_IQ(0.015))){
			wpid=wpids-_IQ(0.015);
			}


		wpids=wpid;
		//_iq wpid=_IQ(314);
		// V/f profil
		un = _IQtoF(_IQmpy(_IQ(1.0),wpid));					//amplituda zadanog napona

		_iq wt=wt + _IQmpy(wpid,_IQ(0.0001));


		EPwm1Regs.CMPA.half.CMPA = 10.7*un*_IQtoF(_IQsin(wt))+3500;                 //10.7* un max 3500
		EPwm2Regs.CMPA.half.CMPA = 10.7*un*_IQtoF(_IQsin(wt+_IQ(2.0943951)))+3500;
		EPwm3Regs.CMPA.half.CMPA = 10.7*un*_IQtoF(_IQsin(wt+_IQ(4.1887902)))+3500;


		 if (wt > _IQ(6.283185307179586)) {
		           wt = wt - _IQ(6.283185307179586);
		      }

		long i;
			for (i=0; i<585;i++)  /////100us koristeno za testiranje
			{
				asm("NOP");
				EALLOW;
								SysCtrlRegs.WDKEY = 0x55;
								SysCtrlRegs.WDKEY = 0xAA;
					EDIS;
			}





	}

}
