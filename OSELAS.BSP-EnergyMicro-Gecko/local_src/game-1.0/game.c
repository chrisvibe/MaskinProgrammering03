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
	printf("Hello World, I'm game!, this is updated again\n");
  printf("DOES THIS SHOW UP\n");
  device = fopen("/dev/Gamepad", "rb"); // Maybe without b?
  if (!device) {
      printf("Unable to open driver device, maybe you didn't load the module?\n");
  }
  /* char str; */
  /* while (fgets(&str, 100, device) != NULL) { */
  /*     printf("Gamepad device returned: %d\n", str); */
  /* } */

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

  while (1) {
    pause();
    printf("File position: %ld\n", ftell(device));
    fseek(device, 0, SEEK_SET);
    char str2 = fgetc(device);
    printf("File position after reed: %ld\n", ftell(device));
    printf("Res str after while loop with fgetc not in handler: %d\n", str2);
    fseek(device, 0, SEEK_SET);
    rewind(device);
  }


  fclose(device);

	exit(EXIT_SUCCESS);
}

void sigio_handler(int signo)
{
  printf("In signal handler\n");
    /* printf("Signal nr.: %d\n", signo); */
    /* fseek(device, 0, SEEK_SET); */
    /* /1* char str; *1/ */
    /* /1* while (fgets(&str, 1, device) != NULL) { *1/ */
    /* /1*     printf("Gamepad device returned: %d\n", str); *1/ */
    /* /1* } *1/ */
    /* /1* printf("Res str after while loop: %d\n", str); *1/ */
    /* fseek(device, 0, SEEK_SET); */
    /* char str2 = fgetc(device); */
    /* printf("Res str after while loop with fgetc: %d\n", str2); */
    /* fseek(device, 0, SEEK_SET); */
    /* rewind(device); */
}
