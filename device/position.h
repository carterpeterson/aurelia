#ifndef __POSITION_H
#define __POSITION_H

// standard c headers
#include <stdint.h>
#include <stdlib.h>

// forward declares
struct Position;

// project specific
#include "jelly.h"

// position related definitions / declerations
#define POSITION_X_MAX 150 // 150 discrete possible x positions
#define POSITION_Y_MAX 30  // 30 discrete possible y positions

typedef uint8_t position_t;

struct Position {
  position_t x, y;
};


void p_set_position(struct Jelly *jelly, struct Position*);
struct Position* p_random_position(void);

#endif
