/*
 * RFC.h
 *
 *  Created on: 2015��9��24��
 *      Author: 10177270
 */

#ifndef CPP_BRP_CRFC_6F_EXP_SRC_RFC_H_
#define CPP_BRP_CRFC_6F_EXP_SRC_RFC_H_


#include "GlobalDef.h"
#include "cRFC.h"


// structure for CES...
struct CES
{
	unsigned short eqID;				// eqID��16bit = 0~65535,����Χ����16λ,���65536��rules;
	unsigned int  cbm[SIZE];			// CBM��LENGTH��SIZE=32*32=1024 bits ��Ӧ 1000rules
	struct CES *pnext;							// next CES
};



// structure for List of CES
struct LISTEqS
{
	unsigned short nCES;				// number of CES
	struct CES *phead;							// head pointer of LISTEqS
	struct CES *prear;							// pointer to end node of LISTEqS
};

// structure for Phase0 node
struct Pnode
{
	unsigned short cell[DIM_RANGE];	// each cell stores an eqID�� ���IPT����Χ��0~65535��16bit�� number line �ռ�
	struct LISTEqS listEqs;			// list of Eqs
};

// structure for Phase1 & Phase2 node
struct Pnoder
{
	unsigned long ncells;				// IPT��index��
	unsigned short *cell;				// dynamic alloc cell of chunk in phase1
	struct LISTEqS listEqs;
};



/****************** function declaration ************************/
void ClearBmp(unsigned int *bmp);
void SetBmpBit(unsigned int *tbmp, unsigned int i, unsigned char flag);
void ResetListEqs(LISTEqS *ptrlistEqs);
bool CompareBmp(unsigned int *abmp, unsigned int *bbmp);
int SearchBmp(LISTEqS *pListEqs, unsigned int *pbmp);
int AddListEqsCES(LISTEqS *ptrlistEqs, unsigned int *ptbmp);
void FreeListEqs(LISTEqS *ptrlistEqs);
int GetRuleCost(unsigned int *tbmp);
void SetPhase0_Cell(struct Part* ptmpPart);
void SetPhase1_Cell(struct Part* ptmpPart);
void SetPhase2_Cell(struct Part* ptmpPart);
void ReOrder_6_33_1_enum(struct Part* ptmpPart);
void ReOrder_6_33_1_bak(struct Part *tmpPart);


#endif /* CPP_BRP_CRFC_6F_EXP_SRC_RFC_H_ */
