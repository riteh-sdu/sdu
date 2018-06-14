/*Brzinu u terminal unositi s + ili - (+1400 ili -1350)
* U slucaju unosa brzine manje od 1000 unositi na slijedeci nacin npr. +0800 ili -900 radi ispravnog punjenja buffera
*/
#include "DSP2833x_Device.h"
#include "stdbool.h"
#ifndef   GLOBAL_Q
#define   GLOBAL_Q       18
#endif
#include "IQmathLib.h"

// Prototipovi funkcija
void delay_loop(long);
void SCIA_Slanje(int b);
void SCIA_Poruka(char *msg);

// Varijable
char msg[30]={'o','m','e','g','a','\n','\0'};
int16 a;
_iq buffer[5];
_iq w;
_iq omega;
int state;

void main(void) {

		EALLOW;
		SysCtrlRegs.WDCR=0x0068;
		SysCtrlRegs.PCLKCR0.bit.SCIAENCLK=1;
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

     while (1){

    	 	if(SciaRegs.SCIFFRX.bit.RXFFST>4){  // Primanje poruke i spremanje u varijablu a

    	 		int z = 0;
    	 		while (SciaRegs.SCIFFRX.bit.RXFFST!=0){

    	 			a=SciaRegs.SCIRXBUF.bit.RXDT;
    	 			buffer[z] = _IQ(a) - _IQ(48);
    	 			z++;
    	 		}

    	 	if(buffer[0]== -786432 || buffer[0]== -1310720){		// Postupak za provjeru pozitivnog i negativnog smjera vrtnje

    	 	buffer[1]=_IQmpy(buffer[1],_IQ(1000));					// Racunanje unesene brzine
    	 	buffer[2]=_IQmpy(buffer[2],_IQ(100));
    	 	buffer[3]=_IQmpy(buffer[3],_IQ(10));
    	 	buffer[4]=_IQmpy(buffer[4],_IQ(1));


    	 	w = buffer[4] + buffer[3] + buffer[2] + buffer[1];		// Varijabla za spremanje unesene brzine s terminala
    	 	}

    	 	if(w <= _IQ(1500)){										// Zastita u slucaju unosa brzine vece od 1500 rpm-a
    	 		if(buffer[0]== -786432){
    	 			omega=_IQmpy(w,_IQ(-1));}
    	 		if(buffer[0]== -1310720){
    	 			omega = w;}										// Varijabla omega gdje je spremljena brzina s + ili - predznakom za daljnje koristenje
    	 				}

    	 		/*Dio za grupu koja je trebala definirati stanja
    	 		 *
    	 		 * SciaRegs.SCITXBUF=a;
    	 		switch (a){
    	 		case '1':
    	 			state=1;
    	 			break;
    	 		case '2':
    	 			state=2;
    	 			break;
    	 		case '3':
    	 			state=3;
    	 			break;
    	 		case '4':
    	 			state=4;
    	 			break;
    	 		}*/
    	 	}

    	 	SCIA_Poruka(msg);                  //	Slanje poruke Herculesu
    	 	delay_loop(10000000);


     }
}
void delay_loop (long end)						// Delay loop funkcija
{
	long i;
	for (i=0; i<end; i++)
	{
	asm( " NOP" );
	EALLOW;
	SysCtrlRegs.WDKEY = 0x55;
	SysCtrlRegs.WDKEY = 0xAA;
	EDIS;
	}
}
void SCIA_Slanje(int b){						// Funkcije za slanje i pakiranje poruke
	while(SciaRegs.SCIFFTX.bit.TXFFST != 0) {}
    SciaRegs.SCITXBUF=b;
}
void SCIA_Poruka(char *msg){
	int i;
	    i = 0;
	    while(msg[i] != '\0')
	    {
	        SCIA_Slanje(msg[i]);
	        i++;
	    }
}


//}


