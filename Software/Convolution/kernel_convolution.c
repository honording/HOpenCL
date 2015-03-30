#define _SLAVE_
//#define DMA
#include "../HCL/hcl.h"

#define FILTER_LEN	7

int main () {
    initCore();
	int filter[FILTER_LEN] = {1,0,2,0,3,4,1};
    int * arrayA          = (int *)getKernelArg(0);
    int * arrayB          = (int *)getKernelArg(1);

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
            int sum = 0;
			int j;
//			if (_coreID==0) xil_printf("%d\r\n",globalIDy);
			for (j=0;j<FILTER_LEN;j++) {
				int index = globalIDy+j;
				sum += arrayA[index]*filter[j];
			}
			arrayB[globalIDy] = sum;
        }
    }
    cleanupSlave();
    return 0;
}
