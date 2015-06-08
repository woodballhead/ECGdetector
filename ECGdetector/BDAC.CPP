#include "stdafx.h"
#include "qrsdet.h"	
#include "bdac.h"

#define ECG_BUFFER_LENGTH	1000	
#define BEAT_QUE_LENGTH	10			

// Internal function prototypes.
void DownSampleBeat(int *beatOut, int *beatIn) ;
// External function prototypes.
int QRSDet( int datum, int init ) ;

int GetDominantType(void) ;
int GetBeatEnd(int type) ;
int GetBeatBegin(int type) ;
int gcd(int x, int y) ;

// Global Variables
int ECGBuffer[ECG_BUFFER_LENGTH], ECGBufferIndex = 0 ;  
int BeatBuffer[BEATLGTH] ;
int BeatQue[BEAT_QUE_LENGTH], BeatQueCount = 0 ;  

int InitBeatFlag = 1 ;

void ResetBDAC(void)
{
	int dummy ;
	int qrsOnset = 0, qrsOffset = 0;
	QRSDet(0,1) ;	
	InitBeatFlag = 1 ;
    BeatQueCount = 0 ;	
}

int peakfinder(short datum, bool init, short refractory )
{
	static int max = 0, timeSinceMax = 0, lastDatum = 0;
	
	if(init)
		max = timeSinceMax = 0 ;

	int Dly = 0 ;	
	if(timeSinceMax > 0)
		++timeSinceMax ;

	if((datum > lastDatum) && (datum > max))
	{
		max = datum ;
		if(max > 0)
			timeSinceMax = 1 ;
	}
	else if ((datum < (max >> 1)) && (timeSinceMax > refractory))
	{
		Dly = timeSinceMax ;
		max = 0 ;
		timeSinceMax = 0 ;
	}
	
	lastDatum = datum ;
	return(Dly) ;
}

int BeatDetectAndClassify(int ecgSample)
{
	int detectDelay = 0, rr = 0, i = 0, j = 0;
	int beatBegin = 0, beatEnd = 0;

	ECGBuffer[ECGBufferIndex] = ecgSample ;
	if(++ECGBufferIndex == ECG_BUFFER_LENGTH)
		ECGBufferIndex = 0 ;



	for(i = 0; i < BeatQueCount; ++i)
		++BeatQue[i] ;

	detectDelay = QRSDet(ecgSample,0) ;
	if(detectDelay != 0)
	{
		BeatQue[BeatQueCount] = detectDelay ;
		++BeatQueCount ;
	}

	return(detectDelay) ;
}

void DownSampleBeat(int *beatOut, int *beatIn)
{
	int i ;

	for(i = 0; i < BEATLGTH; ++i)
		beatOut[i] = (beatIn[i<<1]+beatIn[(i<<1)+1])>>1 ;
}






















