#include <stdio.h>

#include "jelly_manager.h"

int main(int argc, char **argv) {
  // Initialize the main window

  // Initialize the networking

  // Initialize the jellys
  jm_manager_init();

  struct JellyEvent* teardown_event = jm_create_event(TEARDOWN);
  jm_queue_event(teardown_event, true);

  for(;;) {
    // dont exit cause I might wanna look at stuff
  }
}
