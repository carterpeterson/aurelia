#include "input.h"

// mouse tracking variables
struct Mouse *mouse_pos;
XEvent x_event;

void i_init_input(void)
{
  mouse_pos = malloc(sizeof(struct Mouse));
  mouse_pos->pressed = false;
}

void i_process_input(void)
{
  while(XPending(x11_display)) {
    XNextEvent(x11_display, &x_event); // Blocks if no touch event
    if (x_event.type == ButtonPress) {
      mouse_pos->pressed = true;
      mouse_pos->x_pos = x_event.xbutton.x;
      mouse_pos->y_pos = x_event.xbutton.y;

    } else if(x_event.type == ButtonRelease) {
      mouse_pos->pressed = false;

    } else if(x_event.type == MotionNotify &&
              x_event.xmotion.x < SIMULATOR_WINDOW_WIDTH && x_event.xmotion.y < SIMULATOR_WINDOW_HEIGHT) {

      mouse_pos->x_pos = x_event.xbutton.x;
      mouse_pos->y_pos = x_event.xbutton.y;
    }
  }
}
