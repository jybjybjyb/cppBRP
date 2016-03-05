/*
 * LookUp.cpp
 *
 *  Created on: 2015年9月24日
 *      Author: 10177270
 */


//#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windows.h>
#include "GlobalDef.h"
#include "DataProc.h"
#include "BRP.h"
#include "RFC.h"
#include "LookUp.h"

using namespace std;


void
InitResults(){
	unsigned int numResult;
	numResult=g_PackageSet.numPackages;
	gptr_ResultSet=(struct LookupResult *)malloc(numResult * sizeof(struct LookupResult));
	if(NULL == gptr_ResultSet){
		printf("(malloc) gptr_ResultSet error !!!\n");
		exit(1);
	}else{
		malloc_cnt += numResult * sizeof(struct LookupResult);
	}
}



struct Part *
QuarryPartition(struct PACKAGE *pTmpPack){
	//declaration
	struct TreeNode * tmpTreeNode;

	//init
	tmpTreeNode=&g_RootNode;

	for(unsigned int i=0;i<(DIM+1);i++){
		//向下搜索DIM+1层
		if(tmpTreeNode->split < 0){
//			printf("PartID:%d",tmpTreeNode->ptrPart->PartID);
			return tmpTreeNode->ptrPart;
		}else if(pTmpPack->dim[i]<=tmpTreeNode->split && g_split_state[i]>=0){
				tmpTreeNode=tmpTreeNode->LeftTreeNode;
		}else if(pTmpPack->dim[i] > tmpTreeNode->split && g_split_state[i]>=0){
				tmpTreeNode=tmpTreeNode->RightTreeNode;
		}
	}


	printf("partition not found!!!");
	exit(1);
}



void
Lookup_package() {
	//init
	ReadPackFile();
	InitResults();

	unsigned int cid[8]; //6+2
	unsigned int indx[3]; //2+1
	unsigned int tmpResult;
	struct PACKAGE * pTmpPack;
	struct Part * tmpPart;
	pTmpPack=g_PackageSet.pPackageSetHead;
	unsigned int offset;

	/*look up every single rule*/
	for (unsigned int line = 0; line < g_PackageSet.numPackages; line++){

		//inter searching...
		tmpPart=QuarryPartition(pTmpPack);

		/*	phase 0 */
		for (unsigned int i = 0; i < DIM; i++) {
			offset=(unsigned int)tmpPart->dim[i][0];
			cid[i] = (unsigned int)tmpPart->phase0_Nodes[i].cell[(unsigned int)pTmpPack->dim[i]-offset];
		}

		/*	phase 1 */
		indx[0] = cid[tmpPart->dot[0]] * (unsigned int)tmpPart->phase0_Nodes[tmpPart->dot[1]].listEqs.nCES * (unsigned int)tmpPart->phase0_Nodes[tmpPart->dot[2]].listEqs.nCES \
				+ cid[tmpPart->dot[1]] * (unsigned int)tmpPart->phase0_Nodes[tmpPart->dot[2]].listEqs.nCES \
				+ cid[tmpPart->dot[2]];
		indx[1] = cid[tmpPart->dot[3]] * (unsigned int)tmpPart->phase0_Nodes[tmpPart->dot[4]].listEqs.nCES * (unsigned int)tmpPart->phase0_Nodes[tmpPart->dot[5]].listEqs.nCES \
				+ cid[tmpPart->dot[4]] * (unsigned int)tmpPart->phase0_Nodes[tmpPart->dot[5]].listEqs.nCES \
				+ cid[tmpPart->dot[5]];

/*		if((indx[0] > tmpPart->phase1_Nodes[0].ncells) || (indx[1] > tmpPart->phase1_Nodes[1].ncells)){
			printf("\nlookup indx_phase1 overflow!!\n");
//			exit(1);
		}*/

		cid[6] = (unsigned int)tmpPart->phase1_Nodes[0].cell[indx[0]];
		cid[7] = (unsigned int)tmpPart->phase1_Nodes[1].cell[indx[1]];

		/* phase 2 */
		indx[2] = cid[6] * tmpPart->phase1_Nodes[1].listEqs.nCES + cid[7];

/*		if((indx[2] > tmpPart->phase1_Nodes[2].ncells) ){
			printf("\nlookup indx_phase2 overflow!!\n");
//			exit(1);
		}*/
		// store lookup result into lookupResult[]
		tmpResult = (unsigned int)tmpPart->phase2_Nodes.cell[indx[2]];

		//next rule
		pTmpPack=pTmpPack->pNextPack;

		//storing result
		gptr_ResultSet[line].RuleIndx=tmpResult;
		gptr_ResultSet[line].PartIndex=tmpPart->PartID;

	}	//end of line
}

void
ShowTree(){
	printf("\n_______________________Tree_____________________\n");

	TracerShowRuleSet(&g_RootNode);
}

void
ShowPartition(){
	unsigned int cnt;
	printf("\n______________________Partition_________________\n");
	printf("%-12s%-12s\n","PartId","aLiveRuleBmp");
	struct Part *tmpPart;
	tmpPart=g_PartList.PartListHead;
	for(unsigned int i =0;i<g_PartList.nPart;i++){
		cnt=CountRuleSet(tmpPart->aLiveRuleBmp);
		printf("%-12d%-12d",tmpPart->PartID,cnt);
		printf("\n");
		tmpPart=tmpPart->NextPart;
	}
}


void
InitTreeRoot(struct TreeNode *pRootNode){

	//init Root
	unsigned int *ptBMP;
	ptBMP=pRootNode->RootMabj.aLiveRuleBmp;
	FullBmp(ptBMP);

	for(unsigned int j =0;j<DIM;j++){
		pRootNode->RootMabj.ab[j][0]=0;
		pRootNode->RootMabj.ab[j][1]=DIM_RANGE-1;
	}



	//generating tree root node
	BuildNextPhaseMabj(0,pRootNode);
	pRootNode->LeftTreeNode=NULL;
	pRootNode->RightTreeNode=NULL;
	pRootNode->ptrPart=NULL;
}


