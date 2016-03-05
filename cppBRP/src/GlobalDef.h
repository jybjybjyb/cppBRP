/*
 * global_def.h
 *
 *  Created on: 2015Äê9ÔÂ24ÈÕ
 *      Author: 10177270
 */

#ifndef GLOBAL_DEF_H_
#define GLOBAL_DEF_H_


/*************************** Macro Definition *********************/
#define MAXFILTERS	1000	// maximum amount of filters, 1000 for test
#define MAXPACKAGES 1000	// maximum amount of packages, 1000 for test

#define TRUE			1
#define FALSE			0
#define SUCCESS         1
#define LENGTH			32				// length of unsigned int
#define SIZE			32			    // SIZE = ceiling ( rules / LENGTH )
#define DIM_RANGE  65536
#define DIM  6

/**************  IF DEF ****************/
#define CRFC

/************** Global Declaration ****************/

 // structure for data
extern struct FILTSET g_FiltSet;  //Rule set
extern struct PACKAGESET g_PackageSet; //Ip set

 // structure for BRP
extern struct TreeNode g_RootNode; //BRP Tree Root Node
extern int g_split_state[DIM];

 // structure for RFC
extern struct PartList g_PartList;

#ifdef cRFC
// structure for cRFC
//extern struct CompressList g_CompressList;
#endif


 //LookResult
extern struct LookupResult *gptr_ResultSet;

 //malloc
extern unsigned int malloc_cnt;

 // Find proper order


 // Find proper order
extern int *g_Split[DIM];


/*************************** Profile *********************/
extern unsigned long long dff;
extern unsigned long c1;
extern unsigned long c2;
extern LARGE_INTEGER  large_interger;

#define PROFILE_START 		\
						do{\
							QueryPerformanceFrequency(&large_interger);\
							dff = large_interger.QuadPart;\
							QueryPerformanceCounter(&large_interger);\
							c1 = large_interger.QuadPart;\
						}while(0)


#define PROFILE_END 		\
						do{\
							QueryPerformanceCounter(&large_interger);\
							c2 = large_interger.QuadPart;\
							cout<<"Time Cost: "<<(unsigned long long)(c2 - c1) / dff<<" [s]"<<endl;\
						}while(0)



#endif /* GLOBAL_DEF_H_ */
