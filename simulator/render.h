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

// Project Specific Headers
#include "./device/color.h"

// Desktop simulation related constants
#define SIMULATOR_WINDOW_WIDTH   1000
#define SIMULATOR_WINDOW_HEIGHT  200
#define JELLY_RADIUS 10

extern Display *x11_display;
extern int width, height, pixel_width, pixel_height;
extern pthread_mutex_t frame_buffer_lock;
extern bool render_flag;

void render(void);
void render_frame_buffer(void);
void init_display(void);
void set_pixel(int i, int j, Pixel p);

#endif
