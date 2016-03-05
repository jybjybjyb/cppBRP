//============================================================================
// Name        : BRP.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windows.h>
#include "GlobalVer.h"
#include "GlobalDef.h"
#include "DataProc.h"
#include "BRP.h"
#include "RFC.h"
#include "cRFC.h"
#include "LookUp.h"

using namespace std;


int main(int argc, char* argv[]) {

	/*Loading Data*/
	ReadFilterFile();

	/*Build Tree*/
	PROFILE_START;
	InitTreeRoot(&g_RootNode);
	RBRP(DIM,&g_RootNode);
//	ShowTree();
	printf("Build Tree........");
	PROFILE_END;

	/*Build RFC Partiton*/
	PROFILE_START;
	InitPartList();
	BuildPartition(&g_RootNode);
//	ShowPartition();
	printf("Build RFC Partiton........");
	PROFILE_END;

	/*RFC*/
	PROFILE_START;
	BuildPartRFC(&g_PartList);
	printf("RFC........");
	PROFILE_END;


	Lookup_package();
//	ShowResults();
	StoreResults("RFC_result.txt");
	CountMemory();

#ifdef CRFC
	/*cRFC*/
	PROFILE_START;
	cRFC_6F();
	printf("cRFC........");
	PROFILE_END;
	Lookup_package_cRFC_6F();
	StoreResults("CRFC_result.txt");
	CountMemory_CRFC();
#endif


	return 0;
}
