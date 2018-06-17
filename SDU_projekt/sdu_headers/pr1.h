/*
 * pr1.h
 *
 *  Created on: 17. lip 2018.
 *      Author: Lorango
 */

#ifndef SDU_HEADERS_PR1_H_
#define SDU_HEADERS_PR1_H_

#include "../IQmathLib.h"
extern _iq sum;
extern _iq un;
extern _iq ws;
extern _iq wpids;
extern _iq wpid;

// deklaracija globalnih funkcija
void pr1_setup(void);
void pr1_loop(void);

void pr1_interrupt(void);

#endif /* SDU_HEADERS_PR1_H_ */
