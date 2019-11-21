#include "display_tools.h"

struct Settings {
int fbfd;
uint16_t * addr;
};

struct Settings setup_display()
{
   struct Settings settings;

   // open the frame buffer for read/write
   settings.fbfd = open("/dev/fb0", O_RDWR);

   // get address where we can store pixels (write implies read too)
   settings.addr = (uint16_t *) mmap(NULL, LENGTH, PROT_WRITE, MAP_SHARED, settings.fbfd, 0);

   clear_screen(settings);

   return settings;
}

void tear_down_display(struct Settings settings)
{
   munmap(settings.addr, LENGTH);
   close(settings.fbfd);
}

void clear_screen(struct Settings settings)
{
   /* draw_rectangle(settings, 0, 0, WIDTH, HEIGHT, 0); // clear screen */
}

/* { */
/*    struct Settings settings = setup_display(); */

/*    draw_ball(settings, 100, 150,  0xFFF, 0xFFF); */
/*    draw_pad(settings, 130, 160,  0xFFF,  0xFFF); */
/*    draw_pad(settings, 70, 145, 0xFFF,  0xFFF); */
/*    draw_rectangle(settings, 30, 50, 60, 80, 0xFFF); */
/*    refresh_display(settings, 0, 0, HEIGHT, WIDTH); */

/*    tear_down_display(settings); */
/* } */

/* void set_pixel(struct Settings settings, int x, int y, int colour) */
/* { */
/*     /1* settings.addr[x + y * WIDTH] = colour; *1/ */
/*     settings.addr[x + (HEIGHT - y) * WIDTH] = colour; // alternative coordinate system */
/* } */

/* void refresh_display(struct Settings settings, int x, int y, int height, int width) */
/* { */
/*     // setup which part of the frame buffer that is to be refreshed */
/*     // for performance reasons, use as small rectangle as possible */
/*     struct fb_copyarea rect; */

/*     rect.dx = x; */
/*     rect.dy = y; */
/*     rect.height = height; */
/*     rect.width = width; */

/*     // command driver to update display */
/*     ioctl(settings.fbfd, 0x4680, &rect); */
/* } */

/* void draw_rectangle(struct Settings settings, int x1, int y1, int x2, int y2, int colour) */
/* { */
/*     int xi, yi; */

/*     for (xi = x1; xi <= x2; xi++) { */
/*         for (yi = y1; yi <= y2; yi++) { */
/*             set_pixel(settings, xi, yi, colour); */
/*         } */
/*     } */
/* } */

/* void draw_ball(struct Settings settings, int x, int y, int colour, int fade) */
/* { */
/*     int diameter = 5; */
/*     int dx = diameter / 2; */
/*     draw_smooth_object(settings, x, y, dx, dx, colour, fade); */
/* } */

/* void draw_pad(struct Settings settings, int x, int y, int colour, int fade) */
/* { */
/*     int height = 30; */
/*     int width = 2; */
/*     int dx = width / 2; */
/*     int dy = height / 2; */
/*     draw_smooth_object(settings, x, y, dx, dy, colour, fade); */
/* } */

/* void draw_smooth_object(struct Settings settings, int x, int y, int dx, int dy, int colour, int fade) */
/* { */
/*     draw_rectangle(settings, x - dx, y - dy, x + dx, y + dy, colour); */
/*     set_pixel(settings, x - dx, y - dy, fade); */
/*     set_pixel(settings, x - dx, y + dy, fade); */
/*     set_pixel(settings, x + dx, y - dy, fade); */
/*     set_pixel(settings, x + dx, y + dy, fade); */
/* } */

// new paradigm --------------------------------------

struct Canvas {
    uint16_t width;
    uint16_t height;
    uint16_t x;  // centroid placement in global display
    uint16_t y;
    uint16_t * pixels;
};

void set_pixel(struct Canvas* canvas, int x, int y, int colour)
{
    canvas->pixels[x + y * canvas->width] = colour;
}

void fill(struct Canvas* canvas, int colour)
{
    int xi, yi;

    for (yi = 0; yi < canvas->height; yi++) {
        for (xi = 0; xi < canvas->width; xi++) {
            set_pixel(canvas, xi, yi, colour);
        }
    }
}

void smooth(struct Canvas* canvas, int fade)
{
    set_pixel(canvas, 0, 0, fade);
    set_pixel(canvas, 0, canvas->height - 1, fade);
    set_pixel(canvas, canvas->width - 1, 0, fade);
    set_pixel(canvas, canvas->width - 1, canvas->height - 1, fade);
}

void init_ball(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    canvas->x = x;
    canvas->y = y;
    canvas->width = 5;
    canvas->height = 5;
    uint16_t length = canvas->width * canvas->height;
    canvas->pixels = malloc(length * sizeof(uint16_t));

    fill(canvas, colour);
    smooth(canvas, fade);
}

void init_pad(struct Canvas* canvas, int x, int y, int colour, int fade)
{
    canvas->x = x;
    canvas->y = y;
    canvas->width = 2;
    canvas->height = 30;
    uint16_t length = canvas->width * canvas->height;
    canvas->pixels = malloc(length * sizeof(uint16_t));

    fill(canvas, colour);
    smooth(canvas, fade);
}

void print_canvas(struct Canvas* canvas)
{
    printf("\n");
    int x, y;
    for (y = 0; y < canvas->height; y++) {
        for (x = 0; x < canvas->width; x++) {
            printf("%d ", canvas->pixels[x + y * canvas->width]);
        }
        printf("\n");
    }
}

void draw_canvas(struct Canvas* canvas)
{
    uint16_t x, y;
    for (y = 0; y < canvas->height; y++) {
        for (x = 0; x < canvas->width; x++) {
            printf("%d ", canvas->pixels[x + y * canvas->width]);
        }
    }
}

void draw_pixel(struct Settings settings, uint16_t x, uint16_t y)
{
    settings.addr[x + (HEIGHT - y) * WIDTH] = colour;
}

/* void game_dummy() */
int main(void) {

    struct Canvas ballen, pad;
    init_ball(&ballen, 5, 5, 0xFFF, 0xF);
    init_pad(&pad, 20, 20, 0xFFF, 0xF);
    
    print_canvas(&ballen);
    print_canvas(&pad);

    free((&ballen)->pixels);
    free((&pad)->pixels);
    printf("done.\n");
    return 0;
}

