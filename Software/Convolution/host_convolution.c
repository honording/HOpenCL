#define _HOST_
#include "../HCL/hcl.h"
#include <stdio.h>
#include "malloc.h"
#define HCL

#define FILTER_LEN	7
int main () {
//	xil_printf("ddd\r\n");
    initCore();
    int *arrayA;
    int *arrayB;
	int filter[FILTER_LEN] = {1,0,2,0,3,4,1};

	
	
	int len = 256;
	int margin = FILTER_LEN/2;
	
    arrayA = (int *)malloc((len+margin*2)*sizeof(int));
    arrayB = (int *)malloc(len*sizeof(int));

    int i;

    for (i=0;i<len+margin*2;i++) {
    	arrayA[i] = i;
    }
	for (i=0;i<len;i++)
		arrayB[i] = 0;
#ifndef HCL
	for (i=0;i<len;i++) {
		int j;
		int sum = 0;
		for (j=0;j<FILTER_LEN;j++) {
			int index = i+j;
			sum += arrayA[index]*filter[j];
		}
		arrayB[i] = sum;
	}
#endif
	
#ifdef HCL
    //set group size
    //need special functions to get group and local size
    unsigned int ls[2] = {1,16};
    setLocalSize(ls[0],ls[1]);
    setGroupSize(1,len/ls[1]);
    setKernelArg(0,(unsigned int)arrayA);
    setKernelArg(1,(unsigned int)arrayB);
//    setKernelArg(2,(unsigned int)len);

    u32 t=0;
    XTmrCtr_Reset(&timer, 0);
    kernelStart();
    kernelFinish();
    t = XTmrCtr_GetValue(&timer, 0);

    xil_printf("Input Size:%d  Kernel Time=%d\n\r",len,t);

//    for (i=0;i<TOTALLOCALCORE;i++) {
//    	xil_printf("Core:%d,%d\r\n",i,ReadMem(GSMem.Config.MemBaseAddress,OFFPERFORMANCE+i));
//    }
#endif

//    for (i=0;i<len;i++) {
//		xil_printf("%d ",arrayB[i]);
//    }
//	xil_printf("\r\n");
    return 0;
}
