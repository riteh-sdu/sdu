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
	//float omega=1600;
	sum=_IQ(0);//da suma bude 0 postaviti u nekoj pocetnoj petlji
	wpids=_IQ(0);
	while(1){
		//watchdog_timer_reset();







		_iq ws=_IQmpy(omega,_IQdiv(_IQ(3.14159265358979323846),_IQ(30)));   	//pretvorba zadane brzine iz o/min u rad/s
		_iq ww =_IQmpy(spd,_IQdiv(_IQ(3.14159265358979323846),_IQ(30)));   		//pretvorba zadane brzine iz o/min u rad/s
		_iq dw=ws-ww;     													//error broja okretaja (zadani - trenutni)

		//PI regulator
		_iq sum=sum+_IQmpy(dw,_IQ(0.0001));                                                                     //spremati ga kao globalna variabla------------------------------

		if (sum>1){                            			//anti windup limitira vrijednost integrala
			sum=1;
		}
		else if(sum<-1){
			sum=-1;
		}
		_iq wpid =_IQmpy(_IQ(2),ws)+_IQmpy(_IQ(2),dw)+_IQmpy(_IQ(20),sum); 	//izlaz PI regulatora u rad/s
		if (wpid>_IQ(376)){
	           wpid=_IQ(376);
	      }
		else if (wpid<_IQ(-376)){
		           wpid=_IQ(-376);
		      }

		if(wpid>(wpids+_IQ(0.015))){
			wpid=wpids+_IQ(0.015)	;
		}
		else if(wpid<(wpids-_IQ(0.015))){
			wpid=wpids-_IQ(0.015);
			}


		wpids=wpid;                                                                     //spremati ga kao globalna variabla----------------------------------------------
		//_iq wpid=_IQ(314);
		// V/f profil
		//un = _IQtoF(_IQmpy(_IQ(1.0),wpid));					//amplituda zadanog napona racunata poslje

		_iq wt=wt + _IQmpy(wpid,_IQ(0.0001));                                                                        //spremati ga kao globalna variabla----------------------------------


		EPwm1Regs.CMPA.half.CMPA = 10.7*un*_IQtoF(_IQsin(wt))+3500;                 //10.7* un max 3500 (v/f ugraden)
		EPwm2Regs.CMPA.half.CMPA = 10.7*un*_IQtoF(_IQsin(wt+_IQ(2.0943951)))+3500;
		EPwm3Regs.CMPA.half.CMPA = 10.7*un*_IQtoF(_IQsin(wt+_IQ(4.1887902)))+3500;


		if (wt > _IQ(6.283185307179586)) {
			wt = wt - _IQ(6.283185307179586);
		}







	}

}
