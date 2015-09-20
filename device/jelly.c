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
  jelly->color->red = 0;
  jelly->color->green = 0;
  jelly->color->blue = 0;
  jelly->color_invalid = true;

  // reset position
  jelly->position->x = 0;
  jelly->position->y = 0;

  // make sure pointers start at null
  jelly->jelly_message_read_head = NULL;
  jelly->jelly_message_write_head = NULL;
  jelly->proximity_locations = NULL;

#ifdef SIMULATED
  jelly->simulator_sleep_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  jelly->simulator_sleep_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  jelly->sleeping = false;

  jelly->render_color_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
#else
  printf("not simulated\n");
#endif
}

void jelly_sleep(struct Jelly *jelly)
{
#ifdef DEBUG_PRINT
  printf("child (addr: %d) sleep\n", jelly->address);
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

void jelly_wake(struct Jelly *jelly)
{
  pthread_mutex_lock(&(jelly->simulator_sleep_mutex));
  jelly->sleeping = false;
  pthread_cond_signal(&(jelly->simulator_sleep_cond));
  pthread_mutex_unlock(&(jelly->simulator_sleep_mutex));
}

/*
 * Initializes the jelly and starts the main run loop for the jelly device.
 *
 * NOTE: This function should not exit, if it exited there is likely an error.
 */
void *jelly_init(void *jelly_init_frame)
{
  struct JellyInitFrame *init_frame =  (struct JellyInitFrame *) jelly_init_frame;
  struct Jelly* jelly = init_frame->jelly;

  // copy the init values
  jelly->color = init_frame->color;
  jelly->address = init_frame->address;
  jelly->position = init_frame->position;

#ifdef SIMULATED // malloc the render buffer if we're simulated
  jelly->render_color = malloc(sizeof(struct RGBColor));
#endif

  jelly_reset(jelly);
  for (;;) { // main run loop
    m_process_messages(jelly);
    // update color
    // send_pending_network_packets
    jelly_sleep(jelly);
  }
  return NULL;
}
