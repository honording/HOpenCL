#define _SLAVE_
//#define DMA
#include "../HCL/hcl.h"
int main () {
    initCore();
    int * arrayA          = (int *)getKernelArg(0);
    int * arrayB          = (int *)getKernelArg(1);
    int * arrayC          = (int *)getKernelArg(2);
    unsigned int ASizex     = getKernelArg(3);
    unsigned int ASizey     = getKernelArg(4);
    unsigned int BSizex     = getKernelArg(5);
    unsigned int BSizey     = getKernelArg(6);
    //finish arguments
    unsigned int localSizex = getLocalSize(0);
    unsigned int localSizey = getLocalSize(1);

    while (isCurrKernelFinish()==0) {
        unsigned int groupIDx = getGroupID(0);
        unsigned int groupIDy = getGroupID(1);
        while (isCurrGroupFinish()==0) {
            unsigned int localIDx = getLocalID(0);
            unsigned int localIDy = getLocalID(1);
            unsigned int globalIDx = getGlobalID(0,groupIDx,groupIDy,localIDx,localIDy,localSizex,localSizey);
            unsigned int globalIDy = getGlobalID(1,groupIDx,groupIDy,localIDx,localIDy,localSizex,localSizey);
            int i;
            int sum = 0;
#ifdef DMA
            simpleDMA(0,(unsigned int)((unsigned int *)arrayA+globalIDx*ASizey),(unsigned int)((unsigned int *)LocalMem.Config.MemBaseAddress),localSizey*ASizey);
            int * localArrayA = (int *)LocalMem.Config.MemBaseAddress;
            for (i=0;i<ASizey;i++)
                sum += localArrayA[localIDx*ASizey+i] * arrayB[i*BSizey+globalIDy];
#else
            for (i=0;i<ASizey;i++) 
                sum += arrayA[globalIDx*ASizey+i] * arrayB[i*BSizey+globalIDy];
#endif
            arrayC[BSizey*globalIDx+globalIDy]=sum;
        }
    }
    cleanupSlave();
    return 0;
}
