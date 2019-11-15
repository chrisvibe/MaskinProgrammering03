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

   int *addr;
   int d_height = 320;
   int d_width = 240;

   u_int16_t pixels[d_height][d_width];
   u_int16_t length = d_height * d_width * 2; // length in bites

   // open the frame buffer for read/write
   int fbfd = open("/dev/fb0", O_RDWR);

   // get address where we can store pixels (read implies write too)
   addr = mmap(NULL, length, PROT_WRITE, MAP_SHARED, fbfd, 0);

   // temp update pixels
   pixels[0][0] = 0xF;
   pixels[1][0] = 0xF;
   pixels[2][0] = 0xF;
   pixels[0][1] = 0xF;
   pixels[1][1] = 0xF;
   pixels[2][1] = 0xF;
   pixels[0][2] = 0xF;
   pixels[1][2] = 0xF;
   pixels[2][2] = 0xF;

   int i = 0;
   int j = 0;
   for (i = 0; i < d_height; i++) {
        for (j = 0; j < d_width; j++) {
            *(addr + 16*(i + j * d_width)) = pixels[i][j];
        }
   }

   refresh_display(fbfd, 0, 0, 320, 240);

   printf("Sanity check");

    printf("\n");
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            printf("%d ", pixels[i][j]);
        }
        printf("\n");
    }

   printf("Sanity check");

   exit(EXIT_SUCCESS);
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

