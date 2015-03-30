#define _HOST_
#include "../HCL/hcl.h"
#include <stdio.h>
#include "malloc.h"
#define TIME
#define Matrix
int main () {
//	xil_printf("ddd\r\n");
    initCore();
#ifdef Matrix
    int *arrayA;
    int *arrayB;
    int *arrayC;
    
    int ASizex  = 8;
    int ASizey  = 8;
    int BSizex  = 8;
    int BSizey  = 8;

    arrayA = (int *)malloc(ASizex*ASizey*sizeof(int));
    arrayB = (int *)malloc(BSizex*BSizey*sizeof(int));
    arrayC = (int *)malloc(ASizex*BSizey*sizeof(int));
    int i;

    for (i=0;i<ASizex*ASizey;i++) {
    	arrayA[i] = 1;
    	arrayB[i] = 1;
    }
    for (i=0;i<ASizex*ASizey;i++)
    	arrayC[i] = 0;

    //set group size
    //need special functions to get group and local size
    unsigned int ls[2] = {4,4};
    setLocalSize(ls[0],ls[1]);
    setGroupSize(ASizex/ls[0],BSizey/ls[1]);
    setKernelArg(0,(unsigned int)arrayA);
    setKernelArg(1,(unsigned int)arrayB);
    setKernelArg(2,(unsigned int)arrayC);
    setKernelArg(3,(unsigned int)ASizex);
    setKernelArg(4,(unsigned int)ASizey);
    setKernelArg(5,(unsigned int)BSizex);
    setKernelArg(6,(unsigned int)BSizey);
#else

#endif
    u32 t=0;
    XTmrCtr_Reset(&timer, 0);
    kernelStart();
    kernelFinish();
    t = XTmrCtr_GetValue(&timer, 0);
#ifdef TIME
    xil_printf("Kernel Time=%d\n\r",t);
    int tt;
    for (tt=0;tt<1;tt++) {
    	xil_printf("Core:%d,%d\r\n",tt,ReadMem(GSMem.Config.MemBaseAddress,OFFPERFORMANCE+tt));
    }
#endif
    int j;
    for (i=0;i<ASizex;i++) {
    	for (j=0;j<BSizey;j++){
    		xil_printf("%d ",arrayC[BSizey*i+j]);
    	}
    	xil_printf("\r\n");
    }
    return 0;
}
