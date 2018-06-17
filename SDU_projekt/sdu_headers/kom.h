/*
 * kom.h
 *
 *  Created on: 15. lip 2018.
 *      Author: Lorango
 */

#ifndef SDU_HEADERS_KOM_H_
#define SDU_HEADERS_KOM_H_

// deklaracija globalnih varijabli
extern char msg[30];
extern int state;

#include "../IQmathLib.h"
extern _iq kom_buffer[5];
extern _iq w;
extern _iq omega;

// deklaracija globalnih funkcija
void kom_setup(void);
void kom_loop(void);
void kom_loop_2(void);

void SCIA_Slanje(int);
void SCIA_Poruka(char *msg);

#endif /* SDU_HEADERS_KOM_H_ */
