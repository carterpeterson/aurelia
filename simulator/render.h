#ifndef __RENDER_H__
#define __RENDER_H__

// X library headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

// Normal C Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

// simulator headers
#include "jelly_manager.h"
#include "input.h"

// device headers
#include "../device/color.h"

// Desktop simulation related constants
#define SIMULATOR_WINDOW_WIDTH   1000
#define SIMULATOR_WINDOW_HEIGHT  200
#define JELLY_RADIUS 10
#define JELLY_ARC_START_ANGLE 0
#define JELLY_ARC_END_ANGLE 360

extern Display *x11_display;

void r_render(void);
void r_init_display(void);
#endif
