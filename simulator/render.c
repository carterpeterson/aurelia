#include "render.h"

// things needed for x11
Display *x11_display;
int screen;
Window win;
GC gc;

// some forward declares
void r_init_x();
void r_close_x();

int width, height, jelly_radius;
long proximity_window_color;

void r_init_display(void)
{
  r_init_x();
  proximity_window_color = (long) (255 << 16);
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

long r_resolve_rgb_color(struct RGBColor* color)
{
  return (long) (color->red << 16) + (color->green << 8) + color->blue;
}

void r_render()
{
  int i = 0;
  long render_color;
  int x_pos, y_pos;

  // clear the window
  XClearWindow(x11_display, win);

  // draw the jellys
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

  // draw the proximity window (centered on the mouse)
  if (mouse_pos->pressed) {
    x_pos = mouse_pos->x_pos - (PROXIMITY_WINDOW_WIDTH / 2);
    y_pos = mouse_pos->y_pos - (PROXIMITY_WINDOW_HEIGHT / 2);

    XSetForeground(x11_display, gc, proximity_window_color);
    XDrawRectangle(x11_display, win, gc, x_pos, y_pos, PROXIMITY_WINDOW_WIDTH, PROXIMITY_WINDOW_HEIGHT);
  }

  XFlush(x11_display);
}
