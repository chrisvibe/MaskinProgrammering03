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
#include <time.h>

#define P1 1
#define P2 2
#define GAME_SPEED .5
int P1Score = 0; 
int P2Score = 0;
int maxScore = 5;

struct Canvas;
struct Game;
struct Canvas initializeBall(int x, int y);
struct Game initializeGame();
bool isGameFinished(int P1Score, int P2Score);
void mod_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
struct Canvas initializePads(int x, int y);
int whereCollision(struct Game* game);
void checkPadPositions(struct Game* game);
void handleCollision(struct Game* game);
int movePads();
int collisionDetectionBall(struct Game game);
int collisionDetectionPad(struct Game game);
void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade);
void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade);
void draw_ball(struct Game game);
void draw_pads(struct Game game);
struct Game timeStep(struct Game game);
void smart_refresh(struct Game game);
void movePad1(struct Game* game, int dy);
void movePad1(struct Game* game, int dy);

struct Game {
	struct Settings settings;
	struct Canvas ballen;
	struct Canvas pad1;
	struct Canvas pad2;
};

struct Game initializeGame(){
   struct Game game;
   struct Canvas ballen, pad1, pad2;

   game.ballen = ballen;
   game.pad1 = pad1;
   game.pad2 = pad2;

   init_ball(&game.ballen, 75, 200, 0xFFF, 0xF);
   init_pad(&game.pad1, 50, 40, 0xFFF, 0xF);
   init_pad(&game.pad2, 100, 40, 0xFFF, 0xF);
 
   game.settings = setup_display();
   return game;
}

//Checks whether the maxscore of P1 or P2 exceeds maxScore (a variable manually set).
bool isGameFinished(int P1Score, int P2Score){
	if (P1Score >= maxScore || P2Score >= maxScore){
		return 0;
	};
	return 1;
}

void mod_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade)
{
    init_canvas(canvas, x, y, width, height, speed, dx, dy, colour,fade);
}

void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    // init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);j
    init_canvas(canvas, x, y, 5, 5, 1, 3, 0, 0xFFF, 0xF);
}

void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    // init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
    init_canvas(canvas, x, y, 2, 30, 0, 0, 0, 0xFFF, 0xF);
}

int collisionDetectionBall(struct Game game){
	if (game.ballen.x - (game.ballen.width/2) == game.pad1.x + (game.pad1.width/2) && 
		(game.ballen.y + game.ballen.height <= game.pad1.y + game.pad1.height/2) || game.ballen.y + game.ballen.height <= game.pad1.y + game.pad1.height)
		{return 1;}

	else if (game.ballen.x + (game.ballen.width/2) == game.pad2.x - (game.pad2.width/2) && 
		(game.ballen.y - game.ballen.height <= game.pad1.y + game.pad1.height/2) || game.ballen.y + game.ballen.height <= game.pad2.y + game.pad2.height)
		{return 2;}

	else if( (game.ballen.y - (game.ballen.height/2) == 0) || (game.ballen.y + (game.ballen.height/2) == 240) )
		{return 3;}
	
	else if( (game.ballen.x - (game.ballen.width/2) < 0) || (game.ballen.x + (game.ballen.width/2) > 320) )
		{return 4;}
}

int collisionDetectionPad(struct Game game){
	if( (game.pad1.y - (game.pad1.height/2) == 0)  )
		{return 1;}
	
	else if ( (game.pad1.y + (game.pad1.width/2) == 240) )
		{return 2;}

	else if ( (game.pad2.y - (game.pad2.height/2) == 0)  )
		{return 3;}
	
	else if ((game.pad2.y + (game.pad2.width/2) == 240) )
		{return 4;}
}


/* data */
/* Checks for where collision occured */
int whereCollision(struct Game* game){

	if (game->ballen.x == game->ballen.y >= (game->ballen.y <= (game->pad1.y + 15))){ 
		return 0; 
	} 
	
	else if (game->ballen.x == game->pad2.x && (game->ballen.y >= game->pad2.y - 15 && game->ballen.y <= (game->pad2.y + 15))){
		return 1; 
	}

	/* wallcrash in vertical direction */ 
	else if (game->ballen.x <= 10  || game->ballen.x >= 310) {
		return 2;
	}

	/* wallcrash in horizontal direction */ 
	else if (game->ballen.y < 10 || game->ballen.y > 230) {
		return 3;
	}

	return -1; 
}

//Checks the pad positions and if they are outside the buffer, then update their positions back.
void checkPadPositions(struct Game* game){
	if (game->pad1.y < 16){
		game->pad1.y = 16; 
	}
	if (game->pad1.y > 224){
		game->pad1.y = 224; 
	}
	if (game->pad2.y < 16){
		game->pad2.y = 16; 
	}
	if (game->pad2.y > 224){
		game->pad2.y = 224; 
	}
}


/* dx or dy is multiplied by -1 depending on what type of crash it is (horizontal vs vertical) */
void handleCollision(struct Game* game){
	/* This method needs to be changed later as the ball will not change direction based on pad angle by using the current function */
	if (whereCollision(game) == 0){
		if (game->ballen.y >= (game->pad1.y + 10) && (game->ballen.dy < (game->pad1.y + 15))){
			game->ballen.dy = 2;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y >= (game->pad1.y + 5) && (game->ballen.y < (game->pad1.y + 10))){
			game->ballen.dy = 1;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y <= game->pad1.y + 5 || game->ballen.y > game->pad1.y - 5){
			game->ballen.dy = 0;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y >= (game->pad1.y - 15) && (game->ballen.y < (game->pad1.y - 10))){
			game->ballen.dy *= -2;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y >= (game->pad1.y - 10) && (game->ballen.y < (game->pad1.y - 5))){
			game->ballen.dy = -1;
			game->ballen.dx *= -1;	
		}
	}
	if (whereCollision(game) == 1){
		if (game->ballen.y >= (game->pad1.y + 10) && (game->ballen.dy < (game->pad1.y + 15))){
			game->ballen.dy = 2;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y >= (game->pad1.y + 5) && (game->ballen.y < (game->pad1.y + 10))){
			game->ballen.dy = 1;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y <= game->pad1.y + 5 || game->ballen.y > game->pad1.y - 5){
			game->ballen.dy = 0;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y >= (game->pad1.y - 15) && (game->ballen.y < (game->pad1.y - 10))){
			game->ballen.dy *= -2;
			game->ballen.dx *= -1;	
		}
		else if (game->ballen.y >= (game->pad1.y - 10) && (game->ballen.y < (game->pad1.y - 5))){
			game->ballen.dy = -1;
			game->ballen.dx *= -1;	
		}
	}
	if (whereCollision(game) == 2){
		if (game->ballen.x <= 10){
			P2Score += 1;
		}
		if (game->ballen.x >= 310){
			P1Score += 1;
		}
		//Her må det legges inn en "clean funksjon" som starter cleaner hele brettet og starter opp på nytt bare med en ny score. 
	}
	if (whereCollision(game) == 3){
		game->ballen.dy *= -1;
	}
	
}

struct Game timeStep(struct Game game){
    mod_canvas(&game.ballen, game.ballen.x, game.ballen.y, game.ballen.width, game.ballen.height, game.ballen.speed, game.ballen.dx, game.ballen.dy, 0, 0);
    // mod_canvas(&game.pad1, game.pad1.x, game.pad1.y, game.pad1.width, game.pad1.height, game.pad1.speed, game.pad1.dx, game.pad1.dy, 0, 0);
    // mod_canvas(&game.pad2, game.pad2.x, game.pad2.y, game.pad2.width, game.pad2.height, game.pad2.speed, game.pad2.dx, game.pad2.dy, 0, 0);

	draw_ball(game);
	// draw_pads(game);
	smart_refresh(game);

	handleCollision(&game);

    mod_canvas(&game.ballen, game.ballen.x + game.ballen.dx, game.ballen.y, game.ballen.width, game.ballen.height, game.ballen.speed, game.ballen.dx, game.ballen.dy, 0xFFF, 0xF);
    // mod_canvas(&game.pad1, game.pad1.x, game.pad1.y, game.pad1.width, game.pad1.height, game.pad1.speed, game.pad1.dx, game.pad1.dy, 0xFFF, 0xF);
    // mod_canvas(&game.pad2, game.pad2.x, game.pad2.y, game.pad2.width, game.pad2.height, game.pad2.speed, game.pad2.dx, game.pad2.dy, 0xFFF, 0xF);
	draw_ball(game);
	// draw_pads(game);
	smart_refresh(game);

    return game;
}

// still to be implemented properly
void smart_refresh(struct Game game){
	refresh_display(game.settings, 0, 0, WIDTH, HEIGHT);
}

// Function for converting input from driver to commands to pads

void movePad1(struct Game* game, int dy){
    game->pad1.y += dy;
    checkPadPositions(game);
}

void movePad2(struct Game* game, int dy){
	game->pad2.y += dy;
    checkPadPositions(game);
}

/* Function for reading from driver 
void checkDriver(){
	This function will check whether the driver is functioning and shut down game if not 
}

void checkLCD(){ 
	This funciton will check whether the LCD screen is functioning and shut down game if not
}

void ResetScreen(){
	 This function will clean the entire screen and start the game up again given that no player has reached the maxiumum score 
	 clear_screen(settings);
}

THINK THIS MIGHT BE REDUNDANT
 Check whether a collision has occured 
bool checkCollision(struct ball *ballen, struct pad *pad1, struct pad *pad2){
	if (ballen.x == pad1.x && ballen.y == pad1.y || ballen.y == pad2.x && ballen.y == pad2.y || ballen.x == 0 || ballen.x == 320 || ballen.y == 0 || ballen.y == 240){
		return 1; 
	};
};
*/

void draw_ball(struct Game game)
{
    // debug
    printf("ball x, y: %d %d\n", game.ballen.x, game.ballen.y);
    printf("ball dx, dy: %d %d\n", game.ballen.dx, game.ballen.dy);
    draw_canvas(&game.ballen, game.settings);
}

void draw_pads(struct Game game)
{
    draw_canvas(&game.pad1, game.settings);
    draw_canvas(&game.pad2, game.settings);
}

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	struct Game game = initializeGame();

    mod_canvas(&game.pad1, game.pad1.x, game.pad1.y, game.pad1.width, game.pad1.height, game.pad1.speed, game.pad1.dx, 1, 0, 0);
    mod_canvas(&game.pad2, game.pad2.x, game.pad2.y, game.pad2.width, game.pad2.height, game.pad2.speed, game.pad2.dx, 1, 0, 0);

	while (isGameFinished(P1Score, P2Score)){
        // timestep ------------------------------------------
        // debug
	    printf("where collision: %d" , whereCollision(&game));

        movePad1(&game, game.pad1.dy);
        movePad2(&game, game.pad2.dy);
        // erase old image
		mod_canvas(&game.ballen, game.ballen.x, game.ballen.y, game.ballen.width, game.ballen.height, game.ballen.speed, game.ballen.dx, game.ballen.dy, 0, 0);
    	mod_canvas(&game.pad1, game.pad1.x, game.pad1.y, game.pad1.width, game.pad1.height, game.pad1.speed, game.pad1.dx, game.pad1.dy, 0, 0);
    	mod_canvas(&game.pad2, game.pad2.x, game.pad2.y, game.pad2.width, game.pad2.height, game.pad2.speed, game.pad2.dx, game.pad2.dy, 0, 0);
		draw_ball(game);
		draw_pads(game);
		smart_refresh(game);

        sleep(GAME_SPEED);        

        // draw new image
		handleCollision(&game);
    	mod_canvas(&game.ballen, game.ballen.x + game.ballen.dx, game.ballen.y + game.ballen.dy, game.ballen.width, game.ballen.height, game.ballen.speed, game.ballen.dx, game.ballen.dy, 0xFFF, 0xF);
    	mod_canvas(&game.pad1, game.pad1.x + game.pad1.dx, game.pad1.y + game.pad1.dy, game.pad1.width, game.pad1.height, game.pad1.speed, game.pad1.dx, game.pad1.dy, 0xFFF, 0xF);
    	mod_canvas(&game.pad2, game.pad2.x + game.pad2.dx, game.pad2.y + game.pad2.dy, game.pad2.width, game.pad2.height, game.pad2.speed, game.pad2.dx, game.pad2.dy, 0xFFF, 0xF);
		draw_ball(game);
		draw_pads(game);
		smart_refresh(game);

        // timestep ------------------------------------------

        // game = timeStep(game);
	}

    free((&game.ballen)->pixels);
    free((&game.pad1)->pixels);
    free((&game.pad2)->pixels);
    tear_down_display(game.settings);
    printf("done.\n");

	exit(EXIT_SUCCESS);
}
