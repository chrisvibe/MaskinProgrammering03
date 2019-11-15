#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>

void refresh_display(int fbfd, int x, int y, int width, int height);

void refresh_display(int fbfd, int x, int y, int width, int height) {
    // setup which part of the frame buffer that is to be refreshed
    // for performance reasons, use as small rectangle as possible
    struct fb_copyarea rect;

    rect.dx = x;
    rect.dy = y;
    rect.width = width;
    rect.height = height;

    // command driver to update display
    ioctl(fbfd, 0x4680, &rect);
}

