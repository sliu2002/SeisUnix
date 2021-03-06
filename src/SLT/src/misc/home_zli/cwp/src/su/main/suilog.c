/* SUILOG: $Revision: 1.3 $ ; $Date: 90/11/15 10:43:30 $	*/

/*----------------------------------------------------------------------
 * Copyright (c) Colorado School of Mines, 1990.
 * All rights reserved.
 *
 * This code is part of SU.  SU stands for Seismic Unix, a processing line
 * developed at the Colorado School of Mines, partially based on Stanford
 * Exploration Project (SEP) software.  Inquiries should be addressed to:
 *
 *  Jack K. Cohen, Center for Wave Phenomena, Colorado School of Mines,
 *  Golden, CO 80401  (jkc@dix.mines.colorado)
 *----------------------------------------------------------------------
 */

#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc = "\
								\n\
SUILOG -- time axis inverse log-stretch of seismic traces	\n\
								\n\
suilog nt= ntmin=  <stdin >stdout 				\n\
								\n\
Required parameters:						\n\
	nt= 	nt output from sulog prog			\n\
	ntmin= 	ntmin output from sulog prog			\n\
	dt= 	dt output from sulog prog			\n\
Optional parameters:						\n\
	none							\n\
								\n\
NOTE:  Parameters needed by suilog to reconstruct the 		\n\
       original data may be input via a parameter file.		\n\
								\n\
EXAMPLE PROCESSING SEQUENCE:					\n\
		sulog outpar=logpar <data1 >data2		\n\
		suilog par=logpar <data2 >data3			\n\
								\n\
	where logpar is the parameter file			\n\
								\n\
";
/**************** end self doc ***********************************/

/* Credits:
 *	CWP: Shuki, Chris
 *
 * Caveats:
 * 	amplitudes are not well preserved
 */


segy tr;

main(int argc, char **argv)
{
	float *buf;	/* temp repository of unstretched data		*/
	float dt;	/* sample rate on original data			*/
	float dtau;	/* sample rate on stretched data		*/
	float *tau;	/* fractional sample number on input data	*/
	float *w;	/* Interpolation weights 			*/
	int it;		/* output time sample counter			*/
	int *itau;	/* ????						*/
	int nt;		/* samples in reconstructed outdata		*/
	uint ntsize;	/*  ... in bytes				*/
	int ntau;	/* samples in input log-stretch data		*/
	int ntmin;	/* minumum time of interest (from par)		*/
	int nw;		/* Number of interpolation weights (2=linear)	*/
	void stretch();
	void lintrp();


	/* Initialize */
	initargs(argc, argv);
	askdoc(1);


	/* Get information from the first header */
	if (!gettr(&tr)) err("can't get first trace");
	ntau = tr.ns;
	dtau = (float)tr.dt/1000000.0;

	/* Must get nt and ntmin for reconstruction of t-data */
	MUSTIGETPAR("nt", &nt); 
	MUSTIGETPAR("ntmin", &ntmin); 
	MUSTFGETPAR("dt", &dt); 
	ntsize = nt * FSIZE;

	/* 2 weights for linear interpolation */
	nw = 2;
	
	/* Allocate space for stretching operation */
	tau = ealloc1float(nt);
	itau = ealloc1int(nt);
	w = ealloc1float(nw * nt);
	buf = ealloc1float(nt);


/* 	The inverse log-stretch from 'tau' to 't' is given mathematically by  	
 *  								
 * 		t = tmin exp (tau + taumin)            
 *  							
 * 	taumin is arbitrary and taken to be taumin=0	
 */

	/* Calculate fractional tau-sample that each t-sample maps to; */
	/* [ it=itmin --> 0 ..&.. it=nt --> ntau ]  */
	for (it = 0; it < ntmin; it++) {
		tau[it] = -1;	/* neg flag for zero value */
	}
	for (it = ntmin; it < nt; it++) {
		tau[it] = log((float) it/ntmin) / dtau;
	}

	/* Calculate the interpolation coefficients */
	lintrp (tau, w, itau, ntau, nt);

	/* Main loop over traces */
	do {
		/* Perform the stretch; put new data into buf */
		stretch (buf, tr.data, w, itau, nt, nw);

		/* Overwrite the segy data */
		memcpy((char*)tr.data, (char*)buf, ntsize);

		tr.ns = nt;
		tr.dt = dt;

		puttr(&tr);

	} while ( gettr(&tr) );


	return EXIT_SUCCESS;
}

void stretch(float *q, float *p, float *w, int *it, int lq, int nw)
/*
 *  General coordinate stretch with predetermined coefficients
 *
 *         NW-1
 * Q(T) =  SUM W(T,J)*P(IT(T)), FOR T=0,LQ-1
 *         J=0
 */
{
	int j, i;

	for (i = 0; i < lq; i++) {
		q[i] = 0.0;
		for (j = 0; j < nw; j++) {
			q[i] += w[i*nw+j] * p[it[i]+j];
		}
	}
	return;
}

void lintrp (float *q, float *w, int *it, int lp, int lq)
{
	int i;
	float delta;

	for (i = 0; i < lq; i++) {
		if (q[i] >= 0.0 && q[i] < lp - 1) {
			it[i] = q[i]; 
			delta = q[i] - it[i];
			w[i*2] = 1.0 - delta;
			w[i*2+1] = delta;
		} else {
			it[i] = 0;
			w[i*2] = 0.0;
			w[i*2+1] = 0.0;
		}
	}
	return;
}
