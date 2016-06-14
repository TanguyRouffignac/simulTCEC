#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "elotable.h"

#define BLACKLOSS 48
#define WHITELOSS 49
#define BLACKDRAW 50
#define WHITEDRAW 51
#define BLACKWIN  52
#define WHITEWIN  53
#define BLACKPLAY 54
#define WHITEPLAY 55
#define SELF      56
#define NBITER    10000000
#define NBPLAYER  16
#define NBQUALIF  8
#define NBGAMES   2

struct Engine {
	char name[20];
	int nbCrash;
	int SB;
	int Elo;
	int results[NBPLAYER * NBGAMES];
	int score;
	int nbQualif;
	int id;
};

struct Engine input[NBPLAYER];
struct Engine participants[NBPLAYER];
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
	for (int j = 0 ; j < NBPLAYER ; j ++){
		for(int k = 0 ; k < NBGAMES ; k ++){
			switch (participants[i].results[(NBGAMES * participants[j].id) + k]) {
				case BLACKLOSS:
				case WHITELOSS:
					printf("0");
					break;
				case BLACKDRAW:
				case WHITEDRAW:
					printf("=");
					break;
				case BLACKWIN:
				case WHITEWIN:
					printf("1");
					break;
				case BLACKPLAY:
				case WHITEPLAY:
					printf(".");
					break;
				case SELF:
					printf("X");
					break;
			}
		}
		printf(" ");
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
	for(int i = 0 ; i < (NBPLAYER * NBGAMES) ; i ++){
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
	for(int i = 0 ; i < (NBPLAYER * NBGAMES) ; i ++){
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
	for(int i = 0 ; i < (NBPLAYER * NBGAMES) ; i ++){
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

	acount = 0;
	bcount = 0;
	for(int i = 0 ; i < NBGAMES ; i ++){
		if ((participants[a].results[(b * NBGAMES) + i] == BLACKWIN) || (participants[a].results[(b * NBGAMES) + i] == WHITEWIN))
			acount ++;
		if ((participants[a].results[(b * NBGAMES) + i] == BLACKLOSS) || (participants[a].results[(b * NBGAMES) + i] == WHITELOSS))
			bcount ++;
	}

	if (acount > bcount)
		return a;
	if (acount < bcount)
		return b;

	// EGALITE ==> RANDOM

	if (rand() % 2)
		return a;
	return b;
}

static void sort(int init){
	int limit;
	if (init)
		limit = NBPLAYER;
	else
		limit = NBQUALIF;
	for(int i = 0 ; i < limit ; i ++){
		int max = i;
		for(int j = i+1 ; j < NBPLAYER ; j ++){
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
	for (int i = 0 ; i < NBPLAYER ; i ++){
		participants[i].score = 0;
		for (int j = 0 ; j < (NBPLAYER * NBGAMES) ; j ++){
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
	for (int i = 0 ; i < NBPLAYER ; i ++){
		participants[i].SB = 0;
		for (int j = 0 ; j < (NBPLAYER * NBGAMES) ; j ++){
			switch (participants[i].results[j]) {
				case BLACKDRAW:
				case WHITEDRAW:
					participants[i].SB += participants[j / NBGAMES].score;
					break;
				case BLACKWIN:
				case WHITEWIN:
					participants[i].SB += 2 * participants[j / NBGAMES].score;
					break;
				default:
					break;
			}
		}
	}
	sort(init);
}

static void simulate(){
	for(int i = 0 ; i < NBPLAYER ; i ++)
		participants[i] = input[i];
	for(int i = 0 ; i < (NBPLAYER - 1) ; i ++){
		for(int j = i + 1 ; j < NBPLAYER ; j ++){
			for(int k = 0 ; k < NBGAMES ; k ++){
				if(participants[i].results[(NBGAMES * j) + k] == BLACKPLAY){
					int r = rand() % 200;
					if(r < getLosingProbability(participants[i].Elo - participants[j].Elo)){
						participants[i].results[(NBGAMES * j) + k] = BLACKLOSS;
						participants[j].results[(NBGAMES * i) + k] = WHITEWIN;
					} else {
						if(r < 3 * getLosingProbability(participants[i].Elo - participants[j].Elo)){
							participants[i].results[(NBGAMES * j) + k] = BLACKDRAW;
							participants[j].results[(NBGAMES * i) + k] = WHITEDRAW;
						} else {
							participants[i].results[(NBGAMES * j) + k] = BLACKWIN;
							participants[j].results[(NBGAMES * i) + k] = WHITELOSS;
						}
					}
				}
				if(participants[i].results[(NBGAMES * j) + k] == WHITEPLAY){
					int r = rand() % 200;
					if(r < getLosingProbability(participants[i].Elo - participants[j].Elo)){
						participants[i].results[(NBGAMES * j) + k] = WHITELOSS;
						participants[j].results[(NBGAMES * i) + k] = BLACKWIN;
					} else {
						if(r < 3 * getLosingProbability(participants[i].Elo - participants[j].Elo)){
							participants[i].results[(NBGAMES * j) + k] = WHITEDRAW;
							participants[j].results[(NBGAMES * i) + k] = BLACKDRAW;
						} else {
							participants[i].results[(NBGAMES * j) + k] = WHITEWIN;
							participants[j].results[(NBGAMES * i) + k] = BLACKLOSS;
						}
					}
				}
			}
		}
	}
	getScores(0);
}

int main(int argc, char * argv []){
	srand((unsigned int)time(NULL));
	char c;
	for(int i = 0 ; i < NBPLAYER ; i ++) {
		scanf("%s %d %d ", input[i].name, &(input[i].nbCrash), &(input[i].Elo));
		for(int j = 0 ; j < NBPLAYER ; j ++){
			for(int k = 0 ; k < NBGAMES ; k ++){
				scanf("%c", &c);
				input[i].results[(j * NBGAMES) + k] = (int) c;
			}
			scanf(" ");
		}
		input[i].id = i;
		if(strlen(input[i].name) > maxNameLength)
			maxNameLength = strlen(input[i].name);

	}
	for(int i = 0 ; i < NBPLAYER ; i ++)
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
			printf("\r");
			for(int k = 0 ; k < 80 ; k ++)
				printf(" ");
			printf("\rSimulation currently at %d%%", percentage);
			fflush(stdout);
		}
	}
	printf("\n\n  **********************\n");
	printf("  * Simulation results *\n");
	printf("  **********************\n\n");
	for(int i = 0 ; i < NBPLAYER ; i ++){
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
