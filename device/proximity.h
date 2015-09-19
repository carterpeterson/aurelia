#ifndef __PROXIMITY_H
#define __PROXIMITY_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// project specific headers
#include "position.h"
#include "color.h"
#include "jelly.h"
#include "util.h"

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

void proximity_sensed(struct Jelly *jelly, struct Position *position);
void proximity_lost(struct Jelly *jelly, struct Position *position);

#endif
