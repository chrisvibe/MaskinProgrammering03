#include <linux/fb.h>

// setup which part of the frame buffer that is to be refreshed
// for performance reasons, use as small rectangle as possible
struct fb copyarea rect;

rect.dx = x;
rect.dy = y;
rect.width = width;
rect.height = height;

// command driver to update display
ioctl(fbfd, 0x460, &rect);
