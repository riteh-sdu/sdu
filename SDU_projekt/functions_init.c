/*
 * test.c
 *
 *  Created on: 13. lip 2018.
 *      Author: Lorango
 */

//#include "SDU_main_init.h"
#include "DSP2833x_Device.h"

int fun_1(int x){
	return x + 3;
}

void fun_2(void){
	EALLOW;
	SysCtrlRegs.PLLCR.bit.DIV = 10;
	SysCtrlRegs.PLLSTS.bit.DIVSEL = 2;
	EDIS;
}