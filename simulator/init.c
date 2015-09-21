#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "jelly_manager.h"
#include "input.h"
#include "render.h"

#define SLEEP_TIME 10000 // 100 fps i guess

void test_event(void)
{
  struct JellyEvent* proximity_event = jm_create_event(PROXIMITY);
  proximity_event->dst_addr = 0;
  struct ProximitySensedMessage *proximity_message = malloc(sizeof(struct ProximitySensedMessage));
  proximity_message->type = PROXIMITY_SENSED;
  proximity_message->position = malloc(sizeof(struct Position));
  proximity_message->position->x = jelly_threads[0]->jelly->position->x;
  proximity_message->position->y = jelly_threads[0]->jelly->position->y;
  proximity_event->message = (union JellyMessage*) proximity_message;
  jm_queue_event(proximity_event, true);
}

int main(int argc, char **argv) {
  srand(time(NULL));
  // Initialize the networking
  // n_network_init();

  // Initialize the jellys
  jm_manager_init();

  // Initialize the main window
  r_init_display();
  i_init_input();

  // send the test event
  //test_event();

  for(;;) {
    // lazy sleeping, could actually try to hit fps but i'm lazy
    usleep(SLEEP_TIME);

    // actually do the run loop stuff
    i_process_input();
    r_render();
  }
}
