#define GSMem						0x140C0000  //0x50300000/4
#define OFFARG 					0
#define OFFLOCALSIZE			256
#define DDR
#define LSMem						0x30800000 //0xC2000000/4
#define OFFDEBUGG			388
#define OFFGROUPID			64
#define OFFKERNELFINISH	66
#define FILTER_LEN				 7
//#define LocalMem		0x31800000 //0xC6000000/4

int getGlobalID(
        int dim,
        int groupID0,
        int groupID1,
        int localID0,
        int localID1,
        int localSize0,
        int localSize1
        ) {
    return (dim==0) ?
            localSize0*groupID0+localID0:
            localSize1*groupID1+localID1;
}

void convolution(
		volatile int *data,
		volatile unsigned int *barrierIn,
		volatile unsigned int *barrierOut,
		volatile unsigned int *coreNum,
		volatile int *localID0,
		volatile int *localID1,
		volatile unsigned int *localIDout
		) {

#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE m_axi  port=data
#pragma HLS INTERFACE axis port=barrierIn
#pragma HLS INTERFACE axis port=barrierOut
#pragma HLS INTERFACE axis port=coreNum
#pragma HLS INTERFACE axis port=localID0
#pragma HLS INTERFACE axis port=localID1
#pragma HLS INTERFACE axis port=localIDout

	volatile unsigned int bo=1;
	unsigned int addrA,addrB;
    int filter[FILTER_LEN] = {1,0,2,0,3,4,1};
	unsigned int localsize0,localsize1;
	unsigned int groupid0,groupid1;
	unsigned int localid0,localid1;
	unsigned int globalid0,globalid1;
	unsigned int currCoreNum = *coreNum;
	unsigned int status = 1;
	unsigned int localfinish;
	int i,j;
	int sum=0;
	unsigned int kernelfinish;
	{
		#pragma HLS PROTOCOL fixed
		*barrierIn = 1;
	}
	{
		#pragma HLS PROTOCOL fixed
		bo=*barrierOut;
	}
	if (bo == 1) {   //barrier(0) in initCore()
		//get arguments;
		addrA = data[GSMem+OFFARG+0];
		addrA = addrA/4;
//		data[GSMem+OFFDEBUGG+0]=addrA;
		addrB = data[GSMem+OFFARG+1];
		addrB = addrB/4;
//		data[GSMem+OFFDEBUGG+1]=addrB;

		//get localsize
		localsize0 = data[GSMem+OFFLOCALSIZE+0];
//		data[GSMem+OFFDEBUGG+7]=localsize0;
		localsize1 = data[GSMem+OFFLOCALSIZE+1];
//		data[GSMem+OFFDEBUGG+8]=localsize1;
		//curr kernel finish
		while (status == 1) {
			{
				#pragma HLS PROTOCOL fixed
				*barrierIn = 1;
			}
			{
				#pragma HLS PROTOCOL fixed
				bo = *barrierOut;
			}
			if (bo == 1) {  //barrier(1) in isCurrKernelFinish()
				kernelfinish = data[LSMem+OFFKERNELFINISH];
				if (kernelfinish == 1) {
					status = 0;
				} else {		//current kernel not finish
					{
						#pragma HLS PROTOCOL fixed
						localfinish = 0;
						status = 1;
						//send localIDGenerator seeds
						*localIDout = currCoreNum;
						groupid0 = data[LSMem+OFFGROUPID+0];
						groupid1 = data[LSMem+OFFGROUPID+1];
					}
					{
						#pragma HLS PROTOCOL fixed
						while (localfinish == 0) {	//every local id do execution
							localid0 = *localID0;
							localid1 = *localID1;
							if (localid0 == -1) {
								localfinish = 1;
							} else {
								localfinish = 0;
								*localIDout = currCoreNum;
								globalid0=getGlobalID(0,groupid0,groupid1,localid0,localid1,localsize0,localsize1);
								globalid1=getGlobalID(1,groupid0,groupid1,localid0,localid1,localsize0,localsize1);
								sum = 0;
								//convert this to convultion operations
								for (j=0;j<FILTER_LEN;j++) {
									sum +=data[addrA+globalid0+j] * filter[j];
								}
							}
						 data[addrB+j+globalid1] = sum;
						}
					}
				}
			}
		}
	}
}
