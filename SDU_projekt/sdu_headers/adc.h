/*
 * adc.h
 *
 *  Created on: 14. lip 2018.
 *      Author: Lorango
 */

#ifndef SDU_HEADERS_ADC_H_
#define SDU_HEADERS_ADC_H_

#endif /* SDU_HEADERS_ADC_H_ */

// definicija globalnih varijabli
unsigned long Ifb_Ret;
unsigned long Ifb_U;
unsigned long Ifb_V;
unsigned long Vfb_Bus;

unsigned long Ifb_Ret_offset;
unsigned long Ifb_U_offset;
unsigned long Ifb_V_offset;
unsigned long Vfb_Bus_offset;

// globalne funkcije adc grupe
interrupt void int_rut(void);

void interrupt_setup(void);
void adc_setup(void);
void adc_loop(void);
