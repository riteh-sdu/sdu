/*
 * adc.h
 *
 *  Created on: 14. lip 2018.
 *      Author: Lorango
 */

#ifndef SDU_HEADERS_ADC_H_
#define SDU_HEADERS_ADC_H_

// deklaracija globalnih varijabli
extern unsigned long Ifb_Ret;
extern unsigned long Ifb_U;
extern unsigned long Ifb_V;
extern unsigned long Vfb_Bus;

extern unsigned long Ifb_Ret_offset;
extern unsigned long Ifb_U_offset;
extern unsigned long Ifb_V_offset;
extern unsigned long Vfb_Bus_offset;

// deklaracija globalnih funkcija
void adc_setup(void);
void adc_loop(void);

void interrupt_setup_adc(void);
interrupt void int_rut(void);

void fun_offset_adc(void);

#endif /* SDU_HEADERS_ADC_H_ */
