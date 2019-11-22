#include <signal.h>
#include <unistd.h>
#include <math.h>

void init_gamepad();
void sigio_handler(int signo);