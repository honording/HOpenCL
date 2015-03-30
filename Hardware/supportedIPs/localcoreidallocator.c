void localCoreIDAllocator(
		int *numCore,
		unsigned int *core0,
		unsigned int *core1,
		unsigned int *core2,
		unsigned int *core3,
		unsigned int *core4,
		unsigned int *core5,
		unsigned int *core6
		) {
#pragma HLS INTERFACE s_axilite port=return 			bundle=BUS_CMD
#pragma HLS INTERFACE s_axilite register port=numCore	bundle=BUS_CMD
#pragma HLS INTERFACE axis register port=core0
#pragma HLS INTERFACE axis register port=core1
#pragma HLS INTERFACE axis register port=core2
#pragma HLS INTERFACE axis register port=core3
#pragma HLS INTERFACE axis register port=core4
#pragma HLS INTERFACE axis register port=core5
#pragma HLS INTERFACE axis register port=core6
	int num = 0;
	num = *numCore;
	if (num == 1) {
		*core0 = 0;
	}
	if (num == 2) {
		*core0 = 0;
		*core1 = 1;
	}
	if (num == 3) {
		*core0 = 0;
		*core1 = 1;
		*core2 = 2;
	}
	if (num == 4) {
		*core0 = 0;
		*core1 = 1;
		*core2 = 2;
		*core3 = 3;
	}
	if (num == 5) {
		*core0 = 0;
		*core1 = 1;
		*core2 = 2;
		*core3 = 3;
		*core4 = 4;
	}
	if (num == 6) {
		*core0 = 0;
		*core1 = 1;
		*core2 = 2;
		*core3 = 3;
		*core4 = 4;
		*core5 = 5;
	}
	if (num == 7) {
		*core0 = 0;
		*core1 = 1;
		*core2 = 2;
		*core3 = 3;
		*core4 = 4;
		*core5 = 5;
		*core6 = 6;
	}
}
