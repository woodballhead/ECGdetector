#include "stdafx.h"
#include <stdlib.h>
#include "filter.h"
#include <math.h>


#define COEFF_NUM_HP03  2
// data buffer of 0.3Hz highpass filter
// @initialize to: 0
int  mXdataHP03[COEFF_NUM_HP03]={0,0};
int  mYdataHP03[COEFF_NUM_HP03]={0,0};
int  mY1dataHP03[COEFF_NUM_HP03]={0,0};

void ResetHP03Buffer()
{
	//初始化0.3Hz高通滤波
	for (int i = 0; i < COEFF_NUM_HP03; i++)
	{
		mXdataHP03[i] = 0;
		mYdataHP03[i] = 0;
		mY1dataHP03[i] = 0;
	}
}
int FilterHighPass03(int data)
{
   int    i;
   int  temp1, temp2;

   int* xData = mXdataHP03;
   int* yData = mYdataHP03;
   int* y1Data = mY1dataHP03;

   // data shift
   for ( i = COEFF_NUM_HP03 - 1; i > 0; i --)
   {
      *(xData + i) = *(xData + i - 1);
      *(yData + i) = *(yData + i - 1);
      *(y1Data + i) = *(y1Data + i - 1);
   }
   // save X[n]
   *xData = data * 16;

   // Calculating.
   temp1 = 8169 * (xData[0] - xData[1]) + 8146 * yData[1];

   temp2 = 8146 * y1Data[1];

   // result divided by 8192.
   if ( 0 <= temp2 )
      temp2 >>= 13;
   else
   {
      // for a negative value, we must convert it to a positive value,
      // and then do right shift.
      temp2 = - temp2;
      temp2 >>= 13;
      temp2 = - temp2;
   }

   temp1 += temp2;

   if ( 0 <= temp1 )
   {
    // reserve bit0-bit12 (13bits) for decimal fraction to reduce finite-register-length effect     
       *y1Data = temp1 & 0x00001FFF;
       temp1 >>= 13;
   }
   else
   {
       *y1Data = - ( ( - temp1 ) & 0x00001FFF );
       // for a negative value, we must convert it to a positive value,
       // and then do right shift.
       temp1 = - temp1;
       temp1 >>= 13;
       temp1 = - temp1;
   }

   *yData = temp1;

   return (temp1 >> 4);
}