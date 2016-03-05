/*
 * DataProc.cpp
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


void ReadIPRange(FILE *fp, unsigned int* highRange, unsigned int* lowRange) {
	/*assumes IPv4 prefixes*/
	// temporary variables to store IP range
	unsigned int trange[4];
	unsigned int mask;
	char validslash;

	// read IP range described by IP/mask
//	fscanf(fp, "%d.%d.%d.%d/%d", &trange[0],&trange[1],&trange[2],&trange[3],&mask);
	fscanf(fp, "%d.%d.%d.%d", &trange[0], &trange[1], &trange[2], &trange[3]);
	fscanf(fp, "%c", &validslash);

	// deal with default mask
	if (validslash != '/')
		mask = 32;
	else
		fscanf(fp, "%d", &mask);

	int masklit1;
	unsigned int masklit2, masklit3;
	mask = 32 - mask;
	masklit1 = mask / 8;
	masklit2 = mask % 8;

	unsigned int ptrange[4];
	int i;
	for (i = 0; i < 4; i++)
		ptrange[i] = trange[i];

	// count the start IP
	for (i = 3; i > 3 - masklit1; i--)
		ptrange[i] = 0;
	if (masklit2 != 0) {
		masklit3 = 1;
		masklit3 <<= masklit2;
		masklit3 -= 1;
		masklit3 = ~masklit3;
		ptrange[3 - masklit1] &= masklit3;
	}
	// store start IP
	highRange[0] = ptrange[0];
	highRange[0] <<= 8;
	highRange[0] += ptrange[1];
	lowRange[0] = ptrange[2];
	lowRange[0] <<= 8;
	lowRange[0] += ptrange[3];

	// count the end IP
	for (i = 3; i > 3 - masklit1; i--)
		ptrange[i] = 255;
	if (masklit2 != 0) {
		masklit3 = 1;
		masklit3 <<= masklit2;
		masklit3 -= 1;
		ptrange[3 - masklit1] |= masklit3;
	}
	// store end IP
	highRange[1] = ptrange[0];
	highRange[1] <<= 8;
	highRange[1] += ptrange[1];
	lowRange[1] = ptrange[2];
	lowRange[1] <<= 8;
	lowRange[1] += ptrange[3];
}

/* Read protocol, called by ReadFilter
 fp: pointer to filter set file
 protocol: 17 for tcp
 return: void*/
void ReadProtocol(FILE *fp, unsigned char *from, unsigned char *to) {
	unsigned int tfrom, tto;

	fscanf(fp, "%d : %d", &tfrom, &tto);
	*from = (unsigned char) tfrom;
	*to = (unsigned char) tto;
}

/* Read port, called by ReadFilter
 fp: pointer to filter set file
 from:to	=>	0:65535 : specify the port range
 return: void*/
void ReadPort(FILE *fp, unsigned int *from, unsigned int *to) {
	unsigned int tfrom;
	unsigned int tto;

	fscanf(fp, "%d : %d", &tfrom, &tto);

	*from = tfrom;
	*to = tto;
}

/* Read port, called by ReadFilter
 fp: pointer to filter set file
 from:to	=>	0:65535 : specify the port range
 return: void*/
void ReadAct(FILE *fp, unsigned char *action) {
	unsigned int tAction;

	fscanf(fp, "%d", &tAction);

	*action = (unsigned char) tAction;
}

/* ***	function for loading filters   ***
 fp:		file pointer to filterset file
 filtset: pointer to filterset, global variable
 cost:	the cost(position) of the current filter
 cost就是line的排序
 return:	0, this value can be an error code...*/
int
ReadFilter(FILE *fp, struct FILTSET * filtset, unsigned int cost) {
	char validfilter; // validfilter means an '@'
	struct FILTER *ptempfilt, tempfilt;
	ptempfilt = &tempfilt;

	while (!feof(fp)) {
		fscanf(fp, "%c", &validfilter);
		if (validfilter != '@') {
			continue;	// each rule should begin with an '@'
		}

		for(unsigned int i=0; i<DIM;i++){
			fscanf(fp, "%d:%d", &ptempfilt->dim[i][0], &ptempfilt->dim[i][1]);
		}

		// copy the temp filter to the global one
		memcpy(&(filtset->filtArr[filtset->numFilters]), ptempfilt,
				sizeof(struct FILTER));

		filtset->numFilters++;

		return SUCCESS;
	} //ending while, rule set 文件逐行读取并转换成filter形式

	return FALSE;
}

/* ***	function for loading filters   ***
 fp:		file pointer to filterset file
 filtset: pointer to filterset, global variable
 return:	void*/
void
LoadFilters(FILE *fp, struct FILTSET * filtset) {

	filtset->numFilters = 0;	// initial filter number

	int line = 0;// the line to read, indeed, this is the cost(position) of the filter to read
	while (!feof(fp)) {
		ReadFilter(fp, filtset, line);
		line++;
	}

}

/* Load Filters from file, called by main
 return: void*/
void
ReadFilterFile() {
	FILE *fp;	// filter set file pointer
	char filename[] = "filter.txt";
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Couldnt open filter set file \n");
		exit(1);
	}
	printf("........ Filter File %s Loading  ........ \n", filename);

	LoadFilters(fp, &g_FiltSet);	// loading filters...
	fclose(fp);
	printf("Filters Read %d Rules\n", g_FiltSet.numFilters);

	// check whether bmp[SIZE] is long enough to provide one bit for each rule
	if (LENGTH * SIZE < g_FiltSet.numFilters) {
		printf(
				"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nThe bmp[SIZE] is not long enougth, please set SIZE higher!!!\n");
		exit(1);
	}

}


// Load Package Set into memory
void
LoadPackages(FILE *fp, struct PACKAGESET *packageset) {
	packageset->numPackages = 0;	// initial package number
	char validfilter;				// validfilter means an '@'
	struct PACKAGE * ptemppack;

	while (!feof(fp)) {
		fscanf(fp, "%c", &validfilter);

		if (validfilter != '@'){
			continue;	// each rule should begin with an '@'
			}

		// malloc
		ptemppack=(struct PACKAGE *)malloc(sizeof(struct PACKAGE));
		if(NULL == ptemppack){
			printf("(malloc) load pack error !!!\n");
			exit(1);
		}else{
			malloc_cnt += sizeof(struct PACKAGE);
		}

		for(unsigned int i=0; i<DIM; i++){
			fscanf(fp, "%d", &ptemppack->dim[i]);
		}



		if(packageset->numPackages == 0){
			packageset->pPackageSetHead=ptemppack;
			packageset->pPackageSetRear=ptemppack;
			ptemppack->pNextPack=NULL;
		}else{

			packageset->pPackageSetRear->pNextPack=ptemppack;
			packageset->pPackageSetRear=packageset->pPackageSetRear->pNextPack;
			ptemppack->pNextPack=NULL;
		}

		packageset->numPackages++;

	}//end while

}


void
ReadPackFile() {
	// Read packages from file packageset.txt
	FILE *fp;						// filter set file pointer
	char filename[] = "ip.txt";
	fp = fopen(filename,"r");
	if (fp == NULL)
	{
	printf("Cannot open package set file \n");
	exit (0);
	}
//	exit (0);
	LoadPackages(fp, &g_PackageSet);	// loading packages...
	fclose(fp);
	printf("Read %d Packs\n", g_PackageSet.numPackages);
}

/*



 * save preprocessing result to chunkdata.txt
 * saving cell[65535]
 * saving CES (consisting of eqId and CBM)
 *

void
SaveTemporary() {
	FILE *fp;
	char filename[] = "RFC.tmp";
	fp = fopen(filename, "w+");
	if (fp == NULL) {
		printf("Cannot open chunkdata.txt file \n");
		exit(1);
	}


	save global ver
	for(unsigned int i=0;i<DIM;i++){
		fprintf(fp, "%d\t",	g_split_state[i] );
	}

	fprintf(fp, "%d\t",	g_PartList.nPart);

	save parrtition
	struct Part *ptmpPart;
	ptmpPart=g_PartList.PartListHead;

	for(unsigned int n=0;n<g_PartList.nPart;n++){
		//trace partlist

		fprintf(fp, "%d\t",	ptmpPart->PartID);
		save start end
		for(unsigned int i=0;i<DIM;i++){
			for(unsigned int j=0;j<2;j++){
				fprintf(fp, "%d\t",	ptmpPart->dim[i][j]);
			}
		}

		save alivebmp
		for(unsigned int i=0;i<SIZE;i++){
			fprintf(fp, "%d\t",	ptmpPart->aLiveRuleBmp[i]);
		}

		save dot
		for(unsigned int i=0;i<DIM;i++){
			fprintf(fp, "%d\t",	ptmpPart->dot[i] );
		}


		save phase0
		for(unsigned int i=0;i<DIM;i++){
			//trace phase0_Nodes[DIM]

			fprintf(fp, "%d\t", ptmpPart->phase0_Nodes[i].listEqs.nCES);
			fprintf(fp, "%d\t", ptmpPart->phase0_Nodes[i].ncells);
			for(unsigned int j=0;j<ptmpPart->phase0_Nodes[i].ncells;j++){
				//trace cell
				fprintf(fp, "%d\t", ptmpPart->phase0_Nodes[i].cell[j]);
			}

		}

		save phase1
		for(unsigned int i=0;i<2;i++){
			//trace phase0_Nodes[DIM]

			fprintf(fp, "%d\t", ptmpPart->phase1_Nodes[i].listEqs.nCES);
			fprintf(fp, "%d\t", ptmpPart->phase1_Nodes[i].ncells);
			for(unsigned int j=0;j<ptmpPart->phase1_Nodes[i].ncells;j++){
				//trace cell
				fprintf(fp, "%d\t", ptmpPart->phase1_Nodes[i].cell[j]);
			}

		}

		save phase2
		fprintf(fp, "%d\t", ptmpPart->phase2_Nodes.ncells);
		for(unsigned int j=0;j<ptmpPart->phase2_Nodes.ncells;j++){
			//trace cell
			fprintf(fp, "%d\t", ptmpPart->phase2_Nodes.cell[j]);
		}

		//next
		ptmpPart=ptmpPart->NextPart;
	}
	// Save phase0 chunks

	fclose(fp);
}



 *  load preprocessing result from chunkdata.txt

void
LoadTempprary() {
	FILE *fp;
	char filename[] = "RFC.tmp";
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open chunkdata.txt file \n");
		exit(1);
	}



	struct Part *ptmpPart;
	ptmpPart=g_PartList.PartListHead;
	for(unsigned int n=0;n<g_PartList.nPart;n++){
		//trace partlist

		save phase0
		for(unsigned int i=0;i<DIM;i++){
			//trace phase0_Nodes[DIM]

			fprintf(fp, "%d\t", ptmpPart->phase0_Nodes[i].listEqs.nCES);
			fprintf(fp, "%d\t", ptmpPart->phase0_Nodes[i].ncells);
			for(unsigned int j=0;j<ptmpPart->phase0_Nodes[i].ncells;j++){
				//trace cell
				fprintf(fp, "%d\t", ptmpPart->phase0_Nodes[i].cell[j]);
			}

		}

		save phase1
		for(unsigned int i=0;i<2;i++){
			//trace phase0_Nodes[DIM]

			fprintf(fp, "%d\t", ptmpPart->phase1_Nodes[i].listEqs.nCES);
			fprintf(fp, "%d\t", ptmpPart->phase1_Nodes[i].ncells);
			for(unsigned int j=0;j<ptmpPart->phase1_Nodes[i].ncells;j++){
				//trace cell
				fprintf(fp, "%d\t", ptmpPart->phase1_Nodes[i].cell[j]);
			}

		}

		save phase2
		fprintf(fp, "%d\t", ptmpPart->phase2_Nodes.ncells);
		for(unsigned int j=0;j<ptmpPart->phase2_Nodes.ncells;j++){
			//trace cell
			fprintf(fp, "%d\t", ptmpPart->phase2_Nodes.cell[j]);
		}

		//next
		ptmpPart=ptmpPart->NextPart;
	}
	// Save phase0 chunks

	fclose(fp);
}
*/

void
ShowResults(){
	printf("\n______________________Results__________________\n");
	printf("%-12s%-12s\n","RuleIndx","PartIndex");
	for(unsigned int i =0;i<g_PackageSet.numPackages;i++){
		printf("%-12d%-12d\n",gptr_ResultSet[i].RuleIndx,\
				gptr_ResultSet[i].PartIndex);

	}
}

void
StoreResults(char *FileName){

	FILE *fp;
//	char filename1[]; = *FileName;
	fp = fopen(FileName,"w+");
	if (fp == NULL)
	{
	printf("Cannot open lookupResult file \n");
	exit (0);
	}

	fprintf(fp,"\n_____________________%s__________________\n",FileName);
	fprintf(fp,"%-12s%-12s\n","RuleIndx","PartIndex");
	for(unsigned int i =0;i<g_PackageSet.numPackages;i++){
		fprintf(fp,"%-12d%-12d\n",gptr_ResultSet[i].RuleIndx,\
				gptr_ResultSet[i].PartIndex);

	}

	fclose(fp);

	printf("Storing %d Results\n", g_PackageSet.numPackages);
 }


/*
 * count memory : me mory occupied by chunks
 */
void
CountMemory() {

	printf("\n__________________________Memory_________________________\n");


	struct Part *ptmpPart;
	ptmpPart=g_PartList.PartListHead;

	unsigned int nPart;
	nPart=g_PartList.nPart;
	unsigned int single_p0_cellused;
	unsigned int single_p1_cellused;
	unsigned int single_p2_cellused;
	unsigned int tot_p0_cellused=0;
	unsigned int tot_p1_cellused=0;
	unsigned int tot_p2_cellused=0;
	unsigned int tot=0;
	unsigned int numbits,single_tot;

	numbits = sizeof(unsigned short);
	unsigned int part_used[nPart];
	unsigned int cnt[nPart];

	for(unsigned int i =0; i<nPart;i++){
		single_tot=0;
		single_p0_cellused=0;
        single_p1_cellused=0;
		single_p2_cellused=0;

		for(unsigned int j =0; j<DIM;j++){
			single_p0_cellused += ptmpPart->phase0_Nodes[j].ncells* numbits;
		}

		for(unsigned int k =0; k<2;k++){
			single_p1_cellused += ptmpPart->phase1_Nodes[k].ncells* numbits;
		}

		single_p2_cellused += ptmpPart->phase2_Nodes.ncells* numbits;

		single_tot+=single_p0_cellused;
		single_tot+=single_p1_cellused;
		single_tot+=single_p2_cellused;
		part_used[i]=single_tot;

        tot_p0_cellused+=single_p0_cellused;
        tot_p1_cellused+=single_p1_cellused;
        tot_p2_cellused+=single_p2_cellused;
        tot+=single_tot;

		cnt[i]=CountRuleSet(ptmpPart->aLiveRuleBmp);
		ptmpPart=ptmpPart->NextPart;
	}



	printf("\n____partiton____");
	printf("\n%-12s%-12s%-12s%-12s \n","PartID","MemoryUsed","%","RuleCnt");
	for(unsigned int n=0;n<nPart;n++){
		printf("%-12d%-12d%-12.1d%-12d [Bytes] \n", n,part_used[n], part_used[n]*100/tot, cnt[n]);

	}

	printf("\n____totally____");
	printf("\n%-12s%-12s%-12s \n","phase0","phase1","phase2");
	printf("%-12d%-12d%-12d [Bytes] \n", tot_p0_cellused,tot_p1_cellused,tot_p2_cellused );

	printf("\nMemory Used Totally: %d Bytes\n", tot );
	printf("\nMaximum Malloc: %d Bytes\n", malloc_cnt );

/*	unsigned int n_node;
	unsigned int node_size;

	n_node=28;
	node_size=sizeof(struct TreeNode);
	printf("\nTree used: %d Bytes\n", n_node*node_size);*/



	// store memory used int memoryused.txt
	FILE *fp;
	char filename[] = "RFC_memory.txt";
	fp = fopen(filename, "w+");
	if (fp == NULL) {
		printf("Cannot open memoryused file \n");
		exit(1);
	}


	fprintf(fp,"\n____partiton____");
	fprintf(fp,"\n%-12s%-12s%-12s%-12s \n","PartID","MemoryUsed","%","RuleCnt");
	for(unsigned int n=0;n<nPart;n++){
		fprintf(fp,"%-12d%-12d%-12.1d%-12d [Bytes] \n", n,part_used[n], part_used[n]*100/tot, cnt[n]);

	}

	fprintf(fp,"\n____totally____");
	fprintf(fp,"\n%-12s%-12s%-12s \n","phase0","phase1","phase2");
	fprintf(fp,"%-12d%-12d%-12d [Bytes] \n", tot_p0_cellused,tot_p1_cellused,tot_p2_cellused );

	fprintf(fp,"\nMemory Used Totally: %d Bytes\n", tot );
	fprintf(fp,"\nMaximum Malloc: %d Bytes\n", malloc_cnt );


	fclose(fp);
}



