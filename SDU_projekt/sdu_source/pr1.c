/*
 * pr1.c
 *
 *  Created on: 17. lip 2018.
 *      Author: Lorango
 */


#include "DSP2833x_Device.h"
#include "../IQmathLib.h"

#include "../sdu_headers/init.h"
#include "../sdu_headers/qep.h"
#include "../sdu_headers/pr1.h"
#include "../sdu_headers/kom.h"

_iq sum = _IQ(0);
_iq un = _IQ(0);
_iq ws = _IQ(0);
_iq wpids = _IQ(0);
_iq wpid = _IQ(0);
_iq wt = _IQ(0);
_iq dw = _IQ(0);
_iq ww = _IQ(0);
/*
 * Funkcija za podešavanje pr1.
 */
void pr1_setup(void)
{
	// Podesavanje takta dsp-a.
	//DSP_CLOCK_SETUP;
	//Setup_ePWM();

	// Omogucavanje Watchdog timer-a.
	//WATCHDOG_TIMER_SETUP;

	watchdog_timer_reset();

	sum=_IQ(0);//da suma bude 0 postaviti u nekoj pocetnoj petlji
	wpids=_IQ(0);
}

/*
 * Funkcija koja se izvršava u glavoj petlij (pr1).
 */

/*
 * Funkcija koja se izvršava u interruptu pr1.
 */
void pr1_interrupt(void)
{
	watchdog_timer_reset();

	ws=_IQmpy(omega,_IQdiv(_IQ(3.14159265358979323846),_IQ(30)));		//pretvorba zadane brzine iz o/min u rad/s
	ww =_IQmpy(spd,_IQdiv(_IQ(3.14159265358979323846),_IQ(30)));   		//pretvorba zadane brzine iz o/min u rad/s
	dw = ws-ww;     														//error broja okretaja (zadani - trenutni)

	//PI regulator
	sum = sum+_IQmpy(dw,_IQ(0.0001));                                                                     //spremati ga kao globalna variabla------------------------------

	if (sum>1)
	{                            			//anti windup limitira vrijednost integrala
		sum=1;
	}
	else if(sum<-1)
	{
		sum=-1;
	}
	wpid =_IQmpy(_IQ(2),ws)+_IQmpy(_IQ(2),dw)+_IQmpy(_IQ(20),sum); 	//izlaz PI regulatora u rad/s
	if (wpid>_IQ(376))
	{
		   wpid=_IQ(376);
	  }
	else if (wpid<_IQ(-376))
	{
			   wpid=_IQ(-376);
		  }

	if(wpid>(wpids+_IQ(0.015)))
	{
		wpid=wpids+_IQ(0.015)	;
	}
	else if(wpid<(wpids-_IQ(0.015)))
	{
		wpid=wpids-_IQ(0.015);
		}


	wpids=wpid;				//spremati ga kao globalna variabla----------------------------------------------

	wt=wt + _IQmpy(wpid,_IQ(0.0001));                                                                        //spremati ga kao globalna variabla----------------------------------


	//
	//IQtoF(_IQmpy(_IQmpy(_IQ(10.7), wpid), _IQsin(wt))) + 3500;
	///
	/*
	EPwm1Regs.CMPA.half.CMPA = 10.7*wpid*_IQtoF(_IQsin(wt))+3500;                 //10.7* un max 3500 (v/f ugraden)
	EPwm2Regs.CMPA.half.CMPA = 10.7*wpid*_IQtoF(_IQsin(wt+_IQ(2.0943951)))+3500;
	EPwm3Regs.CMPA.half.CMPA = 10.7*wpid*_IQtoF(_IQsin(wt+_IQ(4.1887902)))+3500;
	*/
	EPwm1Regs.CMPA.half.CMPA = _IQtoF(_IQmpy(_IQmpy(_IQ(10.7), wpid), _IQsin(wt))) + 3500;
	EPwm2Regs.CMPA.half.CMPA = _IQtoF(_IQmpy(_IQmpy(_IQ(10.7), wpid), _IQsin(wt+_IQ(2.0943951)))) + 3500;
	EPwm3Regs.CMPA.half.CMPA = _IQtoF(_IQmpy(_IQmpy(_IQ(10.7), wpid), _IQsin(wt+_IQ(4.1887902)))) + 3500;


	if (wt > _IQ(6.283185307179586))
	{
		wt = wt - _IQ(6.283185307179586);
	}
	else if (wt < _IQ(-6.283185307179586))
	{
		wt = wt + _IQ(6.283185307179586);
	}
}
