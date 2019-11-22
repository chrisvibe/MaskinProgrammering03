#include "button_driver.h"

static FILE * device;
int resultFromDeviceDriver;

void init_gamepad()
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
 
  pause();
}

void sigio_handler(int signo)
{
  resultFromDeviceDriver = fgetc(device);
  printf("Signal handler res: %d\n", resultFromDeviceDriver);
}

void shutdown_driver()
{
    fclose(device);
}