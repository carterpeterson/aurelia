/*
 * jelly.c - Jelly Device initialization code
 */

#include "jelly.h"

/*
 * Reset a jelly to a known default state.
 */
void jelly_reset(struct Jelly *jelly)
{
  jelly->color->red = 0;
  jelly->color->green = 0;
  jelly->color->blue = 0;
  jelly->color_invalid = true;

  // reset position
  jelly->position->x = 0;
  jelly->position->y = 0;

#ifdef SIMULATED
  jelly->simulator_sleep_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  jelly->simulator_sleep_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  jelly->sleeping = false;
#else
  printf("not simulated\n");
#endif
}

void jelly_sleep(struct Jelly *jelly)
{
#ifdef DEBUG_PRINT
    printf("child sleep\n");
#endif

#ifdef SIMULATED
  // pthread sleep
  pthread_mutex_lock(&(jelly->simulator_sleep_mutex));
  jelly->sleeping = true;
  while (jelly->sleeping == true) {
    pthread_cond_wait(&(jelly->simulator_sleep_cond) ,&(jelly->simulator_sleep_mutex));
  }
  pthread_mutex_unlock(&(jelly->simulator_sleep_mutex));
#else
  // MSP430 sleep
#endif
}

/*
 *  Do all the mallocs for a jelly
 */
void jelly_malloc(struct Jelly *jelly)
{
  jelly->color = (struct RGBColor*) malloc(sizeof(struct RGBColor));
  jelly->position = (struct Position*) malloc(sizeof(struct Position));
}

/*
 * Initializes the jelly and starts the main run loop for the jelly device.
 *
 * NOTE: This function should not exit, if it exited there is likely an error.
 */
void *jelly_init(void *jelly_ptr)
{
  struct Jelly* jelly = (struct Jelly*) jelly_ptr;
  jelly_malloc(jelly);
  jelly_reset(jelly);
  for (;;) { // main run loop
    // Check reason for wake
    // Process work
    m_process_messages(jelly);
    jelly_sleep(jelly);
  }
  return NULL;
}
