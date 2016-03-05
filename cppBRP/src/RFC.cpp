/*
 * RFC.cpp
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


/*

 * Function to set bit value (0 or 1), called by SetPhase0_Cell
 * call form : SetBmpBit(bmp,i,TRUE)
 * Return : void
 */
void SetBmpBit(unsigned int *tbmp, unsigned int i, unsigned char flag) {

	unsigned int k, pos;
	k =  i / LENGTH;
	pos = i % LENGTH;
	unsigned int mask = 1;
	mask <<= pos;
	if (flag == 1) {
		tbmp[k] |= mask;
	} else if (flag == 0) {
		mask = ~mask;
		tbmp[k] &= mask;
	} else {
		printf(
				"Error in SetBmpBit(unsigned int *tbmp, unsigned int i, unsigned char flag)");
		exit(1);
	}
}

/*
 Initialize listEqs, called by SetPhase0_Cell
 call form : InitListEqs(phase0_Nodes[i].listEqs)
 return : void
 */
void ResetListEqs(struct LISTEqS *ptrlistEqs) {
	ptrlistEqs->nCES = 0;
	ptrlistEqs->phead = NULL;
	ptrlistEqs->prear = NULL;
}

/*
 Compare two bmp, called by SearchBmp
 return: same -- TRUE ;  different -- FALSE
 */
bool CompareBmp(unsigned int *abmp, unsigned int *bbmp) {
	if ((abmp == NULL) || (bbmp == NULL)) {
		return FALSE;
	}

	/*bmp[32] CBM[32]*/
	for (int i = 0; i < SIZE; i++) {
		if ((*(abmp + i)) != (*(bbmp + i))) {
			return FALSE;
		}
	}

	return TRUE;
}

/*
 Function to search bmp in listEqs, called by SetPhase0_Cell
 call form : SearchBmp(phase0_Nodes[i].listEqs,bmp)
 Return: if tbmp not exist in listEqs, return -1
 else return eqID of CES whose cbm matches tbmp
 */
int SearchBmp(struct LISTEqS *pListEqs, unsigned int *pbmp) {

	//first
	if (pListEqs->phead == NULL) {
		return -1;
	}

	struct CES *tCES;
	tCES = pListEqs->phead;

	/*遍历整个CES*/
	for (int i = 0; i < pListEqs->nCES; i++) {
		if (CompareBmp(tCES->cbm, pbmp)) {
			return i;
		} else {
			tCES = tCES->pnext;
		}
	}
//	printf("no match...\n");
	return -1;
}

/*
 Add new CES to ListEqs, called by SetPhase0_Cell
 call form : AddListEqsCES(phase0_Nodes[i].listEqs,bmp)
 Return : the eqID of the new CES
 */
int AddListEqsCES(struct LISTEqS *ptrlistEqs, unsigned int *ptbmp) {
	struct CES *ptCES;
	ptCES = (CES *) malloc(sizeof(CES));
	if (NULL == ptCES) {
		printf("malloc faild ...\n");
		exit(1);
	}else{
		malloc_cnt += sizeof(CES);
	}

	if (ptrlistEqs->phead == NULL) {
		/*第一个CES*/
		ptCES->eqID = 0;
		for (int i = 0; i < SIZE; i++) {
			ptCES->cbm[i] = ptbmp[i];
		}

		ptCES->pnext = NULL;

		// add new CES to tlistEqs
		ptrlistEqs->phead = ptCES;
		ptrlistEqs->prear = ptCES;
		ptrlistEqs->nCES = 1;
	} else {
		/*非第一个CES*/

		ptCES->eqID = ptrlistEqs->nCES;
		for (int i = 0; i < SIZE; i++) {
			ptCES->cbm[i] = ptbmp[i];
		}
		ptCES->pnext = NULL;

		//挂上
		ptrlistEqs->prear->pnext = ptCES;
		ptrlistEqs->prear = ptCES;
		ptrlistEqs->nCES += 1;
	}

	return (int)(ptrlistEqs->prear->eqID);
}

/*
 Free listEqs space, called by SetPhase1_Cell() & SetPhase2_Cell()
 Function : release space after table is established
 return : void
 */
void FreeListEqs(struct LISTEqS *ptrlistEqs) {
	struct CES *ptCES;

	if (ptrlistEqs->phead == NULL){
		return;
	}


	for (int i = 0; i < ptrlistEqs->nCES; i++) {
		ptCES=ptrlistEqs->phead;
		ptrlistEqs->phead=ptrlistEqs->phead->pnext;
		free(ptCES);
		ptCES=NULL;
	}

	ptrlistEqs->prear = NULL;
	ptrlistEqs->phead = NULL;
}

/*
 Get rule cost number with highest priority, called by SetPhase2_Cell
 Note : used for packet matching more than 1 rules
 call form : cost = GetRuleCost(endBmp)
 return : cost number with highest priority
 */
int GetRuleCost(unsigned int *tbmp) {
	unsigned int tmp,k,pos,MASK;

	for (int i=0; i<MAXFILTERS; i++) {
		k= i/32;
		pos= i%32;
		MASK=1 << pos ;
		tmp=tbmp[k] & MASK ;
		if( tmp !=0){
			return i+1;
		}
	}

	//printf("!!! Lack of default rule!\nThere is no rule matched!\n");
	return -1;
}




/*
 Find proper order to cut memory occupied
 improved by jiangyibo ---2015-08-20
 优化排列组合方法
 */
void
ReOrder_6_33_1_enum(struct Part *tmpPart) {
	//init
	unsigned int con0[6]={0,1,2,3,4,5};
	unsigned int con1[6]={0,2,1,3,4,5};
	unsigned int con2[6]={0,3,1,2,4,5};
	unsigned int con3[6]={0,1,2,3,5,4};
	unsigned int con4[6]={0,2,1,3,5,4};
	unsigned int con5[6]={0,3,1,2,5,4};
	unsigned int *tid;
	unsigned long min=0xFFFFFFFF;
	unsigned long calc;


	/*	ip group, symmetry C42 = 3,
	 * 	port groupsymmetry C21 = 2.
	 * 	2*3=6
	 * */
	for(unsigned int con =0; con<6; con++){
		//con
		switch (con) {
		case 0:
			tid=con0;
			break;
		case 1:
			tid=con1;
			break;
		case 2:
			tid=con2;
			break;
		case 3:
			tid=con3;
			break;
		case 4:
			tid=con4;
			break;
		case 5:
			tid=con5;
			break;
		default:
			break;
		}

		//calc
		calc=(unsigned long) tmpPart->phase0_Nodes[tid[0]].listEqs.nCES \
					* (unsigned long) tmpPart->phase0_Nodes[tid[1]].listEqs.nCES \
					* (unsigned long) tmpPart->phase0_Nodes[tid[4]].listEqs.nCES \
					+ (unsigned long) tmpPart->phase0_Nodes[tid[2]].listEqs.nCES \
					* (unsigned long) tmpPart->phase0_Nodes[tid[3]].listEqs.nCES \
					* (unsigned long) tmpPart->phase0_Nodes[tid[5]].listEqs.nCES;
		if (calc< min){
				min=calc;
				for (int i = 0; i < DIM; i++){tmpPart->dot[i] = tid[i];}
				}
		}
}

/*
 Find proper order to cut memory occupied
 improved by jiangyibo ---2015-08-20
 优化排列组合方法
 */
void
ReOrder_6_33_1_bak(struct Part *tmpPart) {
	unsigned int tid[6];
	unsigned int tmp;
	for(int m=0;m<6;m++){tid[m] = m;}
	for(int m=0;m<6;m++){tmpPart->dot[m] = m;} //dot全局变量

	/*注意类型转换*/
	unsigned long min;
	min=0xFFFFFFFF;
	unsigned long calc;

	for(int i =2; i>=0; i--)
	{

		for(int j =3; j<=5; j++)
		{
			tmp=tid[i];
			tid[i]=tid[j];
			tid[j]=tmp;

			//遍历各种情况，选择min的情况并返给dot[6]全局变量
			calc=(unsigned long) tmpPart->phase0_Nodes[tid[0]].listEqs.nCES \
						* (unsigned long) tmpPart->phase0_Nodes[tid[1]].listEqs.nCES \
						* (unsigned long) tmpPart->phase0_Nodes[tid[2]].listEqs.nCES \
						+ (unsigned long) tmpPart->phase0_Nodes[tid[3]].listEqs.nCES \
						* (unsigned long) tmpPart->phase0_Nodes[tid[4]].listEqs.nCES \
						* (unsigned long) tmpPart->phase0_Nodes[tid[5]].listEqs.nCES;
			if (calc< min){
					min=calc;
					for (int i = 0; i < 6; i++){tmpPart->dot[i] = tid[i];}
					}

				tmp=tid[i];
				tid[i]=tid[j];
				tid[j]=tmp;
			}
	}
}


/*
 Function to fill the table of Phase 0, called by main
 return : void
 */
void
SetPhase0_Cell(struct Part* ptmpPart) {
	//init
	unsigned short line_start,line_end;
//	unsigned int offset;

	/*scan every single field*/
	for (unsigned int j = 0; j < DIM; j++) {

		//init
		unsigned long cell_num;
		unsigned int bmp[SIZE]; // 32*32=1024
		for (unsigned int i = 0; i < SIZE; i++){bmp[i] = 0;}
		ResetListEqs(&ptmpPart->phase0_Nodes[j].listEqs);


		// set line_start line_end
		line_start=ptmpPart->dim[j][0];
		line_end=ptmpPart->dim[j][1];
		cell_num=(unsigned long) (line_end-line_start+1);
		ptmpPart->phase0_Nodes[j].ncells=cell_num;


		//malloc cell[line_end-line_start+1]
		ptmpPart->phase0_Nodes[j].cell = (unsigned short *) malloc(ptmpPart->phase0_Nodes[j].ncells * sizeof(unsigned short));
		if(NULL == ptmpPart->phase0_Nodes[j].cell){
			printf("malloc phase0_Nodes[%d].cell %ld faild ...\n", j, ptmpPart->phase0_Nodes[j].ncells);
			exit(1);
		}else{
			malloc_cnt += ptmpPart->phase0_Nodes[j].ncells * sizeof(unsigned short);
		}

		/*Scan through the number line looking for distinct equivalence classes*/
		for (unsigned int n = (unsigned int)line_start; n < ((unsigned int)line_end)+1 ; n++) {

			//Initialize
			int tempeqID; //小心！！！
			unsigned int tempstart, tempend;

			// See if any rule starts or ends at n
			for (unsigned int i = 0; i < g_FiltSet.numFilters; i++) {

				//alive map
				if(isRuleLive(i,ptmpPart->aLiveRuleBmp)){

					tempstart = g_FiltSet.filtArr[i].dim[j][0];
					tempend = g_FiltSet.filtArr[i].dim[j][1];

					if (tempstart <= n &&  n <= tempend){
						SetBmpBit(bmp, i, 1); //bmp 为(unsign int *) 指向bmp[32]首地址
					}else{
						SetBmpBit(bmp, i, 0);
					}
				}
			} // rules i 遍历结束


			/* 遍历比较已有的CBM和BMP */
			tempeqID = SearchBmp(&ptmpPart->phase0_Nodes[j].listEqs, bmp);

			if (-1 == tempeqID){
				tempeqID = AddListEqsCES(&ptmpPart->phase0_Nodes[j].listEqs, bmp);
			}

			// Set Phase0 Cell bits
			ptmpPart->phase0_Nodes[j].cell[n-(unsigned int)line_start] = (unsigned short) tempeqID;

		} // n 遍历结束

	} //j 遍历结束
}

/*
 Function to fill the table of Phase 1, called by main
 return : void
 */
void
SetPhase1_Cell(struct Part* ptmpPart) {

	Pnoder *tnode1, *tnode2, *tnode3;

	/* gen Chunk of Phase 1*/
	for (int com = 0; com < 2; com++) {
		// Initialize
		unsigned int indx = 0;
		int tempeqID;
		ResetListEqs(&ptmpPart->phase1_Nodes[com].listEqs);

		/*3+3 phase0->1 reduction tree*/
		switch (com) {
		case 0:
			tnode1 = &ptmpPart->phase0_Nodes[ptmpPart->dot[0]];
			tnode2 = &ptmpPart->phase0_Nodes[ptmpPart->dot[1]];
			tnode3 = &ptmpPart->phase0_Nodes[ptmpPart->dot[2]];
			break;
		case 1:
			tnode1 = &ptmpPart->phase0_Nodes[ptmpPart->dot[3]];
			tnode2 = &ptmpPart->phase0_Nodes[ptmpPart->dot[4]];
			tnode3 = &ptmpPart->phase0_Nodes[ptmpPart->dot[5]];
			break;
		default:
			break;
		}

		// alloc memory for Phase1 cell
		unsigned long cellNum;
		cellNum = (unsigned long)tnode1->listEqs.nCES * \
				(unsigned long)tnode2->listEqs.nCES * \
				(unsigned long)tnode3->listEqs.nCES;
		ptmpPart->phase1_Nodes[com].ncells = cellNum;

		ptmpPart->phase1_Nodes[com].cell = (unsigned short *) malloc(cellNum * sizeof(unsigned short));
		if(NULL == ptmpPart->phase1_Nodes[com].cell){
			printf("malloc faild ...\n");
			exit(1);
		}else{
			malloc_cnt +=cellNum * sizeof(unsigned short);
		}

		// generate phase1_Nodes[com]->listEqs
		CES *tCES1, *tCES2, *tCES3;
		unsigned int intersectedBmp[SIZE]; //32*32=1024 bit

		tCES1 = tnode1->listEqs.phead;
		for (int i = 0; i < tnode1->listEqs.nCES; i++) {

			tCES2 = tnode2->listEqs.phead;
			for (int j = 0; j < tnode2->listEqs.nCES; j++) {

				tCES3 = tnode3->listEqs.phead;
				for (int k = 0; k < tnode3->listEqs.nCES; k++) {

					// generate intersectedBmp
					for (int m = 0; m < SIZE; m++){
						intersectedBmp[m] = tCES1->cbm[m] & tCES2->cbm[m] & tCES3->cbm[m];
					}

					// return -1 if not exist, else return eqID
					tempeqID = SearchBmp(&ptmpPart->phase1_Nodes[com].listEqs, intersectedBmp);

					// Not exist, add intersectedBmp to listEqs
					if (-1 == tempeqID){
						tempeqID = AddListEqsCES(&ptmpPart->phase1_Nodes[com].listEqs, intersectedBmp);
					}

					// Set Phase1 Cell bits
					ptmpPart->phase1_Nodes[com].cell[indx] = (unsigned short) tempeqID;
					indx++;

					tCES3 = tCES3->pnext;
				}
				tCES2 = tCES2->pnext;
			}
			tCES1 = tCES1->pnext;
		}


		// Release listEqs Space
		FreeListEqs(&tnode1->listEqs);
		FreeListEqs(&tnode2->listEqs);
		FreeListEqs(&tnode3->listEqs);

	}/*end com, generated chunk of phase1*/

}


/*
 Function to fill the table of Phase 2, called by main
 return : void
 */
void
SetPhase2_Cell(struct Part* ptmpPart) {

	unsigned int indx = 0;
	Pnoder *tnode1, *tnode2;
	CES *tCES1, *tCES2;
	unsigned int endBmp[SIZE];
	int tempeqID;
	unsigned int cost;	// cost number with highest priority

	tnode1 = &ptmpPart->phase1_Nodes[0];
	tnode2 = &ptmpPart->phase1_Nodes[1];

	// Initialize phase2_Node.listEqs
	ResetListEqs(&ptmpPart->phase2_Nodes.listEqs);

	// alloc memory for Phase1 cell
	unsigned long cellNum;
	cellNum = (unsigned long)tnode1->listEqs.nCES * \
			(unsigned long)tnode2->listEqs.nCES;
	ptmpPart->phase2_Nodes.ncells = cellNum;
	ptmpPart->phase2_Nodes.cell = (unsigned short *) malloc(cellNum * sizeof(unsigned short));
	if(NULL == ptmpPart->phase2_Nodes.cell){
		printf("malloc faild ......");
		exit(1);
	}

	tCES1 = tnode1->listEqs.phead;
	for (int i = 0; i < tnode1->listEqs.nCES; i++) {

		tCES2 = tnode2->listEqs.phead;
		for (int j = 0; j < tnode2->listEqs.nCES; j++) {

			// generate endBmp
			/*chunk in phase1 的 CBM 叉乘*/
			for (int m = 0; m < SIZE; m++){
				endBmp[m] = tCES1->cbm[m] & tCES2->cbm[m];
			}

			tempeqID=-1;
			tempeqID = SearchBmp(&ptmpPart->phase2_Nodes.listEqs, endBmp);
			if (-1 == tempeqID){
				tempeqID = AddListEqsCES(&ptmpPart->phase2_Nodes.listEqs, endBmp);
			}

			// Get rule cost number with highest priority
			cost = GetRuleCost(endBmp);
			ptmpPart->phase2_Nodes.cell[indx] = cost;
			indx++;

			tCES2 = tCES2->pnext;
		}
		tCES1 = tCES1->pnext;
	}

	// Release listEqs Space
	FreeListEqs(&tnode1->listEqs);
	FreeListEqs(&tnode2->listEqs);
}





