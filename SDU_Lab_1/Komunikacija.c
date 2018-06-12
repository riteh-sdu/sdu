#include "DSP2833x_Device.h"


void delay_loop(long);
void SCIA_Slanje(int b);
void SCIA_Poruka(char *msg);


char msg[30]={'n','=','e','=','d','=','q','\n','\0'};

int16 a;

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

		SciaRegs.SCICCR.bit.PARITY=0; //odd
		SciaRegs.SCICCR.bit.PARITYENA=1;
		SciaRegs.SCICCR.bit.SCICHAR=0b111; //8 bita
		SciaRegs.SCICCR.bit.STOPBITS=0; //stop bit
		SciaRegs.SCICCR.bit.ADDRIDLE_MODE=0;
		SciaRegs.SCICCR.bit.LOOPBKENA=0;

		SciaRegs.SCIHBAUD=0x0001;  // 9600 baud
		SciaRegs.SCILBAUD=0x00E7;

		SciaRegs.SCICTL2.bit.TXINTENA=0; //interrupt
		SciaRegs.SCICTL2.bit.RXBKINTENA=0; //interrupt

		SciaRegs.SCICTL1.bit.RXENA=1;
		SciaRegs.SCICTL1.bit.TXENA=1;
		SciaRegs.SCICTL1.bit.SLEEP=0;
		SciaRegs.SCICTL1.bit.SWRESET=0;
		SciaRegs.SCICTL1.bit.RXERRINTENA=0;
		SciaRegs.SCICTL1.all = 0x0023;

		SciaRegs.SCIFFTX.all=0xE000;  //fifo
		SciaRegs.SCIFFRX.all=0x6066;
		SciaRegs.SCIFFCT.all=0x0000;
		SciaRegs.SCICTL1.bit.SWRESET=1;

		SciaRegs.SCIFFRX.bit.RXFFINT=0;
		SciaRegs.SCIFFRX.bit.RXFFINTCLR=0;

		EDIS;

     while (1){

    	 	//SciaRegs.SCITXBUF='H';
    	 	//delay_loop(10000000);
    	 	//SciaRegs.SCITXBUF='A';
    	 	//delay_loop(10000000);

    	 	if(SciaRegs.SCIFFRX.bit.RXFFST>0){  //primanje poruke i spremanje u varijablu a
    	 		a=SciaRegs.SCIRXBUF.bit.RXDT;
    	 		//SciaRegs.SCITXBUF=a;
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
    	 		}
    	 		}

    	 	SCIA_Poruka(msg);                  //slanje poruke
    	 	delay_loop(10000000);

     }

}
void delay_loop (long end)	// Delay_loop funkcija
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
void SCIA_Slanje(int b){
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


