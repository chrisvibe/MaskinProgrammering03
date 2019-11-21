#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

void sigio_handler(int signo);

FILE *device;


int main(int argc, char *argv[])
{
	printf("Running signal handler test\n");

  device = fopen("/dev/Gamepad", "rb");
  if (!device) {
      printf("Unable to open driver device, maybe you didn't load the module?\n");
  }

  if (signal(SIGIO, &sigio_handler) == SIG_ERR) {
      printf("An error occurred while register a signal handler.\n");
  }
  if (fcntl(fileno(device), F_SETOWN, getpid()) == -1) {
      printf("Error setting pid as owner.\n");
  }
  long oflags = fcntl(fileno(device), F_GETFL);
  if (fcntl(fileno(device), F_SETFL, oflags | FASYNC) == -1) {
      printf("Error setting FASYNC flag.\n");
  }

  // Wait for signal. pause() function stops execution until a signal 
  // is received.
  while (1) {
    pause();
  }

  fclose(device);
	exit(EXIT_SUCCESS);
}

void sigio_handler(int signo)
{
  char res = fgetc(device);
  printf("Signal handler res: %d\n", res);
}
