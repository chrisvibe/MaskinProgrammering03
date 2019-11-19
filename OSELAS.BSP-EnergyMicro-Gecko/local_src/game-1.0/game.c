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

//Checks whether the maxscore of P1 or P2 exceeds maxScore (a variable manually set).
bool isGameFinished(P1Score, P2Score){
	if (P1Score >= maxScore || P2Score >= maxScore){
		return 1;
	};
	return 0;
};

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
	uint16_t center_pad;
	int16_t dy;
} pad1, pad2; 

/* A function that is to give initial dx and dy to the ball. The x, y and speed can be constant */
void initialBall(struct ball *ballen){
	ballen->x = 160;
	ballen->y = 120;
	ballen->speed = 1; /* Setter her speed til 1, men dette kan endres */
	ballen->dx = (rand()/RAND_MAX); /* Setter dx og dy til random nummer/rand max slik at vi har en verdi mellom 0 og 1 */
	ballen->dy = (rand()/RAND_MAX); /* Betyr at ballen kommer til å gå mot høyre hver gang */
};

/* Checks for where collision occured */
int whereCollision(struct ball *ballen, struct pad *pad1, struct pad *pad2){
	/* Implementation for padcrash 
	1. Will have an array of pixels that we are operating with
	2. If the ball has the same x value as the pad, and if the y-value of ball is equal to some y-value of pad
	3. Then check for how far the y-value is from the center of the y-value of the pad
	4. Give the ball a new angle based on how many pixels away from y and disregard the incoming angle. */
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

/* dx or dy is multiplied by -1 depending on what type of crash it is (horizontal vs vertical) */
void ballMovement(struct ball *ballen, struct pad *padden){
	/* This method needs to be changed later as the ball will not change direction based on pad angle by using the current function */
	if (whereCollision == 0){
		if (ballen->dy > (padden->center_pad + 5)){
			ballen->dy *= 1.8;
			ballen->dx *= -1;	
		}
		else if (ballen->dy > (padden->center_pad + 3)){
			ballen->dy *= 1.6;
			ballen->dx *= -1;	
		}
		else if (ballen->dy > (padden->center_pad + 1)){
			ballen->dy *= 1.4;	
			ballen->dx *= -1;
		}
		else if (ballen->dy == padden->center_pad){
			ballen->dx *= -1;	
		}
		else if (ballen->dy < (padden->center_pad - 5)){
			ballen->dy *= -1.8;
			ballen->dx *= -1;	
		}
		else if (ballen->dy < (padden->center_pad - 3)){
			ballen->dy *= -1.6;
			ballen->dx *= -1;	
		}
		else if (ballen->dy < (padden->center_pad - 1)){
			ballen->dy *= -1.4;
			ballen->dx *= -1;	
		}
			
	};
	if (whereCollision == 1){
		ballen->dx *= -1;
	};
	if (whereCollision == 2){
		ballen->dy *= -1;
	};
	
	ballen->x += ballen->dx*ballen->speed;
	ballen->y += ballen->dy*ballen->speed;
};

/* Function for converting input from driver to commands to pads */
int movePads(){
	/* Here is what the implementation might look like: 
	Using mmap to map the file input to the memory output. 
	Will potentially have to files that we are reading from. This game.c file will then only work as 
	a connector between driver and graphic. 
	*/

};

/* Function for reading from driver */ 
void checkDriver(){
	/* This function will check whether the driver is functioning and shut down game if not */
};

void checkLCD(){ 
	/* This funciton will check whether the LCD screen is functioning and shut down game if not */
}


/* THINK THIS MIGHT BE REDUNDANT
 Check whether a collision has occured 
bool checkCollision(struct ball *ballen, struct pad *pad1, struct pad *pad2){
	if (ballen->x == pad1->x && ballen->y == pad1->y || ballen->y == pad2->x && ballen->y == pad2->y || ballen->x == 0 || ballen->x == 320 || ballen->y == 0 || ballen->y == 240){
		return 1; 
	};
};
*/





int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");

	exit(EXIT_SUCCESS);
}



