#ifndef __JELLY_H
#define __JELLY_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Forward declarations for cirular dependancy w/ jelly struct
struct Jelly;

// project specific headers
#include "message.h"
#include "position.h"
#include "color.h"
//#include "network_stack/network.h"

struct Jelly {
  struct RGBColor* color;
  bool color_invalid;
  struct Position* position;
  struct JellyMessageListNode *jelly_message_read_head;
  struct JellyMessageListNode *jelly_message_write_head;

#ifdef SIMULATED
  pthread_cond_t simulator_sleep_cond;
  pthread_mutex_t simulator_sleep_mutex;
  pthread_mutex_t interrupts_enabled_mutex;
  bool sleeping;
#else
  // don't need any way to wake up device externally cause interrupts
#endif

};

#ifndef SIMULATED
struct Jelly *global_jelly_ptr; // this is mostly to be used with ISRs when not simulated.
#endif

void *jelly_init(void *);
void jelly_sleep(struct Jelly *);
void jelly_reset(struct Jelly *);

#endif
