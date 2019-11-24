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
#define GAME_SPEED .8
#define maxScore 3
int P1Score = 0;
int P2Score = 0;

struct Canvas;
struct Game;
struct Canvas initializeBall(int x, int y);
void initializeGame(struct Game *game);
bool isGameFinished(int P1Score, int P2Score);
void mod_canvas(struct Canvas *canvas, int x, int y, int width, int height,
		int speed, int dx, int dy, int colour, int fade);
struct Canvas initializePads(int x, int y);
int whereCollision(struct Game *game);
void checkPadPositions(struct Game *game);
void handleCollision(struct Game *game);
int collisionDetectionBall(struct Game game);
int collisionDetectionPad(struct Game game);
void init_ball(struct Canvas *canvas, int x, int y, int colour, int fade);
void init_pad(struct Canvas *canvas, int x, int y, int colour, int fade);
void init_background(struct Canvas *canvas, int x, int y, int colour, int fade);
void draw_ball(struct Game game);
void draw_pads(struct Game game);
void reset_game_round(struct Game *game);
struct Game timeStep(struct Game game);
void smart_refresh(struct Game game);
void move_pads(struct Game *game);
void reset_game_round(struct Game *game);

void movePad1(struct Game *game, int dy);
void movePad2(struct Game *game, int dy);

struct Game {
	struct Settings settings;
	struct Canvas ballen;
	struct Canvas pad1;
	struct Canvas pad2;
	struct Canvas background;
};

void initializeGame(struct Game *game)
{
	init_ball(&(game->ballen), WIDTH / 2, HEIGHT / 2, 0xFFF, 0xF);
	init_pad(&(game->pad1), 10, HEIGHT / 2, 0xFFF, 0xF);
	init_pad(&(game->pad2), 310, HEIGHT / 2, 0xFFF, 0xF);
	init_background(&(game->background), WIDTH / 2, HEIGHT / 2, 0, 0);

	game->settings = setup_display();
}

//Checks whether the maxscore of P1 or P2 exceeds maxScore (a variable manually set).
bool isGameFinished(int P1Score, int P2Score)
{
	if (P1Score >= maxScore || P2Score >= maxScore) {
		return 0;
	};
	return 1;
}

void reset_game_round(struct Game *game)
{
	initializeGame(game);
	draw_canvas(&(game->background), game->settings);
	refresh_display(game->settings, 0, 0, WIDTH, HEIGHT);
	printf("Round %d -- score P1: %d -- score P2: %d\n", P1Score + P2Score,
	       P1Score, P2Score);
}

void mod_canvas(struct Canvas *canvas, int x, int y, int width, int height,
		int speed, int dx, int dy, int colour, int fade)
{
	init_canvas(canvas, x, y, width, height, speed, dx, dy, colour, fade);
}

void init_ball(struct Canvas *canvas, int x, int y, int colour, int fade)
{
	// init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);j
	init_canvas(canvas, x, y, 6, 6, 1, 1, 0, colour, fade);
}

void init_pad(struct Canvas *canvas, int x, int y, int colour, int fade)
{
	// init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
	init_canvas(canvas, x, y, 2, 30, 0, 0, 1, colour, fade);
}

void init_background(struct Canvas *canvas, int x, int y, int colour, int fade)
{
	// init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade);
	init_canvas(canvas, x, y, WIDTH, HEIGHT, 0, 0, 0, colour, fade);
}

int whereCollision(struct Game *game)
{
	if (game->ballen.x == game->pad1.x &&
	    (game->ballen.y >= game->pad1.y - 15 &&
	     game->ballen.y <= game->pad1.y + 15)) {
		return 0;
	}

	else if (game->ballen.x == game->pad2.x &&
		 (game->ballen.y >= game->pad2.y - 15 &&
		  game->ballen.y <= game->pad2.y + 15)) {
		return 1;
	}

	/* wallcrash in vertical direction */
	else if (game->ballen.x <= 10 || game->ballen.x >= 310) {
		return 2;
	}

	/* wallcrash in horizontal direction */
	else if (game->ballen.y < 10 || game->ballen.y > 230) {
		return 3;
	}

	return -1;
}

//Checks the pad positions and if they are outside the buffer, then update their positions back.
void checkPadPositions(struct Game *game)
{
	if (game->pad1.y < 16) {
		game->pad1.y = 16;
	}
	if (game->pad1.y > 224) {
		game->pad1.y = 224;
	}
	if (game->pad2.y < 16) {
		game->pad2.y = 16;
	}
	if (game->pad2.y > 224) {
		game->pad2.y = 224;
	}
}

/* dx or dy is multiplied by -1 depending on what type of crash it is (horizontal vs vertical) */
void handleCollision(struct Game *game)
{
	int collisionState = whereCollision(game);

	/* This method needs to be changed later as the ball will not change direction based on pad angle by using the current function */
	if (collisionState == 0) {
		if ((game->ballen.y >= game->pad1.y + 10) &&
		    (game->ballen.y <= game->pad1.y + 15)) {
			game->ballen.dy = 2;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y >= game->pad1.y + 5) &&
			   (game->ballen.y < game->pad1.y + 10)) {
			game->ballen.dy = 1;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y < game->pad1.y + 5) &&
			   (game->ballen.y > game->pad1.y - 5)) {
			game->ballen.dy = 0;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y >= game->pad1.y - 15) &&
			   (game->ballen.y <= game->pad1.y - 10)) {
			game->ballen.dy = -2;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y > game->pad1.y - 10) &&
			   (game->ballen.y <= game->pad1.y - 5)) {
			game->ballen.dy = -1;
			game->ballen.dx *= -1;
		}
	}
	if (collisionState == 1) {
		if ((game->ballen.y >= game->pad2.y + 10) &&
		    (game->ballen.y <= game->pad2.y + 15)) {
			game->ballen.dy = 2;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y >= game->pad2.y + 5) &&
			   (game->ballen.y < game->pad2.y + 10)) {
			game->ballen.dy = 1;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y < game->pad2.y + 5) &&
			   (game->ballen.y > game->pad2.y - 5)) {
			game->ballen.dy = 0;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y >= game->pad2.y - 15) &&
			   (game->ballen.y <= game->pad2.y - 10)) {
			game->ballen.dy = -2;
			game->ballen.dx *= -1;
		} else if ((game->ballen.y > game->pad2.y - 10) &&
			   (game->ballen.y <= game->pad2.y - 5)) {
			game->ballen.dy = -1;
			game->ballen.dx *= -1;
		}
	}
	if (collisionState == 2) {
		if (game->ballen.x <= 10) {
			P2Score++;
			reset_game_round(game);
		}
		if (game->ballen.x >= 310) {
			P1Score++;
			reset_game_round(game);
		}
	}
	if (collisionState == 3) {
		game->ballen.dy *= -1;
	}

	// flush values modified above.
	mod_canvas(&(game->ballen), game->ballen.x, game->ballen.y,
		   game->ballen.width, game->ballen.height, game->ballen.speed,
		   game->ballen.dx, game->ballen.dy, 0xFFF, 0xF);
}

void movePad1(struct Game *game, int dy)
{
	mod_canvas(&(game->pad1), game->pad1.x, game->pad1.y + dy,
		   game->pad1.width, game->pad1.height, game->pad1.speed,
		   game->pad1.dx, game->pad1.dy, 0xFFF, 0xF);
	checkPadPositions(game);
}

void movePad2(struct Game *game, int dy)
{
	mod_canvas(&(game->pad2), game->pad2.x, game->pad2.y + dy,
		   game->pad2.width, game->pad2.height, game->pad2.speed,
		   game->pad2.dx, game->pad2.dy, 0xFFF, 0xF);
	checkPadPositions(game);
}

void move_pads(struct Game *game)
{
	int flippedButtonState = ~resultFromDeviceDriver;
	// Left player up
	if ((flippedButtonState & 0b00000010) != 0) {
		movePad1(game, game->pad1.dy);
	}
	// Left player down
	else if ((flippedButtonState & 0b00001000) != 0) {
		movePad1(game, -game->pad1.dy);
	}
	// Right player up
	if ((flippedButtonState & 0b00100000) != 0) {
		movePad2(game, game->pad2.dy);
	}
	// Right player down
	else if ((flippedButtonState & 0b10000000) != 0) {
		movePad2(game, -game->pad2.dy);
	}
}

struct Game timeStep(struct Game game)
{
	return game;
}

void smart_refresh(struct Game game)
{
	// refresh_display(game.settings, 0, 0, WIDTH, HEIGHT);
	refresh_display(game.settings, game.pad1.x0,
			HEIGHT - game.pad1.height - game.pad1.y0,
			game.pad1.width, game.pad1.height);
	refresh_display(game.settings, game.pad2.x0,
			HEIGHT - game.pad2.height - game.pad2.y0,
			game.pad2.width, game.pad2.height);
	refresh_display(game.settings, game.ballen.x0,
			HEIGHT - game.ballen.height - game.ballen.y0,
			game.ballen.width, game.ballen.height);
}

void draw_ball(struct Game game)
{
	draw_canvas(&game.ballen, game.settings);
}

void draw_pads(struct Game game)
{
	draw_canvas(&game.pad1, game.settings);
	draw_canvas(&game.pad2, game.settings);
}

int main(int argc, char *argv[])
{
	init_gamepad();
	printf("Hello World, I'm game!\n");
	struct Game game;
	struct Canvas ballen, pad1, pad2, background;
	game.ballen = ballen;
	game.pad1 = pad1;
	game.pad2 = pad2;
	game.background = background;
	reset_game_round(&game);
	sleep(1);

	while (isGameFinished(P1Score, P2Score)) {
		// timestep ------------------------------------------

		// erase old image
		smart_refresh(game);
		erase_canvas(&(game.ballen), game.settings);
		smart_refresh(game);
		erase_canvas(&(game.pad1), game.settings);
		smart_refresh(game);
		erase_canvas(&(game.pad2), game.settings);
		smart_refresh(game);

		// sleep(0.1);

		// draw new image
		mod_canvas(&game.ballen, game.ballen.x + game.ballen.dx,
			   game.ballen.y + game.ballen.dy, game.ballen.width,
			   game.ballen.height, game.ballen.speed,
			   game.ballen.dx, game.ballen.dy, 0xFFF, 0xF);
		handleCollision(&game);
		move_pads(&game);
		draw_ball(game);
		smart_refresh(game);
		draw_pads(game);
		smart_refresh(game);

		sleep(GAME_SPEED);

		// timestep ------------------------------------------
	}

	free((&game.ballen)->pixels);
	free((&game.pad1)->pixels);
	free((&game.pad2)->pixels);
	free((&game.background)->pixels);
	tear_down_display(game.settings);
	shutdown_driver();
	printf("done.\n");

	exit(EXIT_SUCCESS);
}
