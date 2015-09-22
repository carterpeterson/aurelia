#include "input.h"

// mouse tracking variables
struct Mouse *mouse_pos;
XEvent x_event;

void i_dispatch_proximity_events(void);

void i_init_input(void)
{
  mouse_pos = malloc(sizeof(struct Mouse));
  mouse_pos->pressed = false;
}

void i_process_input(void)
{
  bool update = false;

  while(XPending(x11_display)) {
    update = true;

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

  if (update)
    i_dispatch_proximity_events();
}

bool i_jelly_in_proximity_window(struct Jelly* jelly)
{
  if (jelly->position->x >= (mouse_pos->x_pos - (PROXIMITY_WINDOW_WIDTH / 2)) &&
      jelly->position->x <= (mouse_pos->x_pos + (PROXIMITY_WINDOW_WIDTH / 2)) &&
      jelly->position->y >= (mouse_pos->y_pos - (PROXIMITY_WINDOW_HEIGHT / 2)) &&
      jelly->position->y <= (mouse_pos->y_pos + (PROXIMITY_WINDOW_HEIGHT / 2)) )
    return true;

  return false;
}

void i_dispatch_proximity_events(void)
{
  int i;

  if (mouse_pos->pressed) {
    for (i = 0; i < NUM_JELLYS; i++) {
      if (i_jelly_in_proximity_window(jelly_threads[i]->jelly)) {
        jelly_threads[i]->jelly->local_proximity_sensed = true;
      } else {
        jelly_threads[i]->jelly->local_proximity_sensed = false;
      }
    }
  } else {
    for (i = 0; i < NUM_JELLYS; i++) {
      jelly_threads[i]->jelly->local_proximity_sensed = false;
    }
  }
}
