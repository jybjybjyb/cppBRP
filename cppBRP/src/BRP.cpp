/*
 * BRP.cpp
 *
 *  Created on: 2015年9月24日
 *      Author: 10177270
 */

//#include "stdafx.h"
#include <iostream>
#include <string>
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
ClearBmp(unsigned int *bmp){
	for(unsigned int i=0; i<SIZE;i++){
		bmp[i]=0;
	}
}

void
FullBmp(unsigned int *bmp){
	for(unsigned int i=0; i<SIZE;i++){
		bmp[i]=0xFFFFFFFF;
	}
}

void
ResetMabj(struct Mabj *pMabj){
//	pMabj->complexity=0;
	pMabj->size=0;

	for(unsigned int i=0; i<3; i++){
		pMabj->ab[i][0]=0;
		pMabj->ab[i][1]=65535;
	}

	FullBmp(pMabj->aLiveRuleBmp);
//	pMabj->nextMjab=NULL;
//	pMabj->preMjab=NULL;


}

void
ResetMabjLIST(struct MabjLIST *pMabjList){
	pMabjList->LargestSize=0;
	pMabjList->Largest=NULL;
	pMabjList->ListHead=NULL;
	pMabjList->ListTear=NULL;
}

struct TreeNode *
FindLargest(struct TreeNode *tmpRoot){

	if(tmpRoot->split<0){
		//没有划分
		return tmpRoot;

	}else if(tmpRoot->LeftTreeNode->RootMabj.size <= tmpRoot->RightTreeNode->RootMabj.size){

		return tmpRoot->RightTreeNode;
	}else if(tmpRoot->LeftTreeNode->RootMabj.size > tmpRoot->RightTreeNode->RootMabj.size){

		return tmpRoot->LeftTreeNode;
	}
	return NULL;
}

void
MergeBmp(unsigned int *fromBmp,
			unsigned int *toBmp){

	for(unsigned int i=0; i<SIZE;i++){
		toBmp[i]&=fromBmp[i];
	}
}
/*

unsigned short ArgMin_v1(
		unsigned int FieldIndx,
		unsigned short a,
		unsigned short b){

	//Initializing
	unsigned int com=FieldIndx;
	unsigned short tStartNode=a;
	unsigned short tEndNode=b;

	unsigned short argmins;

	unsigned short cnt_bitmap_1; //计算bitmap 1 的数量
	unsigned short min_cnt = 0xFFFF;
	bool previous_state = false; //前一状态 False 表示 cnt_bitmap_1 减少过
	unsigned int rule_start, rule_end;

	for (unsigned short n = tStartNode; n <tEndNode; n++) {
		//Initializing
		cnt_bitmap_1 = 0;

		for (unsigned int i = 0; i < g_filtset.numFilters; i++) {
			// generating bitmap at node n
			rule_start = g_filtset.filtArr[i].dim[com][0];
			rule_end = g_filtset.filtArr[i].dim[com][1];

			if (rule_start <= n) {
				//波谷
				if (previous_state == true && cnt_bitmap_1 < min_cnt) {
					min_cnt = cnt_bitmap_1;
					argmins = n-1;
				}

//				SetBmpBit(bmp, i, TRUE); /bmp 为(unsign int *) 指向bmp[32]首地址
				cnt_bitmap_1++;
			}
			if (rule_end < n) {
//				SetBmpBit(bmp, i, FALSE);
				cnt_bitmap_1--;
				previous_state = true; //设置 cnt_bitmap_1 减少过
			}

		} rule  遍历结束

	}  StartNode~ EndNode 遍历结束

	return argmins;
}

unsigned short ArgMin_v2(
		unsigned int FieldIndx,
		unsigned short a,
		unsigned short b){

	//Initializing
	unsigned int com=FieldIndx;
	unsigned short tStartNode=a;
	unsigned short tEndNode=b;

	unsigned short argmins=0;
	unsigned short match_rule=0;
	unsigned short pre_match=0;
	unsigned short min_cnt = 0xFFFF;

	//前一状态, 1:pre_match > match_rule, 2:pre_match < match_rule,
//	unsigned int state = 0;
	unsigned int rule_start, rule_end;

	for (unsigned short n = tStartNode; n <tEndNode; n++) {
		match_rule=0;

		for (unsigned int i = 0; i < g_filtset.numFilters; i++) {
			// generating bitmap at node n
			rule_start = g_filtset.filtArr[i].dim[com][0];
			rule_end = g_filtset.filtArr[i].dim[com][1];

			if (rule_start <= n && n <= rule_end ) {
				match_rule++;
			}
		} rule  遍历结束


		if(0 != n){
			if(pre_match > match_rule){

				if(match_rule < min_cnt){
					min_cnt = match_rule;
					argmins = n-1;
				}
			}else if(pre_match < match_rule){

				if(pre_match < min_cnt){
					min_cnt = match_rule;
					argmins = n-1;
				}
			}
		}
		pre_match=match_rule;


	}  StartNode~ EndNode 遍历结束

	return argmins;
}

*/

bool
isRuleLive(unsigned int i, unsigned int *aLiveRuleBmp){
	unsigned int k,pos,MASK,tmp;

	k=i / 32;
	pos=i % 32;
	MASK=1;
	tmp=aLiveRuleBmp[k] & (MASK << pos);
	if(tmp==0){
		return false;
	}
	return true;
}

int
ArgMinBak(unsigned int FieldIndx,struct Mabj *pMabj){

	//Initializing
	unsigned int j=FieldIndx;
	unsigned short tStartNode=pMabj->ab[j][0];
	unsigned short tEndNode=pMabj->ab[j][1];
	unsigned int *aLiveRuleBmp;
	aLiveRuleBmp=pMabj->aLiveRuleBmp;

	int argmins=-1;   //argmins=-1 意味着没有找到
	unsigned short hit_rule=0;
	unsigned short max_hit = 0;

	//前一状态, 1:pre_match > match_rule, 2:pre_match < match_rule,
	unsigned int rule_start, rule_end;

	for (unsigned short n = tStartNode; n <tEndNode; n++) {
		hit_rule=0;

		for (unsigned int i = 0; i < g_FiltSet.numFilters; i++) {
			if(isRuleLive(i, aLiveRuleBmp)){

				rule_start = g_FiltSet.filtArr[i].dim[j][0];
				rule_end = g_FiltSet.filtArr[i].dim[j][1];

				//这样也能集成了
				if (rule_start == n){
					hit_rule++;
				}
				if((rule_end + 1) == n) {
					hit_rule++;
				}
			}
		} /*rule  遍历结束*/

		if(!(n == tStartNode || n == tEndNode)){
			if(hit_rule > max_hit){

				max_hit = hit_rule;
				argmins = n-1;
			}
		}

	} /* StartNode~ EndNode 遍历结束*/

	return argmins;
}



unsigned int
CountRule_1(unsigned int *bmp){

	unsigned int MASK=1;
	unsigned int tmp;
	unsigned int cnt_1=0;

	for(unsigned int i=0; i<1000; i++){
		tmp=bmp[i / 32] & (MASK <<(i % 32));
		if(tmp!=0){
			cnt_1++;
		}
	}
	return cnt_1;
}

int
ArgMin(unsigned int FieldIndx,struct Mabj *pMabj){

	//Initializing
	unsigned int j=FieldIndx;
	unsigned int bmp_a[SIZE]; // 32*32=1024
	for (unsigned int i = 0; i < SIZE; i++){bmp_a[i] = 0;}
	unsigned int bmp_b[SIZE]; // 32*32=1024
	for (unsigned int i = 0; i < SIZE; i++){bmp_b[i] = 0;}
	unsigned int rule_start, rule_end;
	int argmins,tmp_argmins;
	argmins=-1;//argmins=-1 意味着没有找到
	unsigned int cnt_1,cnt_min,first_cnt;

	//copy
	unsigned int tStartNode=(unsigned int)pMabj->ab[j][0];
	unsigned int tEndNode=(unsigned int)pMabj->ab[j][1];
	unsigned int *aLiveRuleBmp;
	aLiveRuleBmp=pMabj->aLiveRuleBmp;


	for (unsigned int n = tStartNode; n <tEndNode; n++) {

		CopyBmp(bmp_b,bmp_a);

		if(n==0){
			for (unsigned int i = 0; i < g_FiltSet.numFilters; i++) {
				if(isRuleLive(i, aLiveRuleBmp)){

					rule_start = g_FiltSet.filtArr[i].dim[j][0];
					rule_end = g_FiltSet.filtArr[i].dim[j][1];

					if (rule_start <= n) {
						SetBmpBit(bmp_a, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
					}
					if (rule_end < n) {
						SetBmpBit(bmp_a, i, 0);
					}

					if (rule_start <= n+1) {
						SetBmpBit(bmp_b, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
					}
					if (rule_end < n+1) {
						SetBmpBit(bmp_b, i, 0);
					}
				}/*rule  遍历结束*/

				MergeBmp(bmp_b,bmp_a);
				first_cnt=CountRule_1(bmp_a);
				cnt_min=first_cnt;

			}
		}else{
			for (unsigned int i = 0; i < g_FiltSet.numFilters; i++) {
				if(isRuleLive(i, aLiveRuleBmp)){

					rule_start = g_FiltSet.filtArr[i].dim[j][0];
					rule_end = g_FiltSet.filtArr[i].dim[j][1];


					if(n==0){
						if (rule_start <= n) {
							SetBmpBit(bmp_a, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
						}
						if (rule_end < n) {
							SetBmpBit(bmp_a, i, 0);
						}

						if (rule_start <= n+1) {
							SetBmpBit(bmp_b, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
						}
						if (rule_end < n+1) {
							SetBmpBit(bmp_b, i, 0);
						}
					}else{
						if (rule_start <= n+1) {
							SetBmpBit(bmp_b, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
						}
						if (rule_end < n+1) {
							SetBmpBit(bmp_b, i, 0);
						}
					}
				}
			} /*rule  遍历结束*/

			MergeBmp(bmp_b,bmp_a);
			cnt_1=CountRule_1(bmp_a);

			if(cnt_1==first_cnt){
				tmp_argmins=n;
			}

			if(cnt_1<cnt_min){
				cnt_min=cnt_1;
				argmins=n;
			}
		}
	} /* n 遍历结束*/

	if(cnt_min<first_cnt ){
		return argmins;
	}else{
		return tmp_argmins;
	}
}





unsigned int
CountRuleSet(unsigned int *aliveBmp){
	unsigned int cnt=0;
	unsigned int bmp[SIZE];
	for(int i=0;i<SIZE;i++){
		bmp[i]=aliveBmp[i];
	}

	unsigned int MASK=1;
	unsigned int tmp;
//	printf("\n(");
	for(unsigned int i=0; i<1000; i++){
		tmp=bmp[i / 32] & (MASK <<(i % 32));
		if(tmp!=0){
//			printf("%d,",i+1);
			cnt++;
		}
	}
	return cnt;
}


void
ShowRuleSet(unsigned int *aliveBmp){
	unsigned int bmp[SIZE];
	for(int i=0;i<SIZE;i++){
		bmp[i]=aliveBmp[i];
	}

	unsigned int MASK=1;
	unsigned int tmp;
	for(unsigned int i=0; i<1000; i++){
		tmp=bmp[i / 32] & (MASK <<(i % 32));
		if(tmp!=0){
			printf("%d,",i+1);
		}
	}
}



void
TracerShowRuleSet(struct TreeNode * ptTreeNode){
	if(ptTreeNode != NULL){
		ShowRuleSet(ptTreeNode->RootMabj.aLiveRuleBmp);
		TracerShowRuleSet(ptTreeNode->LeftTreeNode);
		TracerShowRuleSet(ptTreeNode->RightTreeNode);
	}

}

void
SetRulesVector(unsigned int FieldIndx,
			struct Mabj *pMabj
			){
	//Initializing
	unsigned int j=FieldIndx;
	unsigned short ta=pMabj->ab[j][0];
	unsigned short tb=pMabj->ab[j][1];

	unsigned int *bmp;
	bmp=pMabj->aLiveRuleBmp;
	ClearBmp(bmp);

	unsigned short pos_start, pos_end;

	for (unsigned int i = 0; i < g_FiltSet.numFilters; i++) {
		// generating bitmap at node n
		pos_start = g_FiltSet.filtArr[i].dim[j][0];
		pos_end = g_FiltSet.filtArr[i].dim[j][1];

		if (!(pos_end < ta || pos_start > tb)){
			SetBmpBit(bmp, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
		}

	} // i==1000 rule  遍历结束


}

unsigned int
SetSize(struct Mabj *pMabj){
	unsigned int bmp[SIZE];
	for(unsigned int i=0;i<SIZE;i++){
		bmp[i]=pMabj->aLiveRuleBmp[i];
	}

	unsigned short cnt_1=0;
	unsigned int MASK=1;
	unsigned int k,pos,tmp;
	for(unsigned int j=0;j<1000;j++){
		k=j/32;
		pos=j %32;
		tmp= bmp[k] & (MASK <<pos );
		if( tmp!= 0){
			cnt_1++;
		}
	}

	return cnt_1;
}

void
CopyBmp(unsigned int *from, unsigned int *to){
	for(unsigned int i=0;i<SIZE;i++){
		to[i]=from[i];
	}

}

void
BuildNextPhaseMabj(unsigned int FieldIndx,	struct TreeNode *pMabjNode){
	unsigned int j=FieldIndx;
	struct Mabj *pMabj;
	pMabj=&pMabjNode->RootMabj;

	unsigned int pre_alivebmp[SIZE];
	CopyBmp(pMabj->aLiveRuleBmp,pre_alivebmp);

	//feild range 0~65535
	pMabj->ab[j][0]=0;
	pMabj->ab[j][1]=DIM_RANGE-1;

	SetRulesVector(j,pMabj);  //set rulebmp[SIZE]
	MergeBmp(pre_alivebmp,pMabj->aLiveRuleBmp); //merge bmp

	int split;
	split=ArgMin(j, pMabj); //set split
	if(split==-1){
		pMabjNode->LeftTreeNode=NULL;
		pMabjNode->RightTreeNode=NULL;
		return;
	}
//	pMabj->complexity=CalcComplexity(j,pMabj); //set complexity
	pMabj->size=SetSize(pMabj); //set size

	pMabjNode->LeftTreeNode=NULL;
	pMabjNode->RightTreeNode=NULL;

}


/*
 * calculator of complexity
 * return value of complexity
 * */
unsigned long
CalcComplexity(unsigned int FieldIndx,struct Mabj *ptmpMabj){

	//Init
	unsigned long sum_tot=0;
	unsigned long sum1=1;
	unsigned long sum2=1;
	unsigned long sum3=1;

	//decorating
	struct Part tmpPart;
	struct Part* ptmpPart;
	ptmpPart=&tmpPart;

	CopyBmp(ptmpMabj->aLiveRuleBmp,ptmpPart->aLiveRuleBmp);

	for(unsigned int i=0;i<DIM;i++){
		for(unsigned int j=0;j<2;j++){
			ptmpPart->dim[i][j]=ptmpMabj->ab[i][j];
		}
	}

	//RFC
	SetPhase0_Cell(ptmpPart);
	ReOrder_6_33_1_bak(ptmpPart);
	SetPhase1_Cell(ptmpPart);


	//Statistics
	for(unsigned int k=0;k<3;k++){
		sum1 *=ptmpPart->phase0_Nodes[ptmpPart->dot[k]].listEqs.nCES;
	}
	for(unsigned int k=3;k<DIM;k++){
		sum2 *=ptmpPart->phase0_Nodes[ptmpPart->dot[k]].listEqs.nCES;
	}
	for(unsigned int k=0;k<2;k++){
		sum3 *=ptmpPart->phase1_Nodes[k].listEqs.nCES;
	}

	sum_tot += sum1;
	sum_tot += sum2;
	sum_tot += sum3;

	return sum_tot;

}


void
SetLRNode(int Symbol,
		unsigned int FieldIndx,
		struct Mabj *ptmpRoot,
		struct Mabj *ptmpLR,
		unsigned short SplitNode){

	//init
	unsigned short split=SplitNode;
	unsigned int j=FieldIndx;
	int syn=Symbol;

	//copy ab start and end
	for(unsigned int k=0;k<DIM;k++){
		ptmpLR->ab[k][0]=ptmpRoot->ab[k][0];
		ptmpLR->ab[k][1]=ptmpRoot->ab[k][1];
	}

	switch(syn){
	/*set left and right node...
	 * 0 : Left
	 * 1 : Right*/
	case 0:
		ptmpLR->ab[j][0]=ptmpRoot->ab[j][0];
		ptmpLR->ab[j][1]=split;
		break;
	case 1:
		ptmpLR->ab[j][0]=split;
		ptmpLR->ab[j][1]=ptmpRoot->ab[j][1];
		break;
	default:
		break;
	}

	SetRulesVector(j,ptmpLR);  //set rulebmp[SIZE]
	MergeBmp(ptmpRoot->aLiveRuleBmp,ptmpLR->aLiveRuleBmp); //merge bmp

	ptmpLR->size=SetSize(ptmpLR); //set size

}





void
BRP(unsigned int FieldIndx,
		struct TreeNode *pTreeNode){

	unsigned int j=FieldIndx;
	struct Mabj *ptmp_root;
	ptmp_root=&pTreeNode->RootMabj;
	int split;
	unsigned long complexity_left,complexity_right,complexity_root;

	split=ArgMin(j,ptmp_root);
	//no ArgMin,return
	if(split <0){
		pTreeNode->LeftTreeNode=NULL;
		pTreeNode->RightTreeNode=NULL;
		pTreeNode->split=-1;
		pTreeNode->ptrPart=NULL;
		g_split_state[j]=-1;
		return;
	}

	/*set left and right node...
	 * 0 : Left
	 * 1 : Right*/
	struct Mabj tmp_left;
	SetLRNode(0,j,ptmp_root,&tmp_left,split);

	/*set right node*/
	struct Mabj tmp_right;
	SetLRNode(1,j,ptmp_root,&tmp_right,split);


	/*判断是否符合缩小条件*/
	complexity_left=CalcComplexity(j,&tmp_left);
	complexity_right=CalcComplexity(j,&tmp_right);
	complexity_root=CalcComplexity(j,ptmp_root);
	if(complexity_left+complexity_right < complexity_root){
		/*符合缩小条件*/
		//malloc TreeNode
		struct TreeNode *pNewTreeNode_L;
		struct TreeNode *pNewTreeNode_R;

		pNewTreeNode_L=(struct TreeNode *)malloc(sizeof(struct TreeNode));
		if(NULL == pNewTreeNode_L){
			printf("(malloc) load pack error !!!\n");
			exit(1);
		}else{
			malloc_cnt += sizeof(struct TreeNode);
		}
		pNewTreeNode_R=(struct TreeNode *)malloc(sizeof(struct TreeNode));
		if(NULL == pNewTreeNode_R){
			printf("(malloc) load pack error !!!\n");
			exit(1);
		}else{
			malloc_cnt += sizeof(struct TreeNode);
		}


		//memcpy Mabj
		memcpy(&pNewTreeNode_L->RootMabj,&tmp_left,sizeof(struct Mabj));
		memcpy(&pNewTreeNode_R->RootMabj,&tmp_right,sizeof(struct Mabj));

		//set TreeNode
		pNewTreeNode_L->LeftTreeNode=NULL;
		pNewTreeNode_L->RightTreeNode=NULL;
		pNewTreeNode_L->ptrPart=NULL;
		pNewTreeNode_L->split=-1;
		pNewTreeNode_R->LeftTreeNode=NULL;
		pNewTreeNode_R->RightTreeNode=NULL;
		pNewTreeNode_R->ptrPart=NULL;
		pNewTreeNode_R->split=-1;


		//add TreeNode
		pTreeNode->split=split;
		g_split_state[j]=split;
		pTreeNode->LeftTreeNode=pNewTreeNode_L;
		pTreeNode->RightTreeNode=pNewTreeNode_R;

		//迭代
//		BRP(j, pTreeNode->LeftTreeNode);
//		BRP(j, pTreeNode->RightTreeNode);

	}else{
		/*不符合缩小条件,return*/
		pTreeNode->LeftTreeNode=NULL;
		pTreeNode->RightTreeNode=NULL;
		pTreeNode->split=-1;
		g_split_state[j]=-1;

		return;
	}

}



void
RBRP(unsigned int div, struct TreeNode *pRoot){

	//init
	unsigned int k = div;
	struct TreeNode *ptmpMaxSize;

	for(unsigned int i=0; i<k; i++){
		//F1
		if(i==0){
			ptmpMaxSize=pRoot;
		}else{

			BuildNextPhaseMabj(i,ptmpMaxSize);
		}

		BRP(i,ptmpMaxSize);

		ptmpMaxSize = FindLargest(ptmpMaxSize);
	}
}


void
InitPartList(){
	g_PartList.nPart=0;
	g_PartList.PartListHead=NULL;
	g_PartList.PartListRear=NULL;

}


void
BuildPartition(struct TreeNode *ptmpTreeNode){
	//init
	struct Part *ptmpPart;

	if(ptmpTreeNode->split<0){
	/*	set leaf node*/

		//malloc struct Part
		ptmpPart=(struct Part *)malloc(sizeof(struct Part));
		if(NULL == ptmpPart){
			printf("(malloc) load pack error !!!\n");
			exit(1);
		}else{
			malloc_cnt += sizeof(struct Part);
		}

		//set Part
		for(unsigned int j=0;j<DIM;j++){
			ptmpPart->dim[j][0]=ptmpTreeNode->RootMabj.ab[j][0];
			ptmpPart->dim[j][1]=ptmpTreeNode->RootMabj.ab[j][1];
		}

		CopyBmp(ptmpTreeNode->RootMabj.aLiveRuleBmp,ptmpPart->aLiveRuleBmp);

		//add in leaf node
		ptmpTreeNode->ptrPart=ptmpPart;


		//add in PartList
		if(g_PartList.PartListHead==NULL){
			g_PartList.PartListHead=ptmpPart;
			g_PartList.PartListRear=ptmpPart;
			g_PartList.PartListRear->NextPart=NULL;
		}else{
			g_PartList.PartListRear->NextPart=ptmpPart;
			g_PartList.PartListRear=ptmpPart;
			g_PartList.PartListRear->NextPart=NULL;
		}

		ptmpPart->PartID=g_PartList.nPart;
		g_PartList.nPart++;

	}else{
		/*search leaf node*/
		BuildPartition(ptmpTreeNode->LeftTreeNode);
		BuildPartition(ptmpTreeNode->RightTreeNode);
	}

}



void
BuildPartRFC(struct PartList *ptrPartList){
	struct Part *ptmpPart;
	ptmpPart=ptrPartList->PartListHead;

	for(unsigned int i=0;i<ptrPartList->nPart;i++){
		SetPhase0_Cell(ptmpPart);
		ReOrder_6_33_1_bak(ptmpPart);
		SetPhase1_Cell(ptmpPart);
		SetPhase2_Cell(ptmpPart);
		ptmpPart=ptmpPart->NextPart;
	}
}


