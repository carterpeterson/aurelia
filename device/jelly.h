#ifndef __JELLY_H
#define __JELLY_H

// standard c headers
#include <stdbool.h>
#include <pthread.h>

// project specific headers
#include "position.h"
#include "color.h"

#ifdef SIMULATED
#include "../jelly_manager.h"
#endif

struct Jelly {
  struct RGBColor color;
  bool color_invalid;
  struct Position position;

#ifdef SIMULATED
  pthread_cond_t simulator_sleep_cond;
  pthread_mutex_t simulator_sleep_mutex;
  bool sleeping;
#else
  // don't need any way to wake up device externally cause interrupts
#endif
};

#endif
