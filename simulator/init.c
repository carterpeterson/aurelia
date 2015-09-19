#include <stdio.h>

#include "jelly_manager.h"

int main(int argc, char **argv) {
  // Initialize the main window

  // Initialize the networking

  // Initialize the jellys
  jm_manager_init();

  struct JellyEvent* proximity_event = jm_create_event(PROXIMITY);
  proximity_event->dst_addr = 0;
  struct ProximitySensedMessage *proximity_message = malloc(sizeof(struct ProximitySensedMessage));
  proximity_message->type = PROXIMITY_SENSED;
  proximity_message->position = malloc(sizeof(struct Position));
  proximity_message->position->x = 5;
  proximity_message->position->y = 10;
  proximity_event->message = (union JellyMessage*) proximity_message;
  jm_queue_event(proximity_event, true);

  for(;;) {
    // dont exit cause I might wanna look at stuff
  }
}
