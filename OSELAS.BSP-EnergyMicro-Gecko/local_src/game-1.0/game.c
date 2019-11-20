#include "display_tools.c"
#include "display_tools.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define P1 1
#define P2 2
int P1Score = 0; 
int P2Score = 0;
int maxScore = 5;

struct Object {
	uint16_t x;
	uint16_t y;
	int16_t speed; /* Could be constant as we have no plans of changing speed as of yet */
	int16_t dx; /* Can not be unsigned as we might get som negative values */
	int16_t dy;
};

struct Game {
	struct Settings settings;
	struct Object ballen;
	struct Object pad1;
	struct Object pad2;
};

struct Game initializeGame(){
   struct Game game;
   game.settings = setup_display();
    
   draw_pad(game.settings, 130, 160);
   draw_pad(game.settings, 70, 145);
   game = initializeBall(game);
   game = initializePads(game, 100, 200);
   game = initializePads(game, 200, 150);
   refresh_display(game.settings, 0, 0, HEIGHT, WIDTH);

   return game;
}

//Checks whether the maxscore of P1 or P2 exceeds maxScore (a variable manually set).
bool isGameFinished(int P1Score, int P2Score){
	if (P1Score >= maxScore || P2Score >= maxScore){
		return 1;
	};
	return 0;
}

/* A function that is to give initial dx and dy to the ball. The x, y and speed can be constant */
struct Game initializeBall(struct Game game){
	struct Object ballen;
	ballen.x = 160;
	ballen.y = 120;
	ballen.speed = 1; /* Setter her speed til 1, men dette kan endres */
	ballen.dx = (rand()/RAND_MAX); /* Setter dx og dy til random nummer/rand max slik at vi har en verdi mellom 0 og 1 */
	ballen.dy = (rand()/RAND_MAX); /* Betyr at ballen kommer til å gå mot høyre hver gang */
    game.ballen = ballen;
    return game;
}

struct Game initializePads(struct Settings settings, int x, int y){
	struct Object pad;
	pad.x = x;
	pad.y = y;
	pad.speed = 0;
	pad.dx = 0;
	pad.dy = 0; 
    game.pad = pad;
    return game;
}

/* Checks for where collision occured */
int whereCollision(struct Object ballen, struct Object pad1, struct Object pad2){
	/* Implementation for padcrash 
	1. Will have an array of pixels that we are operating with
	2. If the ball has the same x value as the pad, and if the y-value of ball is equal to some y-value of pad
	3. Then check for how far the y-value is from the center of the y-value of the pad
	4. Give the ball a new angle based on how many pixels away from y and disregard the incoming angle. */
	if (ballen.x == pad1.x && (ballen.y > pad1.y - 15 || ballen.y < pad1.y + 15)){ 
		return 0; 
	} 
	
	else if (ballen.y == pad2.x && (ballen.y > pad2.y - 15 || ballen.y < pad2.y + 15)){
		return 1; 
	}

	/* wallcrash in vertical direction */ 
	else if (ballen.x == 0 || ballen.x == 320) {
		return 2;
	}

	/* wallcrash in horizontal direction */ 
	else if (ballen.y == 0 || ballen.y == 240) {
		return 3;
	}

	return -1; 
}


void checkPadPositions(struct Object pad1, struct Object pad2){
	if (pad1.y < 0){
		pad1.y == 0; 
	}
	if (pad1.y > 240){
		pad1.y == 240; 
	}
	if (pad2.y < 0){
		pad2.y == 0; 
	}
	if (pad2.y > 240){
		pad2.y == 240; 
	}
}

/* dx or dy is multiplied by -1 depending on what type of crash it is (horizontal vs vertical) */
void handleCollision(struct Object ballen, struct Object pad1, struct Object pad2){
	/* This method needs to be changed later as the ball will not change direction based on pad angle by using the current function */
	if (whereCollision == 0){
		if (ballen.y >= (pad1.y + 12) && (ballen.dy < (pad1.y + 15))){
			ballen.dy *= 1.8;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad1.y + 10) && (ballen.y < (pad1.y + 12))){
			ballen.dy *= 1.6;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad1.y + 5) && (ballen.y < (pad1.y + 10))){
			ballen.dy *= 1.4;	
			ballen.dx *= -1;
		}
		else if (ballen.y <= pad1.y + 5 || ballen.y > pad1.y - 5){
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad1.y - 15) && (ballen.y < (pad1.y - 12))){
			ballen.dy *= -1.8;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad1.y - 12) && (ballen.y < (pad1.y - 10))){
			ballen.dy *= -1.6;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad1.y - 10) && (ballen.y < (pad1.y - 5))){
			ballen.dy *= -1.4;
			ballen.dx *= -1;	
		}
	}
	if (whereCollision == 1){
		if (ballen.y >= (pad2.y + 12) && (ballen.dy < (pad2.y + 15))){
			ballen.dy *= 1.8;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad2.y + 10) && (ballen.y < (pad2.y + 12))){
			ballen.dy *= 1.6;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad2.y + 5) && (ballen.y < (pad2.y + 10))){
			ballen.dy *= 1.4;	
			ballen.dx *= -1;
		}
		else if (ballen.y <= pad2.y + 5 || ballen.y > pad2.y - 5){
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad2.y - 15) && (ballen.y < (pad2.y - 12))){
			ballen.dy *= -1.8;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad2.y - 12) && (ballen.y < (pad2.y - 10))){
			ballen.dy *= -1.6;
			ballen.dx *= -1;	
		}
		else if (ballen.y >= (pad2.y - 10) && (ballen.y < (pad2.y - 5))){
			ballen.dy *= -1.4;
			ballen.dx *= -1;	
		}
	}
	if (whereCollision == 2){
		if (ballen.x == 0){
			P2Score += 1;
		}
		if (ballen.x == 320){
			P1Score += 1;
		}
		//Her må det legges inn en "clean funksjon" som starter cleaner hele brettet og starter opp på nytt bare med en ny score. 
	}
	if (whereCollision == 3){
		ballen.dy *= -1;
	}
	
}

/* Function for converting input from driver to commands to pads */
int movePads(){
	/* Here is what the implementation might look like: 
	Using mmap to map the file input to the memory output. 
	Will potentially have to files that we are reading from. This game.c file will then only work as 
	a connector between driver and graphic. 
	*/

}

/* Function for reading from driver */ 
void checkDriver(){
	/* This function will check whether the driver is functioning and shut down game if not */
}

void checkLCD(){ 
	/* This funciton will check whether the LCD screen is functioning and shut down game if not */
}

void ResetScreen(){
	/* This function will clean the entire screen and start the game up again given that no player has reached the maxiumum score */
	clear_screen(settings);
}


/* THINK THIS MIGHT BE REDUNDANT
 Check whether a collision has occured 
bool checkCollision(struct ball *ballen, struct pad *pad1, struct pad *pad2){
	if (ballen.x == pad1.x && ballen.y == pad1.y || ballen.y == pad2.x && ballen.y == pad2.y || ballen.x == 0 || ballen.x == 320 || ballen.y == 0 || ballen.y == 240){
		return 1; 
	};
};
*/

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	struct Settings settings = initializeGame();
	// while (isGameFinished(P1Score, P2Score)){
		
	// }
	// ResetScreen();

    // game_dummy();
	printf("Rahim wuz here.");

	exit(EXIT_SUCCESS);
}
