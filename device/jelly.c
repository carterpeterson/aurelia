/*
 * jelly.c - Jelly Device initialization code
 */
#include "jelly.h"

/*
 * Reset a jelly to a known default state.
 */
void jelly_reset(struct Jelly *jelly)
{
  // make sure pointers start at null
  jelly->jelly_message_read_head = NULL;
  jelly->jelly_message_write_head = NULL;
  jelly->proximity_locations = NULL;

#ifdef SIMULATED
  jelly->simulator_sleep_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
  jelly->simulator_sleep_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  jelly->sleeping = false;

  // x11 rendering color
  jelly->render_color_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  jelly->render_color = malloc(sizeof(struct RGBColor));
#else
  printf("not simulated\n");
#endif
}

void jelly_sleep(struct Jelly *jelly)
{
#ifdef DEBUG_PRINT
  printf("(%d): child (pos x:%d y:%d) sleep\n", jelly->address, jelly->position->x, jelly->position->y);
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

  jelly_reset(jelly);

  // copy the init values
  jelly->color = init_frame->color;
  jelly->address = init_frame->address;
  jelly->position = init_frame->position;

  for (;;) { // main run loop
    m_process_messages(jelly);
    c_update_color(jelly);
    // send_pending_network_packets
    jelly_sleep(jelly);
  }
  return NULL;
}
