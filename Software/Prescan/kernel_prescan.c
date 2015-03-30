#define _SLAVE_
//#define DMA
#include "../HCL/hcl.h"
int main () {
    initCore();
    int * arrayA          = (int *)getKernelArg(0);
    int * arrayB          = (int *)getKernelArg(1);
    unsigned int len	  = getKernelArg(2);

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
            for (i=0;i<=globalIDy;i++) 
                sum += arrayA[i];
            arrayB[globalIDy]=sum;
        }
    }
    cleanupSlave();
    return 0;
}
