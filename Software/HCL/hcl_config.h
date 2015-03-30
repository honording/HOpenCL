#include "xparameters.h"
#include "xstatus.h"
#include "xbram.h"
#include "xtmrctr.h"

#ifdef  _SLAVE_
#include "xaxicdma.h"
#include "xlocalidgenerator.h"
#include "xbarrier.h"
#endif

#include "xgroupidgenerator.h"

#ifdef _HOST_
#include "xgroupnumberallocator.h"
#include "xlocalcoreidallocator.h"
#endif

//Status
#define GROUPDEFAULT    0
#define GROUPSTART      1
#define GROUPFINISH     2
#define KERNELFINISH    1
#define DMADONE         1      
//
#define DIMENSION       2

#define NUMOFLOCALCORE	4
#define NUMOFGROUP 4
#define TOTALLOCALCORE	4
//#define GROUP0
//#define GROUP1
//#define GROUP2
//#define GROUP3
//#define GROUP4
//#define GROUP5
//#define GROUP6

//offset in GSMem
#define OFFKERNELARG    0       //size 256
#define OFFLOCALSIZE    256     //size 2
#define OFFGROUPSIZE    258     //size 2
#define OFFGROUPSTART   260     //size 64
#define OFFPERFORMANCE	324		//size 64
#define OFFDEBUGG		388		//size 256

//
#ifdef _SLAVE_
//offset in LSMem
#define OFFSLAVESTART   0       //size 64
#define OFFGROUPID      64      //size 2
#define OFFKERNELFINISH 66      //size 1
#define OFFDMA          67      //size 256
#define OFFDEBUGL		323		//size 256

unsigned int _groupNum;
unsigned int _localSize[DIMENSION];
unsigned int _totalLocalItem;
unsigned int _itemPerSlave;
//unsigned int _currLocalID[DIMENSION];
unsigned int _groupID[DIMENSION];
unsigned int _localID[DIMENSION];
#endif
unsigned int _coreID;
unsigned int _performance[TOTALLOCALCORE];

XBram                       GSMem;
XBram_Config                *GSMemPtr;
#ifdef  _SLAVE_
XBram                       LocalMem;
XBram_Config                *LocalMemPtr;
XBram                       LSMem;
XBram_Config                *LSMemPtr;
XAxiCdma                    CDMA;
XAxiCdma_Config             *CDMAPtr;
XLocalidgenerator           HlsLocalidgenerator;
XLocalidgenerator_Config    *LocalidgeneratorPtr;
XGroupidgenerator           HlsGroupidgenerator;
XGroupidgenerator_Config    *GroupidgeneratorPtr;
XBarrier					HlsBarrier;
XBarrier_Config				*BarrierPtr;
#endif

#ifdef _HOST_
XTmrCtr timer;
XGroupidgenerator          	 	HlsGroupidgenerator;
XGroupidgenerator_Config    	*GroupidgeneratorPtr;
XGroupnumberallocator 			HlsGroupnumallocator;
XGroupnumberallocator_Config	*GroupnumallocatorPtr;
XLocalcoreidallocator			HlsLocalcoreidallocator[NUMOFGROUP];
XLocalcoreidallocator_Config	*LocalcoreidallocatorPtr[NUMOFGROUP];
#endif

/* Write to memory location or register */
#define WriteMem(BASE_ADDRESS, RegOffset, data) \
           *((unsigned int *)(BASE_ADDRESS) + (RegOffset)) = ((unsigned int)(data))
/* Read from memory location or register */
#define ReadMem(BASE_ADDRESS, RegOffset) \
           *((unsigned int *)(BASE_ADDRESS) + (RegOffset))

