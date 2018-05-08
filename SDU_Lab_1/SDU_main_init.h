// Sadrži razne inicijalizacije.

#include "DSP2833x_Device.h"

// Postavljanje frekvencije takta dsp-a.
// 30 MHz * 10 / 2 = 150MHz
#define DSP_CLOCK_SETUP {EALLOW; SysCtrlRegs.PLLCR.bit.DIV = 10;  SysCtrlRegs.PLLSTS.bit.DIVSEL = 2; EDIS;}

// Makro za inicijalizaciju Watchdog timer-a.
#define WATCHDOG_TIMER_SETUP {EALLOW; SysCtrlRegs.WDCR = 0x00AF; EDIS;}

// Makro za resetiranje Watchdog timer-a.
#define WATCHDOG_TIMER_RESET {EALLOW; SysCtrlRegs.WDKEY = 0x55; SysCtrlRegs.WDKEY = 0xAA; EDIS;}
#define WATCHDOG_TIMER_RESET_PH {asm(" NOP");}

