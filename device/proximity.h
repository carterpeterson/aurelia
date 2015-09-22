#ifndef __PROXIMITY_H
#define __PROXIMITY_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// project specific headers
#include "position.h"
#include "color.h"
#include "jelly.h"
#include "util.h"
#include "message.h"

// simulator headers
#ifdef SIMULATED
#include "../simulator/jelly_manager.h"
#endif

// proximity related definitions
#define DEFAULT_TIME 0

typedef uint16_t JellyTime;

struct Proximity {
  struct Position *position;
  JellyTime time_sensed;
};

struct ProximityListNode {
  struct Proximity *proximity;
  struct ProximityListNode *next_node;
};

void proximity_init(void);
void proximity_sample_isr(void);
void proximity_sensed(struct Jelly *jelly, struct Position *position);
void proximity_lost(struct Jelly *jelly, struct Position *position);


#endif
