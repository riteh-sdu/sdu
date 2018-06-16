/*
 * pr2.c
 *
 *  Created on: 16. lip 2018.
 *      Author: Lorango
 */

#include "DSP2833x_Device.h"
#include "../IQmathLib.h"

#include "../sdu_headers/init.h"
#include "../sdu_headers/adc.h"
#include "../sdu_headers/pr2.h"

long int i_VN = 0;				// brojaci za napone (visoki i niski)
long int i_NN = 0;

long int i_mz_pov = 0;			// brojaci za struje (mali zateg)
long int i_mz_U = 0;
long int i_mz_V = 0;

long int i_vz_pov = 0;			// brojaci za struje (veliki zateg)
long int i_vz_U = 0;
long int i_vz_V = 0;

int T_mz = 20;					// vrijeme za strju sa malim i velikim zategom (2ms) i (100ms)
long int T_vz = 1000;

long int vrijeme_VN = 1000;		// vrijeme za visoki napon i niski napon (100ms)
long int vrijeme_NN = 1000;

int test_VN = 0;					// detekcija visokog mapona
int test_MN = 0;					// detekcija niskog napona

int test_Imz_U = 0;				// detekcija struje sa malim zategom
int test_Imz_V = 0;
int test_Imz_pov = 0;

int test_Ivz_U = 0;				// detekcija struja sa velikim zategom
int test_Ivz_V = 0;
int test_Ivz_pov = 0;

_iq U;														// Volt
_iq UgV = _IQ(65);											// granicni visoki napon	(65V)
_iq UgN = _IQ(55);											// granicni niski napon		(55V)

_iq Ipov,IU,IV;
_iq Ig_mz = _IQ(6);											// granicna struja za mali zateg 	(6A)
_iq Ig_vz = _IQ(5);											// granicna struja za veliki zateg 	(5A)

_iq MV_napon = _IQ(0);										// vrijednost napona iz moving average filtra
_iq MV_struja_I_pov = _IQ(0);								// vrijednost struje faze W iz moving average filtra
_iq MV_struja_U = _IQ(0);									// vrijednost struje faze U iz moving average filtra
_iq MV_struja_V = _IQ(0);									// vrijednost struje faze V iz moving average filtra

_iq Buffer_Vfb[4] = {_IQ(0), _IQ(0), _IQ(0), _IQ(0)}; 			// buffer za napon
_iq Buffer_I_pov[4] = {_IQ(0), _IQ(0), _IQ(0), _IQ(0)};		// buffer za stuju
_iq Buffer_I_U[4] = {_IQ(0), _IQ(0), _IQ(0), _IQ(0)};			// buffer za stuju faze U
_iq Buffer_I_V[4] = {_IQ(0), _IQ(0), _IQ(0), _IQ(0)};			// buffer za stuju faze V

_iq16 N = _IQ(0.25);										// djelitelj za moving average filtar N = 1/4
_iq16 zbroj_Vfb = _IQ(0);									// brojnik za MV filtar	napona
_iq16 zbroj_I_pov = _IQ(0);									// brojnik za MV filtar povratne struje
_iq16 zbroj_I_U = _IQ(0);									// brojnik za MV filtar struje faze U
_iq16 zbroj_I_V = _IQ(0);									// brojnik za MV filtar struje faze V


/*
 * Funkcija ka se poziva u prekidnoj rutini adc-a.
 */
void pr2_interrupt(void){
	watchdog_timer_reset();

	// Bufferi 							------------------------------------------------------

	// Buffer za napon
	Buffer_Vfb[3] = Buffer_Vfb[2];
	Buffer_Vfb[2] = Buffer_Vfb[1];
	Buffer_Vfb[1] = Buffer_Vfb[0];
	Buffer_Vfb[0] =  _IQ(Vfb_Bus) - _IQ(Vfb_Bus_offset);

	// buffer za povratnu struju
	Buffer_I_pov[3] = Buffer_I_pov[2];
	Buffer_I_pov[2] = Buffer_I_pov[1];
	Buffer_I_pov[1] = Buffer_I_pov[0];
	Buffer_I_pov[0] =  _IQ(Ifb_Ret) - _IQ(Ifb_Ret_offset);

	// buffer za struju faze U
	Buffer_I_U[3] = Buffer_I_U[2];
	Buffer_I_U[2] = Buffer_I_U[1];
	Buffer_I_U[1] = Buffer_I_U[0];
	Buffer_I_U[0] =  _IQ(Ifb_U) - _IQ(Ifb_U_offset);

	// buffer za struju faze V
	Buffer_I_V[3] = Buffer_I_V[2];
	Buffer_I_V[2] = Buffer_I_V[1];
	Buffer_I_V[1] = Buffer_I_V[0];
	Buffer_I_V[0] =  _IQ(Ifb_V) - _IQ(Ifb_V_offset);

	// Radimo Moving average filter 	------------------------------------------------------

	// za napon DC linka
	zbroj_Vfb = _IQtoIQ16(Buffer_Vfb[3]) + _IQtoIQ16(Buffer_Vfb[2]) + _IQtoIQ16(Buffer_Vfb[1]) + _IQtoIQ16(Buffer_Vfb[0]);
	MV_napon = _IQ16toIQ(_IQmpy(zbroj_Vfb, N));

	// za povratnu struju
	zbroj_I_pov = _IQtoIQ16(Buffer_I_pov[3]) + _IQtoIQ16(Buffer_I_pov[2]) + _IQtoIQ16(Buffer_I_pov[1]) + _IQtoIQ16(Buffer_I_pov[0]);
	MV_struja_I_pov = _IQ16toIQ(_IQmpy(zbroj_I_pov, N));

	// za struju faze U
	zbroj_I_U = _IQtoIQ16(Buffer_I_U[3]) + _IQtoIQ16(Buffer_I_U[2]) + _IQtoIQ16(Buffer_I_U[1]) + _IQtoIQ16(Buffer_I_U[0]);
	MV_struja_U = _IQ16toIQ(_IQmpy(zbroj_I_U, N));

	// za struju faze V
	zbroj_I_V = _IQtoIQ16(Buffer_I_V[3]) + _IQtoIQ16(Buffer_I_V[2]) + _IQtoIQ16(Buffer_I_V[1]) + _IQtoIQ16(Buffer_I_V[0]);
	MV_struja_V = _IQ16toIQ(_IQmpy(zbroj_I_V, N));

	U = _IQmpy(MV_napon,_IQ(0.000732));
	U = _IQmpy(U,_IQ(124.213));

	Ipov = _IQmpy(MV_struja_I_pov,_IQ(0.00915));
	Ipov = Ipov - _IQ(18.75);

	IU = _IQmpy(MV_struja_U,_IQ(0.00915));
	IU = IU - _IQ(18.75);

	IV = _IQmpy(MV_struja_V,_IQ(0.00915));
	IV = IV - _IQ(18.75);

	// minimalna vrijednost napona DC linka
	if (U < 0)
	{
		U = _IQmpy(U, _IQ(-1));
	}

	// minimalna vrijednost povratne struje
	if (Ipov < 0)
	{
		Ipov = _IQmpy(Ipov, _IQ(-1));
	}

	// minimalna vrijednost struje faze U
	if (IU < 0)
	{
		IU = _IQmpy(IU, _IQ(-1));
	}

	// minimalna vrijednost struje faze V
	if (IV < 0){
		IV = _IQmpy(IV, _IQ(-1));
	}

	// Prenaponska zastita 		------------------------------------------------------
	if (U <= UgV)
	{
		test_VN = 0;
		if (i_VN > 0)
		{
			i_VN -= 1;
			if (i_VN < 0)
			{
				i_VN = 0;
			}
		}
	}
	else
	{
		i_VN += 1;
	}

	if (i_VN >= vrijeme_VN)
	{
		test_VN = 1;			// signaliziraj da je napon veci od gornje granice
	}

	// Podnaponska zastita 		------------------------------------------------------
	if (U > UgN)
	{
		test_MN = 0;
		if (i_NN > 0)
		{
			i_NN -= 1;
			if (i_NN < 0)
			{
				i_NN = 0;
			}
		}
	}
	else
	{
		i_NN += 1;
	}
	if (i_NN > vrijeme_VN)
	{
		test_MN = 1;			// signaliziraj da je napon manji od donje granice
	}

	// Strujna zastita mali zateg ------------------------------------------------------

	// Povratna struja
	if (Ipov <= Ig_mz)
	{
		test_Imz_pov = 0;
		if (i_mz_pov > 0)
		{
			i_mz_pov -= 1;
			if (i_mz_pov < 0)
			{
				i_mz_pov = 0;
			}
		}
	}
	else
	{
		i_mz_pov += 1;
	}

	if (i_mz_pov >= T_mz)
	{
		test_Imz_pov = 1;		// signaliziraj da je struja veca od granice za mali zateg
	}

	// Struja faze U
	if (IU < Ig_mz)
	{
		test_Imz_U = 0;
		if (i_mz_U > 0)
		{
			i_mz_U -= 1;
			if (i_mz_U < 0)
			{
				i_mz_U = 0;
			}
		}
	}
	else
	{
		i_mz_U += 1;
	}

	if (i_mz_U >= T_mz)
	{
		test_Imz_U = 1;			// signaliziraj da je struja veca od granice za mali zateg
	}

	// Struja faze V
	if (IV < Ig_mz)
	{
		test_Imz_V = 0;
		if (i_mz_V > 0)
		{
			i_mz_V -= 1;
			if (i_mz_V < 0)
			{
				i_mz_V = 0;
			}
		}
	}
	else
	{
		i_mz_V += 1;
	}

	if (i_mz_V >= T_mz)
	{
		test_Imz_V = 1;			// signaliziraj da je struja veca od granice za mali zateg
	}

	// Strujna zastita veliki zateg ------------------------------------------------------

	// Povratna struja
	if (Ipov < Ig_vz)
	{
		test_Ivz_pov = 0;
		if (i_vz_pov > 0)
		{
			i_vz_pov -= 1;
			if (i_vz_pov < 0)
			{
				i_vz_pov = 0;
			}
		}
	}
	else
	{
		i_vz_pov += 1;
	}
	if (i_vz_pov >= T_vz)
	{
		test_Ivz_pov = 1;		// signaliziraj da je struja veca od granice za veliki zateg
	}

	// Struja faze U
	if (IU < Ig_vz)
	{
		test_Ivz_U = 0;
		if (i_vz_U > 0)
		{
			i_vz_U -= 1;
			if (i_vz_U < 0)
			{
				i_vz_U = 0;
			}
		}
	}
	else
	{
		i_vz_U += 1;
	}

	if (i_vz_U >= T_vz)
	{
		test_Ivz_U = 1;			// signaliziraj da je struja veca od granice za veliki zateg
	}

	// Struja faze U
	if (IV < Ig_vz)
	{
		test_Ivz_V = 0;
		if (i_vz_V > 0)
		{
			i_vz_V -= 1;
			if (i_vz_V < 0)
			{
				i_vz_V = 0;
			}
		}
	}
	else
	{
		i_vz_V += 1;
	}

	if (i_vz_V >= T_vz)
	{
		test_Ivz_V = 1;			// signaliziraj da je struja veca od granice za veliki zateg
	}
}





