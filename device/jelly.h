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
#include "position.h"
#include "color.h"

struct Jelly {
  struct RGBColor* color;
  bool color_invalid;
  struct Position* position;

#ifdef SIMULATED
  pthread_cond_t simulator_sleep_cond;
  pthread_mutex_t simulator_sleep_mutex;
  bool sleeping;
#else
  // don't need any way to wake up device externally cause interrupts
#endif

};

int jelly_init(struct Jelly *jelly);
void jelly_sleep(struct Jelly *jelly);
void jelly_reset(struct Jelly *jelly);

#endif
