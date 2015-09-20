#include <stdio.h>

#include "jelly_manager.h"
#include "render.h"

void test_event(void)
{
  struct JellyEvent* proximity_event = jm_create_event(PROXIMITY);
  proximity_event->dst_addr = 0;
  struct ProximitySensedMessage *proximity_message = malloc(sizeof(struct ProximitySensedMessage));
  proximity_message->type = PROXIMITY_SENSED;
  proximity_message->position = malloc(sizeof(struct Position));
  proximity_message->position->x = 5;
  proximity_message->position->y = 10;
  proximity_event->message = (union JellyMessage*) proximity_message;
  jm_queue_event(proximity_event, true);
}

int main(int argc, char **argv) {
  // Initialize the networking
  // n_network_init();

  // Initialize the jellys
  jm_manager_init();

  // Initialize the main window
  r_init_display();

  // send the test event
  test_event();

  for(;;) {
    // dont exit cause I might wanna look at stuff
  }
}
