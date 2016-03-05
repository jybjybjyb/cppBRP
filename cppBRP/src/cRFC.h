/*
 * cRFCadd.h
 *
 *  Created on: 2015年9月24日
 *      Author: 10177270
 */

#ifndef CPP_BRP_CRFC_6F_EXP_SRC_CRFC_H_
#define CPP_BRP_CRFC_6F_EXP_SRC_CRFC_H_

#include "GlobalDef.h"
#include "RFC.h"
#include "cRFC.h"



/****************** structure ************************/
struct CompressEqID
{
	unsigned short ComId;
	CompressEqID *nextComId;
};


struct CompressComponent
{
	//string
	unsigned int nBitStraing;   //nBitStraing=ncells /32
	unsigned int *BitStraing;

	//array
	unsigned int nArray; // 1的数目
	CompressEqID *ComIdHead;
	CompressEqID *ComIdTear;
};
/*

struct CompressPrat{
	struct CompressComponent CompressBitMap_p1[2];
	struct CompressComponent CompressBitMap_p0[DIM];
	struct CompressPrat *NextCompressPrat;
};

struct CompressList{
	unsigned int nCompressList;
	struct CompressPrat * CompressListHead;
	struct CompressPrat * CompressListRear;
};

*/



/****************** function declaration ************************/
void BitMapCompress(struct Pnoder *tmpPnoder, struct CompressComponent *tmpCompressBitMap);
unsigned int cRFCTransIndx2eqID(unsigned int index,struct CompressComponent *tmpCompressBitMap);
void CountMemory_CRFC();
void Lookup_package_cRFC_6F();
void cRFC_6F();





#endif /* CPP_BRP_CRFC_6F_EXP_SRC_CRFC_H_ */
