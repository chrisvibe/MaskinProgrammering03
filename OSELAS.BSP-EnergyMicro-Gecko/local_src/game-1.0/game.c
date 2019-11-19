/* #include "../../../pixel/display.h" */
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

void set_pixel();
void refresh_display(int fbfd, int x, int y, int height, int width);

int main(int argc, char *argv[])
{
   printf("Hello World, I'm game!\n");

   set_pixel();
    
   exit(EXIT_SUCCESS);
}


void set_pixel()
{
   int d_height = 240;
   int d_width = 320;
   int * addr;

   int length = d_height * d_width * 2; // length in bites

   // open the frame buffer for read/write
   int fbfd = open("/dev/fb0", O_RDWR);

   // get address where we can store pixels (write implies read too)
   addr = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fbfd, 0);

   int x = 0;
   int y = 0;
   for (x = 0; x < d_height; x++) {
        for (y = 0; y < d_width; y++) {
            addr[x * d_width + y] = 0xF;
        }
   }

   refresh_display(fbfd, 0, 0, d_height, d_width);

   munmap(addr, length);
   close(fbfd);
}

void refresh_display(int fbfd, int x, int y, int height, int width)
{
    // setup which part of the frame buffer that is to be refreshed
    // for performance reasons, use as small rectangle as possible
    struct fb_copyarea rect;

    rect.dx = x;
    rect.dy = y;
    rect.height = height;
    rect.width = width;

    // command driver to update display
    ioctl(fbfd, 0x4680, &rect);
}
