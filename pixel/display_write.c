#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
   char *addr;
   int d_height = 3;
   int d_width = 3;

   uint16_t display[d_width][d_height];
   uint16_t length = d_width * d_height;
   fb = open("/dev/fb0");

   addr = mmap(0x460, length, PROT_READ,
               MAP_PRIVATE, fb, 0);

   s = write(STDOUT_FILENO, addr + offset - pa_offset, length);

   munmap(addr, length + offset - pa_offset);
}
