/*
 * qep.c
 *
 *  Created on: 15. lip 2018.
 *      Author: Lorango
 */

#include "DSP2833x_Device.h"
#include "../IQmathLib.h"

#include "../sdu_headers/init.h"
#include "../sdu_headers/qep.h"

_iq rpm_high;
_iq rpm_low;
_iq test_1;
_iq test_2;
_iq test_3;
_iq test_4;
_iq spd;

/*
 * Funkcija za podešavanje qep-a.
 */
void qep_setup(void)
{
	watchdog_timer_reset();

	EALLOW;
	//GpioCtrlRegs.GPAMUX2.all=0x0000;
	//GpioCtrlRegs.GPAMUX2.bit.GPIO23=1;//eQEP index
	//GpioCtrlRegs.GPAMUX2.bit.GPIO21=1;//eQEP 1B
	//GpioCtrlRegs.GPAMUX2.bit.GPIO20=1;//eQEP qA
	EDIS;

	EQep1Regs.QUPRD=1500000;			// Unit Timer for 100Hz at 150 MHz SYSCLKOUT => Definira vremenski interval T (Svakih 10 ms se broji broj impulsa) s kojim ce se uzorkovati eQEP pozicija i opcionalno generirati prekid


	EQep1Regs.QDECCTL.bit.QSRC=00;		// QEP quadrature count mode

	EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
	EQep1Regs.QEPCTL.bit.PCRM=11;		// PCRM=11 mode => QPOSCNT (Broj izbrojanih impulsa) se resetira svakih 100 ms
	EQep1Regs.QEPCTL.bit.UTE=1; 		// Unit Timeout Enable
	EQep1Regs.QEPCTL.bit.QCLM=1; 		// Latch on unit time out => ( 1- Latch on unit time generated by encoder signals, 0 - latch on position counter read by CPU)
	EQep1Regs.QPOSMAX=0xffffffff;
	EQep1Regs.QEPCTL.bit.QPEN=1; 		// QEP enable

	EQep1Regs.QCAPCTL.bit.UPPS=5;   	// 1/32 for unit position => Number of quadrature periods selected for Time measurement (32xQCLK)
	EQep1Regs.QCAPCTL.bit.CCPS=7;		// 1/128 for CAP clock => eQEP capture timer clock prescale (SYSCLKOUT/128)
	EQep1Regs.QCAPCTL.bit.CEN=1; 		// QEP Capture Enable
}


/*
 * Funkcija qep loop.
 */
void qep_loop(void)
{
	watchdog_timer_reset();

	test_1 =  _IQmpy(_IQ(EQep1Regs.QPOSLAT), _IQ(0.00012));
	rpm_high =	_IQmpy((test_1), _IQ(6000)); //6000 PREDSTAVLJA PREBACIVANJE IZ 10MS U S (*100) I U MINUTE (*60)

	test_3=	_IQmpy(_IQ(0.00021844), _IQ(EQep1Regs.QCPRD));
	rpm_low=_IQdiv(_IQ(60), test_3); //rotations per minute for low speed
	if (rpm_high<_IQ(100) && rpm_high>_IQ(40))
	{
		spd=rpm_low;
	}
	else if(rpm_high<40)
	{
		spd=rpm_high;
	}
	else
	{
		spd=rpm_high;
	}
}

