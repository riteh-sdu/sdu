/*
 * kom.c
 *
 *  Created on: 15. lip 2018.
 *      Author: Lorango
 */

#include "DSP2833x_Device.h"
#include "../IQmathLib.h"

#include "../sdu_headers/init.h"
#include "../sdu_headers/kom.h"

char msg[30]={'o','m','e','g','a','\n','\0'};

//_iq kom_buffer[5];
//_iq w;
//_iq omega;

long kom_buffer[5];
long w;
long omega;

/*
 * Funkcija za podešavanje komunikacije.
 */
void kom_setup(void)
{
	watchdog_timer_reset();

	EALLOW;
	//SysCtrlRegs.WDCR=0x0068;
	SysCtrlRegs.PCLKCR0.bit.SCIAENCLK=1;	// prosljedi takt na komunikaciju
	EDIS;

	EALLOW;
	GpioCtrlRegs.GPAMUX2.all=0x0000;
	GpioCtrlRegs.GPAMUX2.bit.GPIO28=0b1;
	GpioCtrlRegs.GPAMUX2.bit.GPIO29=0b1;
	GpioCtrlRegs.GPADIR.bit.GPIO28=0;
	GpioCtrlRegs.GPADIR.bit.GPIO29=1;

	SciaRegs.SCICCR.bit.PARITY=0; 			//	ODD
	SciaRegs.SCICCR.bit.PARITYENA=1;
	SciaRegs.SCICCR.bit.SCICHAR=0b111; 		//	8 BITA
	SciaRegs.SCICCR.bit.STOPBITS=0; 		//	STOP BIT
	SciaRegs.SCICCR.bit.ADDRIDLE_MODE=0;
	SciaRegs.SCICCR.bit.LOOPBKENA=0;

	SciaRegs.SCIHBAUD=0x0001;  				// 9600 BAUD
	SciaRegs.SCILBAUD=0x00E7;

	SciaRegs.SCICTL2.bit.TXINTENA=0; 		// Interrupt
	SciaRegs.SCICTL2.bit.RXBKINTENA=0; 		// Interrupt

	SciaRegs.SCICTL1.bit.RXENA=1;
	SciaRegs.SCICTL1.bit.TXENA=1;
	SciaRegs.SCICTL1.bit.SLEEP=0;
	SciaRegs.SCICTL1.bit.SWRESET=0;
	SciaRegs.SCICTL1.bit.RXERRINTENA=0;
	SciaRegs.SCICTL1.all = 0x0023;

	SciaRegs.SCIFFTX.all=0xE000;  			// FIFO
	SciaRegs.SCIFFRX.all=0x6066;
	SciaRegs.SCIFFCT.all=0x0000;
	SciaRegs.SCICTL1.bit.SWRESET=1;

	SciaRegs.SCIFFRX.bit.RXFFINT=0;
	SciaRegs.SCIFFRX.bit.RXFFINTCLR=0;

	EDIS;
}

/*
 * Funkcija za koja se izvršava u glavnoj petlji.
 * Primanje poruke i spremanje u varijablu a
 */
void kom_loop(void)
{
	int z = 0;
	int a = 0;

	if(SciaRegs.SCIFFRX.bit.RXFFST>4)
	{
		while (SciaRegs.SCIFFRX.bit.RXFFST!=0)
		{
			a=SciaRegs.SCIRXBUF.bit.RXDT;
			kom_buffer[z] = _IQ(a) - _IQ(48);
			z++;
		}

		// Postupak za provjeru pozitivnog i negativnog smjera vrtnje
		if((kom_buffer[0] == -786432) || (kom_buffer[0] == -1310720))
		{
			kom_buffer[1]=_IQmpy(kom_buffer[1],_IQ(1000));					// Racunanje unesene brzine
			kom_buffer[2]=_IQmpy(kom_buffer[2],_IQ(100));
			kom_buffer[3]=_IQmpy(kom_buffer[3],_IQ(10));
			kom_buffer[4]=_IQmpy(kom_buffer[4],_IQ(1));

			w = kom_buffer[4] + kom_buffer[3] + kom_buffer[2] + kom_buffer[1];		// Varijabla za spremanje unesene brzine s terminala
		}

		if(w <= _IQ(1500))
		{										// Zastita u slucaju unosa brzine vece od 1500 rpm-a
			if(kom_buffer[0]== -786432)
			{
				omega=_IQmpy(w, _IQ(-1));
			}

			if(kom_buffer[0]== -1310720)
			{
				omega = w;
			}										// Varijabla omega gdje je spremljena brzina s + ili - predznakom za daljnje koristenje
		}
	}

	SCIA_Poruka(msg);                  //	Slanje poruke Herculesu
	//delay_loop(10000000);
}

/*
 * Funkcije za slanje i pakiranje poruke
 */
void SCIA_Slanje(int b)
{
	while(SciaRegs.SCIFFTX.bit.TXFFST != 0)
	{
		watchdog_timer_reset();
	}
    SciaRegs.SCITXBUF=b;
}

void SCIA_Poruka(char *msg)
{
	int i = 0;
	while(msg[i] != '\0')
	{
		watchdog_timer_reset();
		SCIA_Slanje(msg[i]);
		i++;
	}
}
