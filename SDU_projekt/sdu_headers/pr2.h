/*
 * pr2.h
 *
 *  Created on: 16. lip 2018.
 *      Author: Lorango
 */

#ifndef SDU_HEADERS_PR2_H_
#define SDU_HEADERS_PR2_H_

// deklaracija globalnih varijabli
extern long int i_VN;				// brojaci za napone (visoki i niski)
extern long int i_NN;

extern long int i_mz_pov;			// brojaci za struje (mali zateg)
extern long int i_mz_U;
extern long int i_mz_V;

extern long int i_vz_pov;			// brojaci za struje (veliki zateg)
extern long int i_vz_U;
extern long int i_vz_V;

extern int T_mz;					// vrijeme za strju sa malim i velikim zategom (2ms) i (100ms)
extern long int T_vz;

extern long int vrijeme_VN;		// vrijeme za visoki napon i niski napon (100ms)
extern long int vrijeme_NN;

extern int test_VN;					// detekcija visokog mapona
extern int test_MN;					// detekcija niskog napona

extern int test_Imz_U;				// detekcija struje sa malim zategom
extern int test_Imz_V;
extern int test_Imz_pov;

extern int test_Ivz_U;				// detekcija struja sa velikim zategom
extern int test_Ivz_V;
extern int test_Ivz_pov;

// deklaracija globalnih funkcija
void pr2_setup(void);
void pr2_loop(void);

void pr2_interrupt(void);

#endif /* SDU_HEADERS_PR2_H_ */
