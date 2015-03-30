#define NUMGROUPS	6

int incX(
		int initX,
		int initY,
		int numX,
		int numY
		) {
	int y=initY+1;
	if (y==numY)
		return initX+1;
	else
		return initX;
}
int incY(
		int initX,
		int initY,
		int numX,
		int numY
		) {
	int y=initY+1;
	if (y==numY)
		return 0;
	else
		return y;
}

void groupIDGenerator(
		int *groupSize0,	//number of X
		int *groupSize1,	//number of Y
		unsigned int *numGroup,
		unsigned int *groupID,
		int *id0x,
		int *id0y,
		int *id1x,
		int *id1y,
		int *id2x,
		int *id2y,
		int *id3x,
		int *id3y,
		int *id4x,
		int *id4y,
		int *id5x,
		int *id5y
		) {
#pragma HLS INTERFACE s_axilite port=return			 			bundle=BUS_CMD
#pragma HLS INTERFACE s_axilite register port=groupSize0	 	bundle=BUS_CMD
#pragma HLS INTERFACE s_axilite register port=groupSize1	 	bundle=BUS_CMD
#pragma HLS INTERFACE s_axilite register port=numGroup			bundle=BUS_CMD
#pragma HLS INTERFACE axis register port=groupID
#pragma HLS INTERFACE axis register port=id0x
#pragma HLS INTERFACE axis register port=id0y
#pragma HLS INTERFACE axis register port=id1x
#pragma HLS INTERFACE axis register port=id1y
#pragma HLS INTERFACE axis register port=id2x
#pragma HLS INTERFACE axis register port=id2y
#pragma HLS INTERFACE axis register port=id3x
#pragma HLS INTERFACE axis register port=id3y
#pragma HLS INTERFACE axis register port=id4x
#pragma HLS INTERFACE axis register port=id4y
#pragma HLS INTERFACE axis register port=id5x
#pragma HLS INTERFACE axis register port=id5y


	int numOfX = *groupSize0; //3
	int numOfY = *groupSize1; //5
	int total = numOfX * numOfY;
	unsigned int currNumGroup = *numGroup;
	unsigned int finish = 0;
	unsigned int cnt 	= 0;
	unsigned int signal[NUMGROUPS] = {0};
	int idx = 0;
	int idy = 0;
	int i = 0;
/*
	if (currNumGroup == 1) { //1 CGU
		while (finish != 1) {
			unsigned int currGroupID = *groupID;
			if (currGroupID == 0) {
				if (cnt >= total) {
					*id0x = -1;
					*id0y = -1;
					signal[0] = 1;
				} else {
					*id0x = idx;
					*id0y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (signal[0]==1)
				finish = 1;
			else
				finish = 0;
		}
	}
	if (currNumGroup == 2) { //2 GCUs
		while (finish != 1) {
			unsigned int currGroupID = *groupID;
			if (currGroupID == 0) {
				if (cnt >= total) {
					*id0x = -1;
					*id0y = -1;
					signal[0] = 1;
				} else {
					*id0x = idx;
					*id0y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 1) {
				if (cnt >= total) {
					*id1x = -1;
					*id1y = -1;
					signal[1] = 1;
				} else {
					*id1x = idx;
					*id1y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if ((signal[0]==1)&(signal[1]==1))
				finish = 1;
			else
				finish = 0;
		}
	}
	if (currNumGroup == 3) { //3 GCUs
		while (finish != 1) {
			unsigned int currGroupID = *groupID;
			if (currGroupID == 0) {
				if (cnt >= total) {
					*id0x = -1;
					*id0y = -1;
					signal[0] = 1;
				} else {
					*id0x = idx;
					*id0y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 1) {
				if (cnt >= total) {
					*id1x = -1;
					*id1y = -1;
					signal[1] = 1;
				} else {
					*id1x = idx;
					*id1y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 2) {
				if (cnt >= total) {
					*id2x = -1;
					*id2y = -1;
					signal[2] = 1;
				} else {
					*id2x = idx;
					*id2y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1))
				finish = 1;
			else
				finish = 0;
		}
	}
	if (currNumGroup == 4) { //4 GCUs
		while (finish != 1) {
			unsigned int currGroupID = *groupID;
			if (currGroupID == 0) {
				if (cnt >= total) {
					*id0x = -1;
					*id0y = -1;
					signal[0] = 1;
				} else {
					*id0x = idx;
					*id0y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 1) {
				if (cnt >= total) {
					*id1x = -1;
					*id1y = -1;
					signal[1] = 1;
				} else {
					*id1x = idx;
					*id1y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 2) {
				if (cnt >= total) {
					*id2x = -1;
					*id2y = -1;
					signal[2] = 1;
				} else {
					*id2x = idx;
					*id2y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 3) {
				if (cnt >= total) {
					*id3x = -1;
					*id3y = -1;
					signal[3] = 1;
				} else {
					*id3x = idx;
					*id3y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1)&(signal[3]==1))
				finish = 1;
			else
				finish = 0;
		}
	}
	if (currNumGroup == 5) { //5 GCUs
		while (finish != 1) {
			unsigned int currGroupID = *groupID;
			if (currGroupID == 0) {
				if (cnt >= total) {
					*id0x = -1;
					*id0y = -1;
					signal[0] = 1;
				} else {
					*id0x = idx;
					*id0y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 1) {
				if (cnt >= total) {
					*id1x = -1;
					*id1y = -1;
					signal[1] = 1;
				} else {
					*id1x = idx;
					*id1y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 2) {
				if (cnt >= total) {
					*id2x = -1;
					*id2y = -1;
					signal[2] = 1;
				} else {
					*id2x = idx;
					*id2y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 3) {
				if (cnt >= total) {
					*id3x = -1;
					*id3y = -1;
					signal[3] = 1;
				} else {
					*id3x = idx;
					*id3y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if (currGroupID == 4) {
				if (cnt >= total) {
					*id4x = -1;
					*id4y = -1;
					signal[4] = 1;
				} else {
					*id4x = idx;
					*id4y = idy;
					idx	  = incX(idx,idy,numOfX,numOfY);
					idy   = incY(idx,idy,numOfX,numOfY);
					cnt   = cnt + 1;
				}
			}
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1)&(signal[3]==1)&(signal[4]==1))
				finish = 1;
			else
				finish = 0;
		}
	}
*/

//	if (currNumGroup == 6) { // 6 GCUs
	while (finish != 1) {
		unsigned int currGroupID = *groupID;
		if (currGroupID == 0) {
			if (cnt >= total) {
				*id0x = -1;
				*id0y = -1;
				signal[0] = 1;
			} else {
				*id0x = idx;
				*id0y = idy;
				idx	  = incX(idx,idy,numOfX,numOfY);
				idy   = incY(idx,idy,numOfX,numOfY);
				cnt   = cnt + 1;
			}
		}
		if (currGroupID == 1) {
			if (cnt >= total) {
				*id1x = -1;
				*id1y = -1;
				signal[1] = 1;
			} else {
				*id1x = idx;
				*id1y = idy;
				idx	  = incX(idx,idy,numOfX,numOfY);
				idy   = incY(idx,idy,numOfX,numOfY);
				cnt   = cnt + 1;
			}
		}
		if (currGroupID == 2) {
			if (cnt >= total) {
				*id2x = -1;
				*id2y = -1;
				signal[2] = 1;
			} else {
				*id2x = idx;
				*id2y = idy;
				idx	  = incX(idx,idy,numOfX,numOfY);
				idy   = incY(idx,idy,numOfX,numOfY);
				cnt   = cnt + 1;
			}
		}
		if (currGroupID == 3) {
			if (cnt >= total) {
				*id3x = -1;
				*id3y = -1;
				signal[3] = 1;
			} else {
				*id3x = idx;
				*id3y = idy;
				idx	  = incX(idx,idy,numOfX,numOfY);
				idy   = incY(idx,idy,numOfX,numOfY);
				cnt   = cnt + 1;
			}
		}
		if (currGroupID == 4) {
			if (cnt >= total) {
				*id4x = -1;
				*id4y = -1;
				signal[4] = 1;
			} else {
				*id4x = idx;
				*id4y = idy;
				idx	  = incX(idx,idy,numOfX,numOfY);
				idy   = incY(idx,idy,numOfX,numOfY);
				cnt   = cnt + 1;
			}
		}
		if (currGroupID == 5) {
			if (cnt >= total) {
				*id5x = -1;
				*id5y = -1;
				signal[5] = 1;
			} else {
				*id5x = idx;
				*id5y = idy;
				idx	  = incX(idx,idy,numOfX,numOfY);
				idy   = incY(idx,idy,numOfX,numOfY);
				cnt   = cnt + 1;
			}
		}
		for (i=0;i<currNumGroup;i++) {
			if (signal[i]==1)
				finish = 1;
			else {
				finish = 0;
				i = currNumGroup;
			}
		}
		/*
		if (currNumGroup == 1)
			if (signal[0]==1)
				finish = 1;
			else
				finish = 0;
		if (currNumGroup == 2)
			if ((signal[0]==1)&(signal[1]==1))
				finish = 1;
			else
				finish = 0;
		if (currNumGroup == 3)
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1))
				finish = 1;
			else
				finish = 0;
		if (currNumGroup == 4)
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1)&(signal[3]==1)&(signal[4]==1)&(signal[5]==1))
				finish = 1;
			else
				finish = 0;
		if (currNumGroup == 5)
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1)&(signal[3]==1)&(signal[4]==1)&(signal[5]==1))
				finish = 1;
			else
				finish = 0;
		if (currNumGroup == 6)
			if ((signal[0]==1)&(signal[1]==1)&(signal[2]==1)&(signal[3]==1)&(signal[4]==1)&(signal[5]==1))
				finish = 1;
			else
				finish = 0;
	*/
	}
//	}
}

