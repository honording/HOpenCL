#define _HOST_
#include "../HCL/hcl.h"
#include <stdio.h>
#include "malloc.h"
#define HCL
int main () {
//	xil_printf("ddd\r\n");
    initCore();
    int *arrayA;
    int *arrayB;
    
	int len = 12;

    arrayA = (int *)malloc(len*sizeof(int));
    arrayB = (int *)malloc(len*sizeof(int));

    int i;

    for (i=0;i<len;i++) {
    	arrayA[i] = 1;
    	arrayB[i] = 1;
    }
#ifndef HCL
	int j;
	for (i=0;i<len;i++) {
		int sum = 0;
		for (j=0;j<i;j++)
			sum += arrayA[j];
		arrayB[i] = sum;
	}		
#endif
	
#ifdef HCL
    //set group size
    //need special functions to get group and local size
    unsigned int ls[2] = {1,4};
    setLocalSize(ls[0],ls[1]);
    setGroupSize(1,len/ls[1]);
    setKernelArg(0,(unsigned int)arrayA);
    setKernelArg(1,(unsigned int)arrayB);
    setKernelArg(2,(unsigned int)len);

    u32 t=0;
    XTmrCtr_Reset(&timer, 0);
    kernelStart();
    kernelFinish();
    t = XTmrCtr_GetValue(&timer, 0);

    xil_printf("Kernel Time=%d\n\r",t);

    for (i=0;i<TOTALLOCALCORE;i++) {
    	xil_printf("Core:%d,%d\r\n",i,ReadMem(GSMem.Config.MemBaseAddress,OFFPERFORMANCE+i));
    }
#endif

    for (i=0;i<len;i++) {
		xil_printf("%d ",arrayB[i]);
    }
	xil_printf("\r\n");
    return 0;
}
