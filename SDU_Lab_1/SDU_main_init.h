// Sadrži razne inicijalizacije.

#include "DSP2833x_Device.h"

#define RESET_WATCHDOG_TIMER {EALLOW;SysCtrlRegs.WDKEY = 0x55;SysCtrlRegs.WDKEY = 0xAA;EDIS;}

