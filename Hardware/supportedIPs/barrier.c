 void barrier (
		int *numCore,
		unsigned int *icore0,
		unsigned int *icore1,
		unsigned int *icore2,
		unsigned int *icore3,
		unsigned int *icore4,
		unsigned int *icore5,
		unsigned int *icore6,
		unsigned int *ocore0,
		unsigned int *ocore1,
		unsigned int *ocore2,
		unsigned int *ocore3,
		unsigned int *ocore4,
		unsigned int *ocore5,
		unsigned int *ocore6
		) {

#pragma HLS INTERFACE s_axilite port=return				bundle=BUS_CMD
#pragma HLS INTERFACE s_axilite register port=numCore	bundle=BUS_CMD
#pragma HLS INTERFACE axis register port=icore0
#pragma HLS INTERFACE axis register port=icore1
#pragma HLS INTERFACE axis register port=icore2
#pragma HLS INTERFACE axis register port=icore3
#pragma HLS INTERFACE axis register port=icore4
#pragma HLS INTERFACE axis register port=icore5
#pragma HLS INTERFACE axis register port=icore6
#pragma HLS INTERFACE axis register port=ocore0
#pragma HLS INTERFACE axis register port=ocore1
#pragma HLS INTERFACE axis register port=ocore2
#pragma HLS INTERFACE axis register port=ocore3
#pragma HLS INTERFACE axis register port=ocore4
#pragma HLS INTERFACE axis register port=ocore5
#pragma HLS INTERFACE axis register port=ocore6

	unsigned int in0 = 0;
	unsigned int in1 = 0;
	unsigned int in2 = 0;
	unsigned int in3 = 0;
	unsigned int in4 = 0;
	unsigned int in5 = 0;
	unsigned int in6 = 0;
	unsigned int trigger = 0;
	unsigned int num;
	num = *numCore;
	if (num == 1) {
		in0 = *icore0;
		if ((in0==1)) {
			*ocore0 = in0;
		}
	}
	if (num == 2) {
		in0 = *icore0;
		in1 = *icore1;
		if ((in0==1) && (in1==1)) {
			*ocore0 = in0;
			*ocore1 = in1;
		}
	}
	if (num == 3) {
		in0 = *icore0;
		in1 = *icore1;
		in2 = *icore2;
		if ((in0==1) && (in1==1) && (in2==1)) {
			*ocore0 = in0;
			*ocore1 = in1;
			*ocore2 = in2;
		}
	}
	if (num == 4) {
		in0 = *icore0;
		in1 = *icore1;
		in2 = *icore2;
		in3 = *icore3;
		if ((in0==1) && (in1==1) && (in2==1) && (in3==1)) {
			*ocore0 = in0;
			*ocore1 = in1;
			*ocore2 = in2;
			*ocore3 = in3;
		}
	}
	if (num == 5) {
		in0 = *icore0;
		in1 = *icore1;
		in2 = *icore2;
		in3 = *icore3;
		in4 = *icore4;
		if ((in0==1) && (in1==1) && (in2==1) && (in3==1) && (in4==1)) {
			*ocore0 = in0;
			*ocore1 = in1;
			*ocore2 = in2;
			*ocore3 = in3;
			*ocore4 = in4;
		}
	}
	if (num == 6) {
		in0 = *icore0;
		in1 = *icore1;
		in2 = *icore2;
		in3 = *icore3;
		in4 = *icore4;
		in5 = *icore5;
		if ((in0==1) && (in1==1) && (in2==1) && (in3==1) && (in4==1) && (in5==1)) {
			*ocore0 = in0;
			*ocore1 = in1;
			*ocore2 = in2;
			*ocore3 = in3;
			*ocore4 = in4;
			*ocore5 = in5;
		}
	}
	if (num == 7) {
		in0 = *icore0;
		in1 = *icore1;
		in2 = *icore2;
		in3 = *icore3;
		in4 = *icore4;
		in5 = *icore5;
		in6 = *icore6;
		if ((in0==1) && (in1==1) && (in2==1) && (in3==1) && (in4==1) && (in5==1) && (in6==1)) {
			*ocore0 = in0;
			*ocore1 = in1;
			*ocore2 = in2;
			*ocore3 = in3;
			*ocore4 = in4;
			*ocore5 = in5;
			*ocore6 = in6;
		}
	}
}
