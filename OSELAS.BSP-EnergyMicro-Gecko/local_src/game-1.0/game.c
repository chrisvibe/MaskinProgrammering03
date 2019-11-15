#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
// #include <sys/stat.h>
// #include <unistd.h>

void refresh_display(int fbfd, int x, int y, int width, int height);

int main(int argc, char *argv[])
{
   printf("Hello World, I'm game!\n");

   int d_height = 240;
   int d_width = 320;
   /* uint16_t * addr; */
   uint16_t * addr;

   uint16_t pixels[d_height][d_width];
   uint16_t length = d_height * d_width * 2; // length in bites

   // open the frame buffer for read/write
   int fbfd = open("/dev/fb0", O_RDWR);

   // get address where we can store pixels (read implies write too)
   addr = mmap(NULL, 240*320*2, PROT_WRITE | PROT_READ, MAP_SHARED, fbfd, 0);

   // temp update pixels
   /* pixels[0][0] = 0xF; */
   /* pixels[1][0] = 0xF; */
   /* pixels[2][0] = 0xF; */
   /* pixels[0][1] = 0xF; */
   /* pixels[1][1] = 0xF; */
   /* pixels[2][1] = 0xF; */
   /* pixels[0][2] = 0xF; */
   /* pixels[1][2] = 0xF; */
   /* pixels[2][2] = 0xF; */

   // bruk addr direkte (en peker ~ en array i C)
   int x = 0;
   int y = 0;

   for (x = 0; x < d_height; x++) {
        for (y = 0; y < d_width; y++) {
            addr[x + y * d_width] = 0xF;
        }
        printf("%d\n", x);
   }

   refresh_display(fbfd, 0, 0, d_width, d_height);

   /* exit(EXIT_SUCCESS); */
}

void refresh_display(int fbfd, int x, int y, int width, int height) {
    // setup which part of the frame buffer that is to be refreshed
    // for performance reasons, use as small rectangle as possible
    struct fb_copyarea rect;

    rect.dx = x;
    rect.dy = y;
    rect.width = width;
    rect.height = height;

    // command driver to update display
    ioctl(fbfd, 0x4680, &rect);
}

