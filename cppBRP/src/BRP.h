/*
 * BRP.h
 *
 *  Created on: 2015年9月24日
 *      Author: 10177270
 */

#ifndef CPP_BRP_CRFC_6F_EXP_SRC_BRP_H_
#define CPP_BRP_CRFC_6F_EXP_SRC_BRP_H_


#include "GlobalDef.h"
#include "RFC.h"

struct Mabj
{
	unsigned short ab[DIM][2];
	unsigned int aLiveRuleBmp[SIZE]; //32*32=1024
	unsigned int size;
};


struct MabjLIST{
	unsigned short LargestSize; //0指空集
	struct Mabj *Largest;
	struct Mabj *ListHead;
	struct Mabj *ListTear;
};


struct TreeNode{

	int split;
	struct TreeNode *LeftTreeNode;
	struct TreeNode *RightTreeNode;
	struct Part *ptrPart;
	struct Mabj RootMabj;
};


struct Part{
	unsigned int PartID;

	// BRP gen
	unsigned short dim[DIM][2]; //每个域的开始结束
	unsigned int aLiveRuleBmp[SIZE];

	// structure for RFC
	unsigned int dot[DIM];
	struct Pnoder phase2_Nodes;
	struct Pnoder phase1_Nodes[2];
	struct Pnoder phase0_Nodes[DIM];

#ifdef CRFC

	struct CompressComponent CompressBitMap_p1[2];
	struct CompressComponent CompressBitMap_p0[DIM];

#endif



	struct Part *NextPart;

};

struct PartList{
	unsigned int nPart;
	struct Part *PartListHead;
	struct Part *PartListRear;
};


/****************** function declaration ************************/
void ResetMabj(struct Mabj *pMabjList);
void ResetMabjLIST(struct MabjLIST *pMabjList);
struct TreeNode * FindLargest(struct TreeNode *tmpRoot);
void MergeBmp(unsigned int *fromBmp,		unsigned int *toBmp);
int ArgMin(unsigned int FieldIndx,	struct Mabj *pMabj);
unsigned long CalcComplexity(unsigned int FieldIndx,	struct Mabj *pMabj);
void ShowRuleSet(unsigned int *aliveBmp);
void TracerShowRuleSet(struct TreeNode * ptTreeNode);
void SetRulesVector(unsigned int FieldIndx,struct Mabj *pMabj);
unsigned int SetSize(struct Mabj *pMabj);
void CopyBmp(unsigned int *from, unsigned int *to);
void BuildNextPhaseMabj(unsigned int FieldIndx,	struct TreeNode *pMabjNode);
void BRP(unsigned int FieldIndx,		struct TreeNode *pTreeNode);
void RBRP(unsigned int div,		struct TreeNode *pRoot);
unsigned int CountRuleSet(unsigned int *aliveBmp);
bool isRuleLive(unsigned int i, unsigned int *aLiveRuleBmp);
void FullBmp(unsigned int *bmp);
void InitPartList();
void BuildPartition(struct TreeNode *ptmpTreeNode);
void BuildPartRFC(struct PartList *ptrPartList);
void InitTreeRoot(struct TreeNode *pRootNode);

#endif /* CPP_BRP_CRFC_6F_EXP_SRC_BRP_H_ */
