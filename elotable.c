#include "elotable.h"

int eloTable[] = {0, 4, 11, 18, 26, 33, 40, 47, 54, 62, 69, 77, 84, 92, 99, 107, 114, 122, 130, 138, 146, 154, 163, 171, 180, 189, 198, 207, 216, 226, 236, 246, 257, 268, 279, 291, 303, 316, 329, 345, 358, 375, 392, 412, 433, 457, 485, 518, 560, 620, 736};

static int readTable(int diff){
	for(int i = 0 ; i < 50 ; i ++){
		if(eloTable[i + 1] > diff)
			return(50 + i);
	}
	return 100;
}

int getLosingProbability(int diff){
	if(diff >= 0)
		return (100 - readTable(diff));
	else
		return readTable(- diff);
}

