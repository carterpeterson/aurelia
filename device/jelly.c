/*
 * jelly.c - Jelly Device initialization code
 */

#include "jelly.h"

/*
 * Reset a jelly to a known default state.
 */
void jelly_reset(struct Jelly *jelly)
{
  // reset color
  jelly->color.red = 0;
  jelly->color.green = 0;
  jelly->color.blue = 0;
  jelly->color.alpha = 0;
  jelly->color_invalid = true;

  // reset position
  jelly->position.x = 0;
  jelly->position.y = 0;

#ifdef SIMULATED
  jelly->simulator_sleep_cond = PTHREAD_COND_INITIALIZER;
  jelly->simulator_sleep_mutex = PTHREAD_MUTEX_INITIALIZER;
  jelly->sleeping = false;
#endif
}

void jelly_sleep(struct Jelly *jelly)
{
#ifdef SIMULATED
  // pthread sleep
  pthread_mutex_lock(&(jelly->simulator_sleep_mutex));
  jelly->sleeping = true;
  while (jelly->sleeping == true)
    pthread_cond_wait(&(jelly->simulator_sleep_cond) ,&(jelly->simulator_sleep_mutex));
  pthread_mutex_unlock(&(jelly->simulator_sleep_mutex));
#else
  // MSP430 sleep
#endif
}

/*
 * Initializes the jelly and starts the main run loop for the jelly device.
 *
 * NOTE: This function should not exit, if it exited there is likely an error.
 */
int jelly_init(struct Jelly *jelly)
{
  jelly_reset(jelly);
  for (;;) { // main run loop
    // Check reason for wake
    // Process work
    jelly_sleep();
  }
}