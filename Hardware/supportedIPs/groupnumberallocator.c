void groupNumberAllocator(
		int *numGroup,
		unsigned int *group0,
		unsigned int *group1,
		unsigned int *group2,
		unsigned int *group3,
		unsigned int *group4,
		unsigned int *group5
		) {
#pragma HLS INTERFACE s_axilite port=return						bundle=BUS_CMD
#pragma HLS INTERFACE s_axilite register port=numGroup			bundle=BUS_CMD
#pragma HLS INTERFACE axis register port=group0
#pragma HLS INTERFACE axis register port=group1
#pragma HLS INTERFACE axis register port=group2
#pragma HLS INTERFACE axis register port=group3
#pragma HLS INTERFACE axis register port=group4
#pragma HLS INTERFACE axis register port=group5
	int num = 0;
	num = *numGroup;
	if (num == 1) {
		*group0 = 0;
	}
	if (num == 2) {
		*group0 = 0;
		*group1 = 1;
	}
	if (num == 3) {
		*group0 = 0;
		*group1 = 1;
		*group2 = 2;
	}
	if (num == 4) {
		*group0 = 0;
		*group1 = 1;
		*group2 = 2;
		*group3 = 3;
	}
	if (num == 5) {
		*group0 = 0;
		*group1 = 1;
		*group2 = 2;
		*group3 = 3;
		*group4 = 4;
	}
	if (num == 6) {
		*group0 = 0;
		*group1 = 1;
		*group2 = 2;
		*group3 = 3;
		*group4 = 4;
		*group5 = 5;
	}
}
