/*
 * data_proc.h
 *
 *  Created on: 2015��9��24��
 *      Author: 10177270
 */

#ifndef DATA_PROC_H_
#define DATA_PROC_H_

#include "GlobalDef.h"


/******************** structure defination *****************/

//	structures for filters...
struct FILTER
{
	// the bytes needed in practice, totally
//	unsigned int 	cost;				// 4 bytes, ����Ĵ���, �������ڹ�����е������, ���ǹ����line���
//	unsigned char  	act;				// 1 byte, ִ������
	unsigned int	dim[DIM][2];			// refer to the start & end of every dimension

};

struct FILTSET
{
	unsigned int	numFilters;				// ��������
	struct FILTER	filtArr[MAXFILTERS];	// ��Ź���Ŀռ�, ���������Թ��������̬�����ڴ�
};

//	structures for packages...
struct PACKAGE
{
	int	dim[6];				// ά�� refer to all the dimension��chunk
	struct PACKAGE * pNextPack;
};

struct PACKAGESET
{
	unsigned int	numPackages;				// ��������
	struct PACKAGE *	pPackageSetHead;
	struct PACKAGE *	pPackageSetRear;
//	struct PACKAGE	PackArr[MAXPACKAGES];		// ��������Ŀռ�, ��������������������̬�����ڴ�
};



struct LookupResult{
	unsigned int RuleIndx;
	unsigned int PartIndex;
//	char *isFound;
	struct LookupResult *NextResult;
};
//char y[4]="yes";
//char n[4]="no";


/****************** function declaration ************************/
void ReadIPRange(FILE *fp, unsigned int* highRange, unsigned int* lowRange);
void ReadProtocol(FILE *fp, unsigned char *from, unsigned char *to);
void ReadPort(FILE *fp, unsigned int *from, unsigned int *to);
void ReadAct(FILE *fp, unsigned char *action);
int ReadFilter(FILE *fp, struct FILTSET * filtset, unsigned int cost);
void LoadFilters(FILE *fp, struct FILTSET * filtset);
void ReadFilterFile();
void LoadPackages(FILE *fp, struct PACKAGESET *packageset);
void ReadPackFile();
void StoreResults(char *FileName);
void SaveTemporary();
void LoadTempprary();
void ShowResults();
void CountMemory();




#endif /* DATA_PROC_H_ */
