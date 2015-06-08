// ECGdetector.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include "filter.h"
#include "BDAC.H"

using namespace std;


short *OpenTxtfile(char *filename,int& len);
int _tmain(int argc, _TCHAR* argv[])
{
	int len = 0;
	short *origndata = NULL;

	origndata = OpenTxtfile("..\\Data\\ecg.txt",len); // .txt格式文件读取
	if (origndata == NULL)
		return 0;

	printf("length:%d \n",len);

	short* dbldata = (short *)malloc(sizeof(short)*(2*len - 1));
	short* filterdata = (short *)malloc(sizeof(short)*(2*len - 1));

	if (filterdata == NULL)
	{
		return NULL;
	}

	ResetHP03Buffer();
	int rnum = 0;
	short delay;
	int* rlist = (int *)malloc(sizeof(int)*(len>>4));   // R点个数不会超过采样点数的1/16

	ResetBDAC();
	for (int i = 0; i < len - 1; i++)
	{
		dbldata[2*i] = origndata[i];
		dbldata[2*i + 1] = (origndata[i] + origndata[i + 1])>>1;
		filterdata[2*i] = FilterHighPass03(dbldata[2*i]);
		filterdata[2*i + 1] = FilterHighPass03(dbldata[2*i + 1]);
		int data = (int)(filterdata[2*i]);
		delay = BeatDetectAndClassify(data); 
		if(delay != 0)
		{
			rlist[rnum] = 2*i - delay; 
			rnum ++;
		}
		data = (int)(filterdata[2*i + 1]);
		delay = BeatDetectAndClassify(data); 
		if(delay != 0)
		{
			rlist[rnum] = 2*i + 1 - delay; 
			rnum ++;
		}

	}
	printf("rnum:%d \n",rnum);

	FILE *fp;	
	fp = fopen("..\\data\\ecg_out.txt", "w");
	if (fp == NULL)
	{
		printf("Can't save file.");
		return 0;
	}

	for (int i = 0; i < rnum; i++)
		fprintf(fp, "%d\t%d\r\n",i + 1, rlist[i]);
		
	fclose(fp);

	
	FILE *fp1;	
	fp1 = fopen("..\\data\\ecg_filtered.txt", "w");
	if (fp == NULL)
	{
		printf("Can't save file.");
		return 0;
	}

	for (int i = 0; i < 2*len - 2; i++)
		fprintf(fp1, "%d\r\n",filterdata[i]);
		
	fclose(fp1);

	if (filterdata != NULL)
	{
		free(filterdata);
		filterdata = NULL;
	}
	if (rlist != NULL)
	{
		free(rlist);
		rlist = NULL;
	}

	
	return 0;
}


short *OpenTxtfile(char *filename,int& len)
{
	short *ecgdata;
	ifstream ReadFile;
	FILE *fp;
	int n=0;
	char line[512];
	string temp;
	ReadFile.open(filename,ios::in);//ios::in 表示以只读的方式读取文件
	if(ReadFile.fail())//文件打开失败:返回0
	{
		return 0;
	}
	else//文件存在
	{
		while(getline(ReadFile,temp))
		{
			len++;
		}
	}
	ReadFile.close();

	if((fp  = fopen(filename, "r")) == NULL)
	{
		printf("打开数据文件失败！");
		return 0;
	}
	ecgdata = (short *)malloc(sizeof(short)* len);
	//read data
	for (int j = 0; j < len; j++)
	{
		fscanf(fp, "%d", ecgdata + j);
	}
	
	return ecgdata;
}
