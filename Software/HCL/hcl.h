#include "fsl.h"
#include "hcl_config.h"
#include "hcl_debug.h"

unsigned int dNum = DEBUGNUM;

unsigned int initCore();
void getCoreID();
#ifdef _SLAVE_
void barrier(unsigned int id);
unsigned int getLocalSize(unsigned int dim);
unsigned int isLocalHost();
unsigned int isLocalSlave();
void getGroupNum();
void cleanupSlave();
unsigned int getKernelArg(unsigned int argNum);
unsigned int getGroupID(unsigned int dim);
unsigned int getLocalID(unsigned int dim);
unsigned int getGlobalID(
        unsigned int dim,
        unsigned int groupID0,
        unsigned int groupID1,
        unsigned int localID0,
        unsigned int localID1,
        unsigned int localSize0,
        unsigned int localSize1
        );
unsigned int isCurrKernelFinish();
unsigned int isCurrGroupFinish();
unsigned int simpleDMA(unsigned int id,unsigned int src,unsigned int dst,unsigned int len);
#endif
#ifdef	_HOST_
void kernelStart();
void kernelFinish();
void setKernelArg(unsigned int argNum, unsigned int val);
void setLocalSize(unsigned int dim0, unsigned int dim1);
void setGroupSize(unsigned int dim0, unsigned int dim1);
#endif


void getCoreID() {
#ifdef	_SLAVE_
    getfslx(_coreID,0,FSL_DEFAULT);  //get value from FSL 0
#endif
#ifdef	_HOST_
    _coreID = 714;
#endif
}

unsigned int initCore() {
	getCoreID();
    int status = XST_SUCCESS;
    int i;
    //HOST and SLAVES shared opperations
    //GSMem
    GSMemPtr = XBram_LookupConfig(0);
    if (!GSMemPtr)
        return XST_FAILURE;
    status = XBram_CfgInitialize(&GSMem,GSMemPtr,GSMemPtr->MemBaseAddress);
    if (status!=XST_SUCCESS)
        return XST_FAILURE;
    if (_coreID==dNum) xil_printf("Core %d finish initializing GSMem\n\r",dNum);
#ifdef  _HOST_
    //Timer Config
    status = XTmrCtr_Initialize(&timer, 0);
    if (status != XST_SUCCESS)
    	return XST_FAILURE;
    XTmrCtr_SetResetValue(&timer, 0, 0);
    XTmrCtr_Start(&timer, 0);
    //GroupIDGenerator config
	GroupidgeneratorPtr = XGroupidgenerator_LookupConfig(0);
    if (!GroupidgeneratorPtr)
		return XST_FAILURE;
    status = XGroupidgenerator_CfgInitialize(&HlsGroupidgenerator,GroupidgeneratorPtr);
    if (status != XST_SUCCESS)
        return XST_FAILURE;	
    //GroupNumAllocator config
    GroupnumallocatorPtr = XGroupnumberallocator_LookupConfig(0);
    if (!GroupnumallocatorPtr)
    	return XST_FAILURE;
    status = XGroupnumberallocator_CfgInitialize(&HlsGroupnumallocator,GroupnumallocatorPtr);
    if (status != XST_SUCCESS)
    	return XST_FAILURE;
    //LocalIDAllocator config
    for (i=0;i<NUMOFGROUP;i++) {
    	LocalcoreidallocatorPtr[i] = XLocalcoreidallocator_LookupConfig(i);
    	if (!LocalcoreidallocatorPtr[i])
    		return XST_FAILURE;
    	status = XLocalcoreidallocator_CfgInitialize(&HlsLocalcoreidallocator[i],LocalcoreidallocatorPtr[i]);
    	if (status != XST_SUCCESS)
    		return XST_FAILURE;
    }
#endif
#ifdef  _SLAVE_
    _performance[_coreID]=0;
    getGroupNum();
    //Local HOST and Slaves shared operation:
    //LocalMem
    //LSMem
	LSMemPtr = XBram_LookupConfig(1);
    if (!LSMemPtr)
        return XST_FAILURE;
    status = XBram_CfgInitialize(&LSMem,LSMemPtr,LSMemPtr->MemBaseAddress);
    if (status!=XST_SUCCESS)
        return XST_FAILURE;
    //LocalMem
	LocalMemPtr = XBram_LookupConfig(2);
    if (!LocalMemPtr)
        return XST_FAILURE;
    status = XBram_CfgInitialize(&LocalMem,LocalMemPtr,LocalMemPtr->MemBaseAddress);
    if (status!=XST_SUCCESS)
        return XST_FAILURE;
    //LocalIDGenerator config
	LocalidgeneratorPtr = XLocalidgenerator_LookupConfig(0);
    if (!LocalidgeneratorPtr)
        return XST_FAILURE;
    status = XLocalidgenerator_CfgInitialize(&HlsLocalidgenerator,LocalidgeneratorPtr);
    if (status != XST_SUCCESS)
        return XST_FAILURE;
    //local HOST exclusive operation:
    //CDMA
    //GroupIDGenerator
    if (isLocalHost()) {      //slave but localHost
    	//Barrier config
    	BarrierPtr =  XBarrier_LookupConfig(0);
    	if (!BarrierPtr)
    		return XST_FAILURE;
    	status = XBarrier_CfgInitialize(&HlsBarrier,BarrierPtr);
    	if (status != XST_SUCCESS)
    		return XST_FAILURE;
        //CDMA config
		CDMAPtr = XAxiCdma_LookupConfig(0);
        if (!CDMAPtr)
            return XST_FAILURE;
        status = XAxiCdma_CfgInitialize(&CDMA,CDMAPtr,CDMAPtr->BaseAddress);
        if (status != XST_SUCCESS)
            return XST_FAILURE;
        XAxiCdma_IntrDisable(&CDMA, XAXICDMA_XR_IRQ_ALL_MASK);
        //GroupIDGenerator config
		GroupidgeneratorPtr = XGroupidgenerator_LookupConfig(0);
        if (!GroupidgeneratorPtr)
            return XST_FAILURE;
        status = XGroupidgenerator_CfgInitialize(&HlsGroupidgenerator,GroupidgeneratorPtr);
        if (status != XST_SUCCESS)
            return XST_FAILURE; 
		//sent a seed to groupidgenerator
		putfslx(_groupNum,3,FSL_DEFAULT);
        if (_coreID==dNum) xil_printf("Core %d finish initializing All peripherals, waiting for HOST to triger\n\r",dNum);
        //wait for global HOST to give start signal
        unsigned int runTrigger=GROUPDEFAULT;
        while (runTrigger != GROUPSTART)
            runTrigger = ReadMem(GSMem.Config.MemBaseAddress,OFFGROUPSTART+_groupNum);
    }

    barrier(0);  //Block all slaves in one group to wait for signal to start!

    //update group shared variables
    _localSize[0] = ReadMem(GSMem.Config.MemBaseAddress,OFFLOCALSIZE+0);
    _localSize[1] = ReadMem(GSMem.Config.MemBaseAddress,OFFLOCALSIZE+1);
#endif
    return status;
}

#ifdef _SLAVE_
void barrier(unsigned int id) {
    unsigned int temp;
    if (_coreID==dNum) xil_printf("Core %d start barrier:%d\n\r",dNum,id);
    if (isLocalHost()) {				//local host
    	XBarrier_Set_numCore(&HlsBarrier,NUMOFLOCALCORE);
    	XBarrier_Start(&HlsBarrier);
    }
    putfslx(1,1,FSL_DEFAULT);       //get value from FSL 1
    getfslx(temp,1,FSL_DEFAULT);
    if (_coreID==dNum) xil_printf("Core %d release barrier:%d\n\r",dNum,id);
}
unsigned int getLocalSize(unsigned int dim) {
    if (_coreID==dNum) xil_printf("Core %d get local size dim %d=%d\n\r",dNum,dim,_localSize[dim]);
    return _localSize[dim];
}


unsigned int isLocalHost() {
    return (_coreID==0)?1:0;
}
unsigned int isLocalSlave() {
	return (_coreID==0)?0:1;
}

void getGroupNum() {
    if (isLocalHost()) {                      //Local Host
        getfslx(_groupNum,6,FSL_DEFAULT);    //get value from FSL 6
        if (_coreID==dNum) xil_printf("Core %d get groupNum%d\n\r",dNum,_groupNum);
    }
}
void cleanupSlave() {
//    barrier(2);
	WriteMem(GSMem.Config.MemBaseAddress,OFFPERFORMANCE+_coreID,_performance[_coreID]);
    if (isLocalHost())
        WriteMem(GSMem.Config.MemBaseAddress,OFFGROUPSTART+_groupNum,GROUPFINISH);
    if (_coreID==dNum) xil_printf("Core %d Program FINISHED\n\r",dNum);
}

unsigned int getKernelArg(unsigned int argNum) {
    if (_coreID==dNum) xil_printf("Core %d get kernel arg:%d\n\r",dNum,argNum);
    return ReadMem(GSMem.Config.MemBaseAddress,OFFKERNELARG+argNum);
}
unsigned int getGroupID(unsigned int dim) {
    return _groupID[dim];
}
unsigned int getLocalID(unsigned int dim) {
	return _localID[dim];
}

unsigned int getGlobalID(
        unsigned int dim,
        unsigned int groupID0,
        unsigned int groupID1,
        unsigned int localID0,
        unsigned int localID1,
        unsigned int localSize0,
        unsigned int localSize1
        ) {
    return (dim==0) ?
            localSize0*groupID0+localID0:
            localSize1*groupID1+localID1;
}

unsigned int isCurrKernelFinish() {
    unsigned int status;
    if (isLocalHost()) {
		int id0 = 0;
		int id1 = 0;
		//get groupid
        getfslx(id0,4,FSL_DEFAULT);
        getfslx(id1,5,FSL_DEFAULT);		
		if (id0 == -1) {
            WriteMem(LSMem.Config.MemBaseAddress,OFFKERNELFINISH,KERNELFINISH);
            if (_coreID==dNum) xil_printf("Core %d kernel finish\n\r",dNum);
            barrier(1);
            return 1;
        } else {
			_groupID[0] = id0;
			_groupID[1] = id1;
			putfslx(_groupNum,3,FSL_DEFAULT);
            if (_coreID==dNum) xil_printf("Core %d new group start,GroupID0=%d, GroupID1=%d\n\r",dNum,_groupID[0],_groupID[1]);
            WriteMem(LSMem.Config.MemBaseAddress,OFFGROUPID+0,_groupID[0]);
            WriteMem(LSMem.Config.MemBaseAddress,OFFGROUPID+1,_groupID[1]);
            //setup localIDGenerator
            if (_coreID==dNum) {
            	if (XLocalidgenerator_IsDone(&HlsLocalidgenerator))
            		xil_printf("Core %d curr localid done\n\r",dNum);
            	else
            		xil_printf("Core %d curr localid NOT done\n\r",dNum);
            }
            XLocalidgenerator_Set_localSize0(&HlsLocalidgenerator,_localSize[0]);
            XLocalidgenerator_Set_localSize1(&HlsLocalidgenerator,_localSize[1]);
            XLocalidgenerator_Set_numCore(&HlsLocalidgenerator,NUMOFLOCALCORE);
            XLocalidgenerator_Start(&HlsLocalidgenerator);
            barrier(1);
            //send localIDGenerator a seed
            putfslx(_coreID,2,FSL_DEFAULT);
			if (_coreID==dNum) xil_printf("Core %d Put localIDGenerator Seeds\n\r",dNum);
            return 0;
        }
    } else {
        barrier(1);
        status = ReadMem(LSMem.Config.MemBaseAddress,OFFKERNELFINISH);
        if (status == KERNELFINISH) {
            if (_coreID==dNum) xil_printf("Core %d kernel finish\n\r",dNum);
            return 1;
        } else {
        	//send localIDGenerator a seed
        	putfslx(_coreID,2,FSL_DEFAULT);
            _groupID[0] = ReadMem(LSMem.Config.MemBaseAddress,OFFGROUPID+0);
            _groupID[1] = ReadMem(LSMem.Config.MemBaseAddress,OFFGROUPID+1);
            if (_coreID==dNum) xil_printf("Core %d new group start,groupID0=%d,groupID1=%d\n\r",dNum,_groupID[0],_groupID[1]);
            return 0;
        }
    }
}
unsigned int isCurrGroupFinish() {
	if (_coreID==dNum) {
		xil_printf("Core %d Entering isCurrGroupFinish\n\r",dNum);
	}
	_performance[_coreID]++;
	int id0 = 0;
	int id1 = 0;
	getfslx(id0,2,FSL_DEFAULT);
	getfslx(id1,3,FSL_DEFAULT);
	if (_coreID==dNum) xil_printf("Core %d curr num of localID %d  %d\n\r",dNum,id0,id1);
	if (id0 == -1)
		return 1;
	else {
		_localID[0] = id0;
		_localID[1] = id1;
		putfslx(_coreID,2,FSL_DEFAULT);
		return 0;
	}
//   return (_currLocalID[0]>=_itemPerSlave)?1:0;
}
unsigned int simpleDMA(unsigned int id,unsigned int src,unsigned int dst,unsigned int len) {
    unsigned int DMAStatus;
    int status=XST_SUCCESS;
    DMAStatus=ReadMem(LSMem.Config.MemBaseAddress,OFFDMA+id);
    if (DMAStatus!=DMADONE) { 
        if (isLocalHost()) {
            status = XAxiCdma_SimpleTransfer(&CDMA,(u32)src,(u32)dst,len*4,NULL,NULL);
            if (status != XST_SUCCESS) 
                return 1;
            while (XAxiCdma_IsBusy(&CDMA)); //wait for transfer is done
            WriteMem(LSMem.Config.MemBaseAddress,OFFDMA+id,DMADONE);
            barrier(3);
            return 0;
        } else {
            barrier(3);
            return 0;
        }
    } else
        return 0;
}

#endif

#ifdef _HOST_
void setKernelArg(unsigned int argNum, unsigned int val) {
    WriteMem(GSMem.Config.MemBaseAddress,OFFKERNELARG+argNum,val);
}
void setLocalSize(unsigned int dim0, unsigned int dim1) {
    WriteMem(GSMem.Config.MemBaseAddress,OFFLOCALSIZE+0,dim0);
    WriteMem(GSMem.Config.MemBaseAddress,OFFLOCALSIZE+1,dim1);
}
void setGroupSize(unsigned int dim0, unsigned int dim1) {
	XGroupidgenerator_Set_groupSize0(&HlsGroupidgenerator,dim0);
	XGroupidgenerator_Set_groupSize1(&HlsGroupidgenerator,dim1);
	XGroupidgenerator_Set_numGroup(&HlsGroupidgenerator,NUMOFGROUP);
	XGroupidgenerator_Start(&HlsGroupidgenerator);
}
void kernelStart() {
    int i;
    //groupNumAllocator config
    XGroupnumberallocator_Set_numGroup(&HlsGroupnumallocator,NUMOFGROUP);
    XGroupnumberallocator_Start(&HlsGroupnumallocator);
    //localCoreIDAllocator config
    for (i=0;i<NUMOFGROUP;i++) {
        //localCoreIDAllocator config
    	XLocalcoreidallocator_Set_numCore(&HlsLocalcoreidallocator[i],NUMOFLOCALCORE);
    	XLocalcoreidallocator_Start(&HlsLocalcoreidallocator[i]);
        WriteMem(GSMem.Config.MemBaseAddress,OFFGROUPSTART+i,GROUPSTART);
    }
}
void kernelFinish() {
    int i;
    int isFinish=0;
    unsigned int status;
    while (isFinish == 0) 
        for (i=0;i<NUMOFGROUP;i++) {
            status = ReadMem(GSMem.Config.MemBaseAddress,OFFGROUPSTART+i);
            if (status == GROUPFINISH)
                isFinish=1;
            else {
                isFinish=0;
                i=NUMOFGROUP;
            }
        }
}
#endif
