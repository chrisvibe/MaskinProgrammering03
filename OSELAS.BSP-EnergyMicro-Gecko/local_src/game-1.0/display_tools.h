#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

#define HEIGHT 240 // display dimensions
#define WIDTH 320
#define LENGTH HEIGHT * WIDTH * 2 // length in bites

/* struct Settings; */
/* struct Settings setup_display(); */
/* void game_dummy(); */
/* void tear_down_display(struct Settings settings); */
/* void refresh_display(struct Settings settings, int x, int y, int height, int width); */
/* void clear_screen(struct Settings settings); */
/* void set_pixel(struct Settings settings, int x, int y, int colour); */
/* void draw_rectangle(struct Settings settings, int x1, int y1, int x2, int y2, int colour); */
/* void draw_smooth_object(struct Settings settings, int x, int y, int dx, int dy, int colour, int fade); */
/* void draw_ball(struct Settings settings, int x, int y, int colour, int fade); */
/* void draw_pad(struct Settings settings, int x, int y, int colour, int fade); */

struct Canvas;
void set_pixel(struct Canvas* canvas, int x, int y, int colour);
void fill(struct Canvas* canvas, int colour);
void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade);
void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade);
void smooth(struct Canvas* canvas, int fade);
void print_canvas(struct Canvas* canvas);
