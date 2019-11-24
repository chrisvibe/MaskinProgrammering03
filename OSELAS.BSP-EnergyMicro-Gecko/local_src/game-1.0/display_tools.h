#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

#define HEIGHT 240 // display dimensions
#define WIDTH 320
#define LENGTH HEIGHT *WIDTH * 2 // length in bites

struct Settings;
struct Settings setup_display();
void tear_down_display(struct Settings settings);
void refresh_display(struct Settings settings, int x, int y, int width,
		     int height);
void clear_screen(struct Settings settings);

struct Canvas;
void set_pixel(struct Canvas *canvas, int x, int y, int colour);
void draw_pixel(struct Settings settings, int x, int y, int colour);
void draw_canvas(struct Canvas *canvas, struct Settings settings);
void erase_canvas(struct Canvas *canvas, struct Settings settings);
void fill(struct Canvas *canvas, int colour);
void init_canvas(struct Canvas *canvas, int x, int y, int width, int height,
		 int speed, int dx, int dy, int colour, int fade);
void smooth(struct Canvas *canvas, int fade);
void print_canvas(struct Canvas *canvas);
