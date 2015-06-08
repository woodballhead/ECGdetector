#include "stdafx.h"
#include <memory.h>		/* For memmov. */
#include <tchar.h>
#include <math.h>
#include "qrsdet.h"
#define PRE_BLANK	MS200

// External Prototypes.

int QRSFilter(int datum, int init) ;
int deriv1( int x0, int init ) ;

// Local Prototypes.

int Peak( int datum, int init ) ;
int median(int *array, int datnum) ;
int thresh(int qmedian, int nmedian) ;
int BLSCheck(int *dBuf,int dbPtr,int *maxder) ;

int earlyThresh(int qmedian, int nmedian) ;


double TH = 0.475  ;

int DDBuffer[DER_DELAY], DDPtr ;	/* Buffer holding derivative data. */
int Dly  = 0 ;

const int MEMMOVELEN = 7*sizeof(int);

int QRSDet( int datum, int init )
{
	static int det_thresh, qpkcnt = 0 ;
	static int qrsbuf[8], noise[8], rrbuf[8] ;
	static int rsetBuff[8], rsetCount = 0 ;
	static int nmedian, qmedian, rrmedian ;
	static int count, sbpeak = 0, sbloc, sbcount = MS1500 ;
	static int maxder, lastmax ;
	static int initBlank, initMax ;
	static int preBlankCnt, tempPeak ;
	
	int fdatum, QrsDelay = 0 ;
	int i, newPeak, aPeak ;

/*	Initialize all buffers to 0 on the first call.	*/
	if( init )
	{
		for(i = 0; i < 8; ++i)
		{
			noise[i] = 0 ;	/* Initialize noise buffer */
			rrbuf[i] = MS1000 ;/* and R-to-R interval buffer. */
			qrsbuf[i] = 0;
			rsetBuff[i] = 0;
		}

		for (int j = 0; j < DER_DELAY; j++)
		{
			DDBuffer[j] = 0;
		}

		det_thresh = qpkcnt = maxder = lastmax = count = sbpeak = 0 ;
		initBlank = initMax = preBlankCnt = DDPtr = Dly = 0 ;
		sbcount = MS1500 ;
		rsetCount = sbloc = tempPeak = 0;
		nmedian = qmedian = rrmedian = 0;	
		QRSFilter(0,1) ;	/* initialize filters. */
		Peak(0,1) ;
	}

	fdatum = QRSFilter(datum,0) ;	/* Filter data. */

	aPeak = Peak(fdatum,0) ;

	newPeak = 0 ;
	if(aPeak && !preBlankCnt)			// If there has been no peak for 200 ms
	{										// save this one and start counting.
		tempPeak = aPeak ;
		preBlankCnt = PRE_BLANK ;			// MS200
	}
	else if(!aPeak && preBlankCnt)	// If we have held onto a peak for
	{										// 200 ms pass it on for evaluation.
		if(--preBlankCnt == 0)
			newPeak = tempPeak ;
	}
	else if(aPeak)							// If we were holding a peak, but
	{										// this ones bigger, save it and
		if(aPeak > tempPeak)				// start counting to 200 ms again.
		{
			tempPeak = aPeak ;
			preBlankCnt = PRE_BLANK ; // MS200
		}
		else if(--preBlankCnt == 0)
			newPeak = tempPeak ;
	}
	
	DDBuffer[DDPtr] = deriv1( datum, 0 ) ;
	if(++DDPtr == DER_DELAY)
		DDPtr = 0 ;

	if( qpkcnt < 8 )
	{
		++count ;
		if(newPeak > 0) count = WINDOW_WIDTH ;
		if(++initBlank == MS1000)
		{
			initBlank = 0 ;
			qrsbuf[qpkcnt] = initMax ;
			initMax = 0 ;
			++qpkcnt ;
			if(qpkcnt == 8)
			{
				qmedian = median( qrsbuf, 8 ) ;
				nmedian = 0 ;
				rrmedian = MS1000 ;
				sbcount = MS1500+MS150 ;
				det_thresh = thresh(qmedian,nmedian) ;
			}
		}
		if( newPeak > initMax )
			initMax = newPeak ;
	}
	else	
	{
		++count ;
		if(newPeak > 0)
		{   
			if(!BLSCheck(DDBuffer, DDPtr, &maxder))
			{
				if(newPeak > det_thresh)
				{
					memmove(&qrsbuf[1], qrsbuf, MEMMOVELEN) ;
					qrsbuf[0] = newPeak ;
					qmedian = median(qrsbuf,8) ;
					det_thresh = thresh(qmedian,nmedian) ;
					memmove(&rrbuf[1], rrbuf, MEMMOVELEN) ;
					rrbuf[0] = count - WINDOW_WIDTH ;
					rrmedian = median(rrbuf,8) ;
					sbcount = rrmedian + (rrmedian >> 1) + WINDOW_WIDTH ;
					count = WINDOW_WIDTH ;

					sbpeak = 0 ;

					lastmax = maxder ;
					maxder = 0 ;
					QrsDelay =  WINDOW_WIDTH + FILTER_DELAY ;
					initBlank = initMax = rsetCount = 0 ;
				}
				else
				{
					memmove(&noise[1],noise,MEMMOVELEN) ;
					noise[0] = newPeak ;
					nmedian = median(noise,8) ;
					det_thresh = thresh(qmedian,nmedian) ;

					if((newPeak > sbpeak) && ((count-WINDOW_WIDTH) >= MS360))
					{
						sbpeak = newPeak ;
						sbloc = count  - WINDOW_WIDTH ;
					}
				}
			}
		}

		if((count > sbcount) && (sbpeak > (det_thresh >> 1)))
		{
			memmove(&qrsbuf[1],qrsbuf,MEMMOVELEN) ;
			qrsbuf[0] = sbpeak ;
			qmedian = median(qrsbuf,8) ;
			det_thresh = thresh(qmedian,nmedian) ;
			memmove(&rrbuf[1],rrbuf,MEMMOVELEN) ;
			rrbuf[0] = sbloc ;
			rrmedian = median(rrbuf,8) ;
			sbcount = rrmedian + (rrmedian >> 1) + WINDOW_WIDTH ;
			QrsDelay = count = count - sbloc ;
			QrsDelay += FILTER_DELAY ;
			sbpeak = 0 ;
			lastmax = maxder ;
			maxder = 0 ;
			initBlank = initMax = rsetCount = 0 ;
		}
	}

	if( qpkcnt == 8 )
	{
		if(++initBlank == MS1000)
		{
			initBlank = 0 ;
			rsetBuff[rsetCount] = initMax ;
			initMax = 0 ;
			++rsetCount ;

			if(rsetCount == 8)
			{
				for(i = 0; i < 8; ++i)
				{
					qrsbuf[i] = rsetBuff[i] ;
					noise[i] = 0 ;
				}
				qmedian = median( rsetBuff, 8 ) ;
				nmedian = 0 ;
				rrmedian = MS1000 ;
				sbcount = MS1500+MS150 ;
				det_thresh = thresh(qmedian,nmedian) ;
				initBlank = initMax = rsetCount = 0 ;
                sbpeak = 0 ;
			}
		}
		if( newPeak > initMax )
			initMax = newPeak ;
	}

	return(QrsDelay) ;
}

int Peak( int datum, int init )
{
	static int max = 0, timeSinceMax = 0, lastDatum ;
	int pk = 0 ;

	if(init)
		max = timeSinceMax = 0 ;
		
	if(timeSinceMax > 0)
		++timeSinceMax ;

	if((datum > lastDatum) && (datum > max))
	{
		max = datum ;
		if(max > 2)
			timeSinceMax = 1 ;
	}
	else if(datum < (max >> 1))
	{
		pk = max ;
		max = 0 ;
		timeSinceMax = 0 ;
		Dly = 0 ;
	}
	else if(timeSinceMax > MS95)
	{
		pk = max ;
		max = 0 ;
		timeSinceMax = 0 ;
		Dly = 3 ;
	}
	lastDatum = datum ;
	return(pk) ;
}

int median(int *array, int datnum)
{
	int i, j, k, temp, sort[20] ;
	for(i = 0; i < datnum; ++i)
		sort[i] = array[i] ;
	for(i = 0; i < datnum; ++i)
	{
		temp = sort[i] ;
		for(j = 0; (temp < sort[j]) && (j < i) ; ++j) ;
		for(k = i - 1 ; k >= j ; --k)
			sort[k+1] = sort[k] ;
		sort[j] = temp ;
	}
	return(sort[datnum>>1]) ;
}

int thresh(int qmedian, int nmedian)
{
	int thrsh, dmed ;
	double temp ;
	dmed = qmedian - nmedian ;
/*	thrsh = nmedian + (dmed>>2) + (dmed>>3) + (dmed>>4); */
	temp = dmed ;
	temp *= TH ;
	dmed = temp ;
	thrsh = nmedian + dmed ; /* dmed * THRESHOLD */
	return(thrsh) ;
}

int BLSCheck(int *dBuf,int dbPtr,int *maxder)
{
	int max, min, maxt, mint, t, x ;
	max = min = 0 ;

	return(0) ;
	
	for(t = 0; t < MS220; ++t)
	{
		x = dBuf[dbPtr] ;
		if(x > max)
		{
			maxt = t ;
			max = x ;
		}
		else if(x < min)
		{
			mint = t ;
			min = x;
		}
		if(++dbPtr == DER_DELAY)
			dbPtr = 0 ;
	}

	*maxder = max ;
	min = -min ;
   
	if((max > (min>>3)) && (min > (max>>3)) &&
		(abs(maxt - mint) < MS150))
		return(0) ;		
	else
		return(1) ;
}

