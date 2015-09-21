#include "render.h"

// things needed for x11
Display *x11_display;
int screen;
Window win;
GC gc;

// some forward declares
void r_init_x();
void r_close_x();
void r_redraw();

int width, height, jelly_radius;

void r_init_display(void)
{
  r_init_x();
}

void r_init_x()
{
  unsigned long black,white;

  // setup the display
  x11_display = XOpenDisplay((char *) 0);
  screen = DefaultScreen(x11_display);
  black = BlackPixel(x11_display, screen);
  white = WhitePixel(x11_display, screen);
  win=XCreateSimpleWindow(x11_display, DefaultRootWindow(x11_display), 0, 0,
			  SIMULATOR_WINDOW_WIDTH, SIMULATOR_WINDOW_HEIGHT, 0, black, white);
  XSetStandardProperties(x11_display, win, "aurelia simulator",
			 "aurelia", None, NULL, 0, NULL);
  XSelectInput(x11_display, win, ExposureMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask);
  XAutoRepeatOn(x11_display);
  gc = XCreateGC(x11_display, win, 0, 0);
  XSetBackground(x11_display, gc, white);
  XSetForeground(x11_display, gc, black);

  // enfore the window sizing
  XSizeHints hints;
  hints.flags = PMinSize | PMaxSize;
  hints.max_width = SIMULATOR_WINDOW_WIDTH;
  hints.min_width = SIMULATOR_WINDOW_WIDTH;
  hints.max_height = SIMULATOR_WINDOW_HEIGHT;
  hints.min_height = SIMULATOR_WINDOW_HEIGHT;
  XSetWMNormalHints(x11_display, win, &hints);

  XClearWindow(x11_display, win);
  XMapRaised(x11_display, win);

  // get the window / rendering properties
  XWindowAttributes win_attr;
  XGetWindowAttributes(x11_display, win, &win_attr);

  width = win_attr.width;
  height = win_attr.height;
  jelly_radius = JELLY_RADIUS;
}

void r_close_x()
{
  XFreeGC(x11_display, gc);
  XDestroyWindow(x11_display, win);
  XCloseDisplay(x11_display);
  exit(1);
}

/*
 *  X Window rendering related functions
 */
void r_redraw()
{
  XClearWindow(x11_display, win);
}

long r_resolve_rgb_color(struct RGBColor* color)
{
  return (long) (color->red << 16) + (color->green << 8) + color->blue;
}

void r_render()
{
  int i = 0;
  long render_color;
  int x_pos, y_pos;
  for(; i < NUM_JELLYS; i++) {

    // resolve the color
    pthread_mutex_lock(&(jelly_threads[i]->jelly->render_color_mutex));
    render_color = r_resolve_rgb_color(jelly_threads[i]->jelly->render_color);
    pthread_mutex_unlock(&(jelly_threads[i]->jelly->render_color_mutex));
    XSetForeground(x11_display, gc, render_color);

    // get the position
    x_pos = (jelly_threads[i]->jelly->position->x - JELLY_RADIUS);
    y_pos = (jelly_threads[i]->jelly->position->y - JELLY_RADIUS);

    XFillArc(x11_display, win, gc,  x_pos, y_pos, (JELLY_RADIUS * 2), (JELLY_RADIUS * 2),
             (JELLY_ARC_START_ANGLE * 64), (JELLY_ARC_END_ANGLE * 64));
  }

  XFlush(x11_display);
}
