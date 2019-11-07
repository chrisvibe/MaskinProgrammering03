#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/fb.h>

int main(int argc, char *argv[])
{
   char *addr;
   int d_height = 3;
   int d_width = 3;

   uint16_t pixels[d_width][d_height];
   int *display = pixels;
   uint16_t length = d_width * d_height * 2; // length in bites

   // open the frame buffer for read/write
   fbfd = open("/dev/fb0", O_RDWR);

   // get address where we can store pixels
   addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fb, 0);

   // temp update pixels
   pixels[1][0] = 0xF
   pixels[2][0] = 0xF
   pixels[0][1] = 0xF
   pixels[0][2] = 0xF

   // write pixel matrix to said address
   int s = write(fbfd, &display, 10);

   refresh_display(&addr, 3, 3, 3, 3);
}

void refresh_display(char addr, int x, int y, int width, int height) {
    // setup which part of the frame buffer that is to be refreshed
    // for performance reasons, use as small rectangle as possible
    struct fb_copyarea rect;

    rect.dx = x;
    rect.dy = y;
    rect.width = width;
    rect.height = height;

    // command driver to update display
    ioctl(addr, 0x460, &rect);
}
