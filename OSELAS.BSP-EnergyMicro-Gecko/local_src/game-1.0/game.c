#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
// #include <sys/stat.h>
// #include <unistd.h>

void refresh_display(int fbfd, int x, int y, int width, int height);

int main(int argc, char *argv[])
{
   printf("Hello World, I'm game!\n");

   int d_height = 3;
   int d_width = 3;
   /* u_int16_t * addr; */
   u_int16_t ** addr;

   u_int16_t pixels[d_height][d_width];
   u_int16_t length = d_height * d_width * 2; // length in bites

   // open the frame buffer for read/write
   int fbfd = open("/dev/fb0", O_RDWR);

   // get address where we can store pixels (read implies write too)
   addr = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fbfd, 0);

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
   /* addr[x + y * d_width] = 0xF; */
   addr[y][x] = 0xF;

   /* int i; */
   /* int j; */
   /* for (i = 0; i < d_height; i++) { */
   /*      for (j = 0; j < d_width; j++) { */
   /*          *(addr + (i + j * d_height)) = pixels[i][j]; */
   /*      } */
   /* } */

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
    ioctl(fbfd, 0x460, &rect);
}

