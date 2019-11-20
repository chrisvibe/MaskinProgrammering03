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

   return settings;
}

void tear_down_display(struct Settings settings)
{
   munmap(settings.addr, LENGTH);
   close(settings.fbfd);
}

void game_dummy()
{
   struct Settings settings = setup_display();

   draw_rectangle(settings, 0, 0, WIDTH, HEIGHT, 0); // clear screen
   draw_ball(settings, 100, 150);
   draw_pad(settings, 130, 160);
   draw_pad(settings, 70, 145);
   draw_rectangle(settings, 30, 50, 60, 80, 0xFFF);
   refresh_display(settings, 0, 0, HEIGHT, WIDTH);

   tear_down_display(settings);
}

void set_pixel(struct Settings settings, int x, int y, int colour)
{
    /* settings.addr[x + y * WIDTH] = colour; */
    settings.addr[x + (HEIGHT - y) * WIDTH] = colour; // alternative coordinate system
}

void refresh_display(struct Settings settings, int x, int y, int height, int width)
{
    // setup which part of the frame buffer that is to be refreshed
    // for performance reasons, use as small rectangle as possible
    struct fb_copyarea rect;

    rect.dx = x;
    rect.dy = y;
    rect.height = height;
    rect.width = width;

    // command driver to update display
    ioctl(settings.fbfd, 0x4680, &rect);
}

void draw_rectangle(struct Settings settings, int x1, int y1, int x2, int y2, int colour)
{
    int xi, yi;

    for (xi = x1; xi <= x2; xi++) {
        for (yi = y1; yi <= y2; yi++) {
            set_pixel(settings, xi, yi, colour);
        }
    }
}

void draw_ball(struct Settings settings, int x, int y)
{
    int diameter = 5;
    int dx = diameter / 2;
    draw_smooth_object(settings, x, y, dx, dx, 0xFFF, 0xF);
}

void draw_pad(struct Settings settings, int x, int y)
{
    int height = 30;
    int width = 2;
    int dx = width / 2;
    int dy = height / 2;
    draw_smooth_object(settings, x, y, dx, dy, 0xFFF, 0xF);
}

void draw_smooth_object(struct Settings settings, int x, int y, int dx, int dy, int colour, int fade)
{
    draw_rectangle(settings, x - dx, y - dy, x + dx, y + dy, colour);
    set_pixel(settings, x - dx, y - dy, fade);
    set_pixel(settings, x - dx, y + dy, fade);
    set_pixel(settings, x + dx, y - dy, fade);
    set_pixel(settings, x + dx, y + dy, fade);
}
