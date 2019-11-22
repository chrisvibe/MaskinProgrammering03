#include "display_tools.c"
#include "display_tools.h"
#include "button_driver.c"
#include "button_driver.h"
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
int maxScore = 1;

struct Canvas;
struct Game;
void initializeGame(struct Game* game);
bool isGameFinished(int P1Score, int P2Score);
void mod_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
int whereCollision(struct Game* game);
void checkPadPositions(struct Game* game);
void handleCollision(struct Game* game);
void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade);
void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade);
void init_background(struct Canvas* canvas, int colour);
void draw_ball(struct Game game);
void draw_pads(struct Game game);
void draw_background(struct Game game);
struct Game timeStep(struct Game game);
void smart_refresh(struct Game* game);
void movePad1(struct Game* game, int dy);
void movePad2(struct Game* game, int dy);
void move_pads(struct Game* game);

struct Game {
	struct Settings* settings;
	struct Canvas* ballen;
	struct Canvas* pad1;
	struct Canvas* pad2;
	struct Canvas* background;
};

void initializeGame(struct Game* game){
   init_ball(game->ballen, WIDTH / 2, HEIGHT / 2, 0xFFF, 0xF);
   init_pad(game->pad1, 10, HEIGHT / 2, 0xFFF, 0xF);
   init_pad(game->pad2, WIDTH - 10, HEIGHT / 2, 0xFFF, 0xF);
   init_background(game->background, 0);
 
   setup_display(game->settings);
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
    init_canvas(canvas, x, y, 5, 5, 1, 3, 0, 0xFFF, 0xF);
}

void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    init_canvas(canvas, x, y, 2, 30, 0, 0, 1, 0xFFF, 0xF);
}

void init_background(struct Canvas* canvas, int colour)
{
    init_canvas(canvas, WIDTH / 2 - 10, HEIGHT / 2 - 10, WIDTH - 1, HEIGHT - 1, 0, 0, 0, colour, colour);
}

/* Checks for where collision occured */
int whereCollision(struct Game* game){

	if (game->ballen->x == game->pad1->x && (game->ballen->y >= game->pad1->y - 15 && game->ballen->y <= game->pad1->y + 15)){ 
		printf("returning 0\n");
		return 0; 
	} 
	
	else if (game->ballen->x == game->pad2->x && (game->ballen->y >= game->pad2->y - 15 && game->ballen->y <= game->pad2->y + 15)){
		printf("returning 1\n");
		return 1; 
	}

	/* wallcrash in vertical direction */ 
	else if (game->ballen->x <= 10  || game->ballen->x >= 310) {
		printf("returning 2\n");
		return 2;
	}

	/* wallcrash in horizontal direction */ 
	else if (game->ballen->y < 10 || game->ballen->y > 230) {
		printf("returning 3\n");
		return 3;
	}

	return -1; 
}

/* Checks the pad positions and if they are outside the buffer, then update their positions back. */
void checkPadPositions(struct Game* game){
	if (game->pad1->y < 16){
		game->pad1->y = 16; 
	}
	if (game->pad1->y > 224){
		game->pad1->y = 224; 
	}
	if (game->pad2->y < 16){
		game->pad2->y = 16; 
	}
	if (game->pad2->y > 224){
		game->pad2->y = 224; 
	}
}


/* dx or dy is multiplied by -1 depending on what type of crash it is (horizontal vs vertical) */
void handleCollision(struct Game* game){
	int collisionState = whereCollision(game);

	/* This method needs to be changed later as the ball will not change direction based on pad angle by using the current function */
	if (collisionState == 0){
		if ((game->ballen->y >= game->pad1->y + 10) && (game->ballen->y <= game->pad1->y + 15)){
			game->ballen->dy = 2;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y >= game->pad1->y + 5) && (game->ballen->y < game->pad1->y + 10)){
			game->ballen->dy = 1;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y < game->pad1->y + 5) && (game->ballen->y > game->pad1->y - 5)){
			game->ballen->dy = 0;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y >= game->pad1->y - 15) && (game->ballen->y <= game->pad1->y - 10)){
			game->ballen->dy = -2;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y > game->pad1->y - 10) && (game->ballen->y <= game->pad1->y - 5)){
			game->ballen->dy = -1;
			game->ballen->dx *= -1;	
		}
	}
	if (collisionState == 1){
		if ((game->ballen->y >= game->pad2->y + 10) && (game->ballen->y <= game->pad2->y + 15)){
			game->ballen->dy = 2;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y >= game->pad2->y + 5) && (game->ballen->y < game->pad2->y + 10)){
			game->ballen->dy = 1;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y < game->pad2->y + 5) && (game->ballen->y > game->pad2->y - 5)){
			game->ballen->dy = 0;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y >= game->pad2->y - 15) && (game->ballen->y <= game->pad2->y - 10)){
			game->ballen->dy = -2;
			game->ballen->dx *= -1;	
		}
		else if ((game->ballen->y > game->pad2->y - 10) && (game->ballen->y <= game->pad2->y - 5)){
			game->ballen->dy = -1;
			game->ballen->dx *= -1;	
		}
	}
	if (collisionState == 2){
		if (game->ballen->x <= 10){
			P2Score += 1;
		}
		if (game->ballen->x >= 310){
			P1Score += 1;
		}
		//Her må det legges inn en "clean funksjon" som starter cleaner hele brettet og starter opp på nytt bare med en ny score. 
	}
	if (collisionState == 3){
		game->ballen->dy *= -1;
	}
	
}

struct Game timeStep(struct Game game){
    return game;
}

void smart_refresh(struct Game* game){
	refresh_display(game->settings, 0, 0, WIDTH, HEIGHT);
}

/* Function for converting input from driver to commands to pads */
void movePad1(struct Game* game, int dy){
	if (dy == 0) {
		printf("WARNING: dy was null\n");
	}
    game->pad1->y += dy;
    checkPadPositions(game);
}

void movePad2(struct Game* game, int dy){
	if (dy == 0) {
		printf("WARNING: dy was null\n");
	}
	game->pad2->y += dy;
    checkPadPositions(game);
}

void move_pads(struct Game* game)
{
	int flippedButtonState = ~resultFromDeviceDriver;
	// Left player up
    if ((flippedButtonState & 0b00000010) != 0){
        movePad1(game, game->pad1->dy);
    } 
	// Left player down
    else if ((flippedButtonState & 0b00001000) != 0){
        movePad1(game, -game->pad1->dy);
    } 
	// Right player up
    if ((flippedButtonState & 0b00100000) != 0){
        movePad2(game, game->pad2->dy);
    }
	// Right player down
    else if ((flippedButtonState & 0b10000000) != 0){
        movePad2(game, -game->pad2->dy);
    } 
}

void draw_ball(struct Game game)
{
    // debug
    draw_canvas(game.ballen, game.settings);
}

void draw_pads(struct Game game)
{
    draw_canvas(game.pad1, game.settings);
    draw_canvas(game.pad2, game.settings);
}

void draw_background(struct Game game)
{
    draw_canvas(game.background, game.settings);
}

int main(int argc, char *argv[])
{
	  printf("Hello World, I'm game!\n");
    init_gamepad();

    struct Game game;
	  struct Canvas *ballen, *pad1, *pad2, *background;
    game.ballen = malloc(sizeof(struct Canvas));
    game.pad1 = malloc(sizeof(struct Canvas));
    game.pad2 = malloc(sizeof(struct Canvas));
    game.background = malloc(sizeof(struct Canvas));
    game.settings = malloc(sizeof(struct Settings));
	  initializeGame(&game);

    while (isGameFinished(P1Score, P2Score)){
        sleep(GAME_SPEED);
        game.ballen->x += game.ballen->dx;
        game.ballen->y += game.ballen->dy;
        draw_ball(game);
        draw_pads(game);
        smart_refresh(&game);
	  }

    shutdown_driver();
    tear_down_display(game.settings);
    free(game.ballen->pixels);
    free(game.pad1->pixels);
    free(game.pad2->pixels);
    free(game.background->pixels);
    free(game.ballen);
    free(game.pad1);
    free(game.pad2);
    free(game.background);
}
