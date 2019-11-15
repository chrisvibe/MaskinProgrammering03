#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

void refresh_display(int fbfd, int x, int y, int width, int height);

int main(int argc, char *argv[])
{
   printf("Hello World, I'm game!\n");

   exit(EXIT_SUCCESS);
}
