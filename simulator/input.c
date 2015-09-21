#include "input.h"

// mouse tracking variables
struct Mouse *mouse_pos;
XEvent x_event;

void i_send_proximity_event(address_t jelly_address, bool sensed);
void i_dispatch_proximity_events(void);

bool proximity_active_jellys[NUM_JELLYS];

void i_init_input(void)
{
  int i;
  mouse_pos = malloc(sizeof(struct Mouse));
  mouse_pos->pressed = false;

  for (i = 0; i < NUM_JELLYS; i++) {
    proximity_active_jellys[i] = false;
  }
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

// proximity event dispatch logic

void i_send_proximity_event(address_t jelly_address, bool sensed)
{
  struct JellyEvent* proximity_event = jm_create_event(PROXIMITY);
  proximity_event->dst_addr = jelly_address;
  struct ProximitySensedMessage *proximity_message = malloc(sizeof(struct ProximitySensedMessage));
  proximity_message->type = (sensed == true) ? PROXIMITY_SENSED : PROXIMITY_LOST;
  proximity_message->position = malloc(sizeof(struct Position));
  proximity_message->position->x = jelly_threads[jelly_address]->jelly->position->x;
  proximity_message->position->y = jelly_threads[jelly_address]->jelly->position->y;
  proximity_event->message = (union JellyMessage*) proximity_message;
  jm_queue_event(proximity_event, false);
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
        if (!proximity_active_jellys[i])
          i_send_proximity_event(i, true);
        proximity_active_jellys[i] = true;
      } else {
        if (proximity_active_jellys[i])
          i_send_proximity_event(i, false);
        proximity_active_jellys[i] = false;
      }
    }
  } else {
    for (i = 0; i < NUM_JELLYS; i++) {
      if (proximity_active_jellys[i])
        i_send_proximity_event(i, false);
      proximity_active_jellys[i] = false;
    }
  }

  jm_queue_notify();
}
