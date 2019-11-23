#include "display_tools.h"

struct Settings {
int fbfd;
uint16_t * addr;
};

struct Canvas {
    uint16_t x;  // centroid placement in global display
    uint16_t y;
    uint16_t x0; // corner placement in global display
    uint16_t y0;
    uint16_t width;
    uint16_t height;
	int16_t speed;
	int16_t dx;
	int16_t dy;
    uint16_t * pixels;
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
    /* struct Canvas screen; */
    /* init_canvas(&screen, WIDTH / 2, HEIGHT / 2, WIDTH - 1, HEIGHT - 1, 0, 0, 0, 0, 0); */
    /* draw_canvas(&screen, settings); */
    /* free(screen.pixels); */
}

void refresh_display(struct Settings settings, int x, int y, int width, int height)
{
    // setup which part of the frame buffer that is to be refreshed */
    // for performance reasons, use as small rectangle as possible */
    struct fb_copyarea rect;

    rect.dx = x; 
    rect.dy = y; 
    rect.height = height;
    rect.width = width;

    // command driver to update display */
    ioctl(settings.fbfd, 0x4680, &rect);
}

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

void draw_canvas(struct Canvas* canvas, struct Settings settings)
{
    int x, y;
    for (y = 0; y < canvas->height; y++) {
        for (x = 0; x < canvas->width; x++) {
            draw_pixel(settings, x + canvas->x0, y + canvas->y0, canvas->pixels[x + y * canvas->width]);
        }
    }
}

void draw_pixel(struct Settings settings, int x, int y, int colour)
{
    settings.addr[x + (HEIGHT - y) * WIDTH] = colour;
}


void erase_canvas(struct Canvas * canvas, struct Settings settings)
{
    int x, y;
    for (y = 0; y < canvas->height; y++) {
        for (x = 0; x < canvas->width; x++) {
            draw_pixel(settings, x + canvas->x0, y + canvas->y0, 0);
        }
    }
}

void init_canvas(struct Canvas* canvas, int x, int y, int width, int height, int speed, int dx, int dy, int colour, int fade)
{
    canvas->x = x;
    canvas->y = y;
    canvas->width = width;
    canvas->height = height;
    canvas->x0 = canvas->x - canvas->width / 2;
    canvas->y0 = canvas->y - canvas->height / 2;
	canvas->speed = speed; /* Setter her speed til 1, men dette kan endres */
	canvas->dx = dx; /* Setter dx og dy til random nummer/rand max slik at vi har en verdi mellom 0 og 1 */
	canvas->dy = dy; /* Betyr at ballen kommer til å gå mot høyre hver gang */
    uint16_t length = canvas->width * canvas->height;
    canvas->pixels = malloc(length * sizeof(uint16_t));

    fill(canvas, colour);
    smooth(canvas, fade);
}

// int main(void) {
void game_dummy()
{
//     struct Settings settings = setup_display();
//     struct Canvas ballen, pad;

//     init_ball(&ballen, 100, 100, 0xFFF, 0xF);
//     init_pad(&pad, 50, 50, 0xFFF, 0xF);
    
//     print_canvas(&ballen);
//     print_canvas(&pad);

//     draw_canvas(&ballen, settings);
//     draw_canvas(&pad, settings);

//     refresh_display(settings, 0, 0, WIDTH, HEIGHT);

//     free((&ballen)->pixels);
//     free((&pad)->pixels);
//     printf("done.\n");
}

