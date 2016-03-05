/*
 * GlobalVer.h
 *
 *  Created on: 2015Äê9ÔÂ24ÈÕ
 *      Author: 10177270
 */

#ifndef CPP_BRP_CRFC_6F_EXP_SRC_GLOBALVER_H_
#define CPP_BRP_CRFC_6F_EXP_SRC_GLOBALVER_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windows.h>
#include "GlobalDef.h"
#include "DataProc.h"
#include "BRP.h"
#include "RFC.h"


/************** global parameter definition ****************/
unsigned long long dff;
unsigned long c1;
unsigned long c2;
LARGE_INTEGER  large_interger;

// structure for data
struct FILTSET g_FiltSet;  //Rule set
struct PACKAGESET g_PackageSet; //Ip set

// structure for BRP
struct TreeNode g_RootNode; //BRP Tree Root Node
int g_split_state[DIM];

// structure for RFC
struct PartList g_PartList;

#ifdef cRFC
// structure for cRFC
//struct CompressList g_CompressList;
#endif



//LookResult
struct LookupResult *gptr_ResultSet;

//malloc
unsigned int malloc_cnt=0;



#endif /* CPP_BRP_CRFC_6F_EXP_SRC_GLOBALVER_H_ */
