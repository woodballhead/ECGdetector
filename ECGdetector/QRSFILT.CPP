

#include "stdafx.h"

// Local Prototypes.

int lpfilt( int datum ,int init) ;
int hpfilt( int datum, int init ) ;
int deriv1( int x0, int init ) ;
int deriv2( int x0, int init ) ;
int mvwint(int datum, int init) ;


int QRSFilter(int datum,int init)
{
	int fdatum ;

	if(init)
	{
		hpfilt( 0, 1 ) ;		// Initialize filters.
		lpfilt( 0, 1 ) ;
		mvwint( 0, 1 ) ;
		deriv1( 0, 1 ) ;
		deriv2( 0, 1 ) ;
	}

	fdatum = lpfilt( datum, 0 ) ;	// Low pass filter data.
	fdatum = hpfilt( fdatum, 0 ) ;	// High pass filter data.
	fdatum = deriv2( fdatum, 0 ) ;	// Take the derivative.
	fdatum = abs(fdatum) ;			// Take the absolute value.
	fdatum = mvwint( fdatum, 0 ) ;	// Average over an 80 ms window .
	return(fdatum) ;
}

int lpfilt( int datum ,int init)
{
	static long y1 = 0, y2 = 0 ;
	static int data[LPBUFFER_LGTH], ptr = 0 ;
	long y0 ;
	int output, halfPtr ;
	if(init)
	{
		for(ptr = 0; ptr < LPBUFFER_LGTH; ++ptr)
			data[ptr] = 0 ;
		y1 = y2 = 0 ;
		ptr = 0 ;
	}
	halfPtr = ptr-(LPBUFFER_LGTH/2) ;	// Use halfPtr to index
	if(halfPtr < 0)							// to x[n-6].
		halfPtr += LPBUFFER_LGTH ;
	y0 = (y1 << 1) - y2 + datum - (data[halfPtr] << 1) + data[ptr] ;
	y2 = y1;
	y1 = y0;
	output = y0 / ((LPBUFFER_LGTH*LPBUFFER_LGTH)/4);
	data[ptr] = datum ;			// Stick most recent sample into
	if(++ptr == LPBUFFER_LGTH)	// the circular buffer and update
		ptr = 0 ;					// the buffer pointer.
	return(output) ;
}

int hpfilt( int datum, int init )
{
	static long y=0 ;
	static int data[HPBUFFER_LGTH], ptr = 0 ;
	int z, halfPtr ;

	if(init)
	{
		for(ptr = 0; ptr < HPBUFFER_LGTH; ++ptr)
			data[ptr] = 0 ;
		ptr = 0 ;
		y = 0 ;
	}

	y += datum - data[ptr];
	halfPtr = ptr-(HPBUFFER_LGTH/2) ;
	if(halfPtr < 0)
		halfPtr += HPBUFFER_LGTH ;
	z = data[halfPtr] - (y / HPBUFFER_LGTH);

	data[ptr] = datum ;
	if(++ptr == HPBUFFER_LGTH)
		ptr = 0 ;

	return( z );
}

int deriv1(int x, int init)
{
	static int derBuff[DERIV_LENGTH], derI = 0 ;
	int y ;

	if(init != 0)
	{
		for(derI = 0; derI < DERIV_LENGTH; ++derI)
			derBuff[derI] = 0 ;
		derI = 0 ;
		return(0) ;
	}

	y = x - derBuff[derI] ;
	derBuff[derI] = x ;
	if(++derI == DERIV_LENGTH)
		derI = 0 ;
	return(y) ;
}

int deriv2(int x, int init)
{
	static int derBuff[DERIV_LENGTH], derI = 0 ;
	int y ;

	if(init != 0)
	{
		for(derI = 0; derI < DERIV_LENGTH; ++derI)
			derBuff[derI] = 0 ;
		derI = 0 ;
		return(0) ;
	}

	y = x - derBuff[derI] ;
	derBuff[derI] = x ;
	if(++derI == DERIV_LENGTH)
		derI = 0 ;
	return(y) ;
}

int mvwint(int datum, int init)
{
	static long sum = 0 ;
	static int data[WINDOW_WIDTH], ptr = 0 ;
	int output;
	if(init)
	{
		for(ptr = 0; ptr < WINDOW_WIDTH ; ++ptr)
			data[ptr] = 0 ;
		sum = 0 ;
		ptr = 0 ;
	}
	sum += datum ;
	sum -= data[ptr] ;
	data[ptr] = datum ;
	if(++ptr == WINDOW_WIDTH)
		ptr = 0 ;
	if((sum / WINDOW_WIDTH) > 32000)
		output = 32000 ;
	else
		output = sum / WINDOW_WIDTH ;
	return(output) ;
}
