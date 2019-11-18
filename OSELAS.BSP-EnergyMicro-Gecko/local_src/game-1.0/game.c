#include "../../../pixel/display.h"
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

   printf("Done.\n");

   exit(EXIT_SUCCESS);
}
