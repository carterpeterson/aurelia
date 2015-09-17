#ifndef __JELLY_MANAGER_H
#define __JELLY_MANAGER_H

// standard c headers
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <semaphore.h>

// project specific
#include "../device/jelly.h"

#define NUM_JELLYS 50

enum jelly_event_type {
  PROXIMITY,
  NETWORK_MESSAGE,
  TIMER,
  TEARDOWN
};

struct JellyEvent {
  enum jelly_event_type type;
  void *payload;
  struct JellyEvent* next_event;
};

// jelly manager event queue
extern struct JellyEvent *jm_event_queue_head;
extern pthread_mutex_t jelly_event_queue_mutex;

void jm_manager_init(void);
void jm_queue_event(struct JellyEvent *, bool);
struct JellyEvent* jm_create_event(enum jelly_event_type);

#endif
