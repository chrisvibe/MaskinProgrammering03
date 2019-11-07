#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define P1 1
#define P2 2

#define HEIGHT 240; 
#define WIDTH 320;

int P1Score = 0; 
int P2Score = 0;
int maxScore = 5;


/* Lager en struct for en ball som per nå er 1 pixel stor. Struct skal alltid inneholde nyeste oppdatering av ballen. */
struct ball {
	uint16_t x;
	uint16_t y;
	int16_t speed; /* Could be constant as we have no plans of changing speed as of yet */
	int16_t dx; /* Can not be unsigned as we might get som negative values */
	int16_t dy;
} ballen;

/* Lager en struct for en pad, merk at de per nå kun har en pixel som verdi */
struct pad { 
	uint16_t x; /* x-lokasjon til padden kan være statisk */ 
	uint16_t y; 
	int16_t dy;
} pad1, pad2; 

/* A function that is to give initial dx and dy to the ball. The x, y and speed can be constant */
void initialBall(struct ball *ballen){
	

}



//Checks whether the maxscore of P1 or P2 exceeds maxScore (a variable manually set).
bool isGameFinished(P1Score, P2Score){
	if (P1Score >= maxScore || P2Score >= maxScore){
		return 1;
	};
	return 0;
};

/* Check whether a collision has occured */
bool checkCollision(struct ball *ballen, struct pad *pad1, struct pad *pad2){
	if (ballen->x == pad1->x && ballen->y == pad1->y || ballen->y == pad2->x && ballen->y == pad2->y || ballen->x == 0 || ballen->x == 320 || ballen->y == 0 || ballen->y == 240){
		return 1; 
	};
};

/* Checks for where collision occured */
int whereCollision(struct ball *ballen, struct pad *pad1, struct pad *pad2){
	/* padcrash */
	if (ballen->x == pad1->x && ballen->y == pad1->y || ballen->y == pad2->x && ballen->y == pad2->y){
		return 0; 
	}; 
	
	/* wallcrash in vertical direction */ 
	if (ballen->x == 0 || ballen->x == 320) {
		return 1;
	};

	/* wallcrash in horizontal direction */ 
	if (ballen->y == 0 || ballen->y == 240) {
		return 2;
	};

	return -1; 
};

/* Helper method for handling a wall crash */
struct ball handleCrash(struct ball *ballen){

}

/* dx or dy is multiplied by -1 depending on what type of crash it is (horizontal vs vertical) */
void ballMovement(struct ball *ballen){
	if (whereCollision == 1){
		ballen->dx *= -1;
	};
	if (whereCollision == 2){
		ballen->dy *= -1;
	};
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");

	exit(EXIT_SUCCESS);
}
