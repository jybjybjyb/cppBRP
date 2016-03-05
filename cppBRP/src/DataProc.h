/*
 * data_proc.h
 *
 *  Created on: 2015年9月24日
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
//	unsigned int 	cost;				// 4 bytes, 规则的代价, 即规则在规则库中的正序号, 就是规则的line序号
//	unsigned char  	act;				// 1 byte, 执行命令
	unsigned int	dim[DIM][2];			// refer to the start & end of every dimension

};

struct FILTSET
{
	unsigned int	numFilters;				// 规则总数
	struct FILTER	filtArr[MAXFILTERS];	// 存放规则的空间, 这里可以针对规则个数动态分配内存
};

//	structures for packages...
struct PACKAGE
{
	int	dim[6];				// 维度 refer to all the dimension，chunk
	struct PACKAGE * pNextPack;
};

struct PACKAGESET
{
	unsigned int	numPackages;				// 网包总数
	struct PACKAGE *	pPackageSetHead;
	struct PACKAGE *	pPackageSetRear;
//	struct PACKAGE	PackArr[MAXPACKAGES];		// 存放网包的空间, 这里可以针对网包个数动态分配内存
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
