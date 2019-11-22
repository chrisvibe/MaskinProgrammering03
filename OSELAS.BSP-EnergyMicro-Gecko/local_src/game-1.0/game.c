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
#define GAME_SPEED .1
#define MAX_SCORE 1 
#define DEBUG 1

struct Canvas;
void mod_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade);
void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade);
void init_background(struct Canvas* canvas, int colour);

struct Game;
struct Game initGame();
void endGame(struct Game* game);
void timeStep(struct Game* game);
bool isGameFinished(struct Game game);
void checkPadPositions(struct Game* game);
void handleCollision(struct Game* game);
void move(struct Canvas* canvas);
void movePad1(struct Game* game);
void movePad2(struct Game* game);
void draw_ball(struct Game* game);
void draw_pads(struct Game* game);
void smart_refresh(struct Game* game);

int collisionDetectionBall(struct Game game);
int collisionDetectionPad(struct Game game);
int whereCollision(struct Game* game);

struct Game{
	struct Settings settings;
	struct Canvas background;
	struct Canvas ballen;
	struct Canvas pad1;
	struct Canvas pad2;
    int P1Score;
    int P2Score;
};

struct Game initGame(){
   printf("----------PONG GAME----------\n");
   struct Game game;
   struct Canvas background, ballen, pad1, pad2;

   game.background = background;
   game.ballen = ballen;
   game.pad1 = pad1;
   game.pad2 = pad2;
   game.P1Score = 0;
   game.P2Score = 0;

   init_ball(&game.ballen, 75, 200, 0xFFF, 0xF);
   init_pad(&game.pad1, 50, 40, 0xFFF, 0xF);
   init_pad(&game.pad2, 100, 40, 0xFFF, 0xF);
   init_background(&game.background, 0);
 
   game.settings = setup_display();
   return game;
}

void endGame(struct Game* game)
{
    free(&(game->background.pixels));
    free(&(game->ballen.pixels));
    free(&(game->pad1.pixels));
    free(&(game->pad2.pixels));
    tear_down_display(game->settings);
    printf("----------GAME OVER----------\n");
}

//Checks whether the MAX_SCORE of P1 or P2 exceeds MAX_SCORE (a variable manually set).
bool isGameFinished(struct Game game){
    return game.P1Score >= MAX_SCORE || game.P2Score >= MAX_SCORE;
}

void mod_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade)
{
    init_canvas(canvas, x, y, width, height, speed, dx, dy, colour,fade);
}

void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    // init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);j
    init_canvas(canvas, x, y, 5, 5, 1, 1, 0, 0xFFF, 0xF);
}

void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    // init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
    init_canvas(canvas, x, y, 2, 30, 0, 0, 1, 0xFFF, 0xF);
}

void init_background(struct Canvas* canvas, int colour)
{
    // init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
    init_canvas(canvas, 0, 0, WIDTH, HEIGHT, 0, 0, 0, colour, colour);
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
			game->P2Score += 1;
            // draw_canvas(&(game->background), game->settings);
		}
		if (game->ballen.x >= 310){
			game->P1Score += 1;
            // draw_canvas(&(game->background), game->settings);
		}
		//Her må det legges inn en "clean funksjon" som starter cleaner hele brettet og starter opp på nytt bare med en ny score. 
	}
	if (whereCollision(game) == 3){
		game->ballen.dy *= -1;
	}
	
}

void move(struct Canvas* canvas)
{
    canvas->x += canvas->dx;
    canvas->y += canvas->dy;
}

void timeStep(struct Game* game){
        // move stuff
        move(&(game->ballen));
        movePad1(game);
        movePad2(game);
        
        // erase old 
        erase_canvas(&game->ballen, game->settings);
        erase_canvas(&game->pad1, game->settings);
        erase_canvas(&game->pad2, game->settings);
		smart_refresh(game);

        // seperate erasing old and writting new
        sleep(GAME_SPEED);        
        
        // draw new 
		handleCollision(game);
		draw_ball(game);
		draw_pads(game);
		smart_refresh(game);

        if (DEBUG){
	        printf("where collision: %d", whereCollision(game));
        }
}

void smart_refresh(struct Game* game){
	refresh_display(game->settings, game->pad1.x0, game->pad1.y0, game->pad1.width, game->pad1.height);
	refresh_display(game->settings, game->pad2.x0, game->pad2.y0, game->pad2.width, game->pad2.height);
	refresh_display(game->settings, game->ballen.x0, game->ballen.y0, game->ballen.width, game->ballen.height);
}

// Function for converting input from driver to commands to pads

void movePad1(struct Game* game){
    move(&(game->pad1));
    checkPadPositions(game);
}

void movePad2(struct Game* game){
    move(&(game->pad2));
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

void draw_ball(struct Game* game)
{
    // debug
    if (DEBUG){
        printf("ball: x=%d, y=%d, dx=%d, dy=%d\n", game->ballen.x, game->ballen.y, game->ballen.dx, game->ballen.dy);
    }
    draw_canvas(&(game->ballen), game->settings);
}

void draw_pads(struct Game* game)
{
    draw_canvas(&(game->pad1), game->settings);
    draw_canvas(&(game->pad2), game->settings);
}

int main(int argc, char *argv[])
{
	struct Game game = initGame();

	while (isGameFinished(game)){
        timeStep(&game);
	}

	exit(EXIT_SUCCESS);
    return 0;
}
