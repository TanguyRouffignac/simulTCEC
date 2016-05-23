#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "elotable.h"

#define BLACKLOSS 0
#define WHITELOSS 1
#define BLACKDRAW 2
#define WHITEDRAW 3
#define BLACKWIN  4
#define WHITEWIN  5
#define BLACKPLAY 6
#define WHITEPLAY 7
#define SELF      8
#define NBITER    10000000

struct Engine {
	char name[20];
	int nbCrash;
	int SB;
	int Elo;
	int results[16];
	int score;
	int nbQualif;
	int id;
};

struct Engine input[16];
struct Engine participants[16];
int maxNameLength = 0;

static void printEngine(int i){
	if(i < 9)
		printf(" ");
	printf("%d %s ", i + 1, participants[i].name);
	for(int j = 0 ; j < (maxNameLength - strlen(participants[i].name)) ; j++)
		printf(" ");
	printf("%d ", participants[i].Elo);
	if(participants[i].score / 2 < 10)
		printf(" ");
	printf("%d", participants[i].score / 2);
	switch(participants[i].score % 2){
		case 0:
			printf(".0 ");
			break;
		case 1:
			printf(".5 ");
			break;
	}
	if(participants[i].SB / 4 < 10)
		printf(" ");
	printf("%d", participants[i].SB / 4);
	switch(participants[i].SB % 4){
		case 0:
			printf(".00 ");
			break;
		case 1:
			printf(".25 ");
			break;
		case 2:
			printf(".50 ");
			break;
		case 3:
			printf(".75 ");
			break;
	}
	for (int j = 0 ; j < 16 ; j ++){
		switch (participants[i].results[participants[j].id]) {
			case BLACKLOSS:
			case WHITELOSS:
				printf("0 ");
				break;
			case BLACKDRAW:
			case WHITEDRAW:
				printf("= ");
				break;
			case BLACKWIN:
			case WHITEWIN:
				printf("1 ");
				break;
			case BLACKPLAY:
			case WHITEPLAY:
				printf(". ");
				break;
			case SELF:
				printf("X ");
				break;
		}
	}
	printf("\n");
}

static int isAhead(int a, int b){

	// NBRE de POINTS

	if(participants[a].score > participants[b].score)
		return a;
	if(participants[b].score > participants[a].score)
		return b;

	// 1ER TB = NBRE de CRASH

	if(participants[a].nbCrash > participants[b].nbCrash)
		return b;
	if(participants[b].nbCrash > participants[a].nbCrash)
		return a;

	// 2EME TB = SB

	if(participants[a].SB > participants[b].SB)
		return a;
	if(participants[b].SB > participants[a].SB)
		return b;

	// 3EME TB = NBRE BLACK

	int acount = 0;
	int bcount = 0;
	for(int i = 0 ; i < 16 ; i ++){
		if((participants[a].results[i] == BLACKLOSS) || (participants[a].results[i] == BLACKDRAW) || (participants[a].results[i] == BLACKWIN))
			acount ++;
		if((participants[b].results[i] == BLACKLOSS) || (participants[b].results[i] == BLACKDRAW) || (participants[b].results[i] == BLACKWIN))
			bcount ++;
	}
	if(acount > bcount)
		return a;
	if(bcount > acount)
		return b;

	// 4EME TB NBRE WINS

	acount = 0;
	bcount = 0;
	for(int i = 0 ; i < 16 ; i ++){
		if((participants[a].results[i] == WHITEWIN) || (participants[a].results[i] == BLACKWIN))
			acount ++;
		if((participants[b].results[i] == WHITEWIN) || (participants[b].results[i] == BLACKWIN))
			bcount ++;
	}
	if(acount > bcount)
		return a;
	if(bcount > acount)
		return b;

	// 5EME TB NBRE WINSBLACK

	acount = 0;
	bcount = 0;
	for(int i = 0 ; i < 16 ; i ++){
		if(participants[a].results[i] == BLACKWIN)
			acount ++;
		if(participants[b].results[i] == BLACKWIN)
			bcount ++;
	}
	if(acount > bcount)
		return a;
	if(bcount > acount)
		return b;

	// 6EME TB DIRECT CONFRONTATION

	if ((participants[b].results[a] == BLACKWIN) || (participants[a].results[b] == WHITEWIN))
		return a;
	if ((participants[b].results[a] == BLACKWIN) || (participants[a].results[b] == WHITEWIN))
		return b;

	// EGALITE ==> RANDOM

	if (rand() % 2)
		return a;
	return b;
}

static void sort(int init){
	int limit;
	if (init)
		limit = 16;
	else
		limit = 8;
	for(int i = 0 ; i < limit ; i ++){
		int max = i;
		for(int j = i+1 ; j < 16 ; j ++){
			if(isAhead(max, j) == j)
				max = j;
		}
		if(max != i){
			struct Engine tmp = participants[i];
			participants[i] = participants[max];
			participants[max] = tmp;
		}
		if(! init)
			input[participants[i].id].nbQualif ++;
	}
	if (init)
		for(int i = 0 ; i < limit ; i ++)
			printEngine(i);
}

static void getScores(int init){
	for (int i = 0 ; i < 16 ; i ++){
		participants[i].score = 0;
		for (int j = 0 ; j < 16 ; j ++){
			switch (participants[i].results[j]) {
				case BLACKDRAW:
				case WHITEDRAW:
					participants[i].score += 1;
					break;
				case BLACKWIN:
				case WHITEWIN:
					participants[i].score += 2;
					break;
				default:
					break;
			}
		}
	}
	for (int i = 0 ; i < 16 ; i ++){
		participants[i].SB = 0;
		for (int j = 0 ; j < 16 ; j ++){
			switch (participants[i].results[j]) {
				case BLACKDRAW:
				case WHITEDRAW:
					participants[i].SB += participants[j].score;
					break;
				case BLACKWIN:
				case WHITEWIN:
					participants[i].SB += 2 * participants[j].score;
					break;
				default:
					break;
			}
		}
	}
	sort(init);
}

static void simulate(){
	for(int i = 0 ; i < 16 ; i ++)
		participants[i] = input[i];
	for(int i = 0 ; i < 15 ; i ++){
		for(int j = i + 1 ; j < 16 ; j ++){
			if(participants[i].results[j] == BLACKPLAY){
				int r = rand() % 200;
				if(r < getLosingProbability(participants[i].Elo - participants[j].Elo)){
					participants[i].results[j] = BLACKLOSS;
					participants[j].results[i] = WHITEWIN;
				} else {
					if(r < 3 * getLosingProbability(participants[i].Elo - participants[j].Elo)){
						participants[i].results[j] = BLACKDRAW;
						participants[j].results[i] = WHITEDRAW;
					} else {
						participants[i].results[j] = BLACKWIN;
						participants[j].results[i] = WHITELOSS;
					}
				}
			}
			if(participants[i].results[j] == WHITEPLAY){
				int r = rand() % 200;
				if(r < getLosingProbability(participants[i].Elo - participants[j].Elo)){
					participants[i].results[j] = WHITELOSS;
					participants[j].results[i] = BLACKWIN;
				} else {
					if(r < 3 * getLosingProbability(participants[i].Elo - participants[j].Elo)){
						participants[i].results[j] = WHITEDRAW;
						participants[j].results[i] = BLACKDRAW;
					} else {
						participants[i].results[j] = WHITEWIN;
						participants[j].results[i] = BLACKLOSS;
					}
				}
			}
		}
	}
	getScores(0);
}

int main(int argc, char * argv []){
	srand((unsigned int)time(NULL));
	for(int i = 0 ; i < 16 ; i ++) {
		scanf("%s %d %d ", input[i].name, &(input[i].nbCrash), &(input[i].Elo));
		for(int j = 0 ; j < 16 ; j ++)
			scanf("%d ", &(input[i].results[j]));
		input[i].id = i;
		if(strlen(input[i].name) > maxNameLength)
			maxNameLength = strlen(input[i].name);

	}
	for(int i = 0 ; i < 16 ; i ++)
		participants[i] = input[i];
	printf("\n                     *********************\n");
	printf("                     * Current standings *\n");
	printf("                     *********************\n\n");
	getScores(1);
	printf("\nStarting a simulation with %d iterations\n", NBITER);
	printf("Simulation currently at 0%%");
	fflush(stdout);
	for(int i = 0 ; i < NBITER ; i ++){
		simulate();
		if((100 * (i + 1) % NBITER == 0)){
			int percentage = (100 * (i + 1)) / NBITER;
			if (percentage > 10)
				printf("\b");
			printf("\b\b%d%%", percentage);
			fflush(stdout);
		}
	}
	printf("\n\n  **********************\n");
	printf("  * Simulation results *\n");
	printf("  **********************\n\n");
	for(int i = 0 ; i < 16 ; i ++){
		printf("%s ", input[i].name);
		for(int j = 0 ; j < (maxNameLength - strlen(input[i].name)) ; j++)
			printf(" ");
		switch (input[i].nbQualif){
			case NBITER:
				printf("is qualified\n");
				break;
			case 0:
				printf("is eliminated\n");
				break;
			default:
				if((100 * input[i].nbQualif) % NBITER < NBITER / 2)
					printf("has %d%% chances to qualify\n", (100 * input[i].nbQualif / NBITER));
				else
					printf("has %d%% chances to qualify\n", (100 * input[i].nbQualif / NBITER) + 1);
				break;
		}
	}
	return 0;
}
