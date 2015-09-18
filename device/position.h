#ifndef __POSITION_H
#define __POSITION_H

// standard c headers
#include <stdint.h>

// forward declares
struct Position;

// project specific
#include "jelly.h"

// forward struct declarations
typedef uint16_t position_t;

struct Position {
  position_t x, y;
};


void p_set_position(struct Jelly *jelly, struct Position*);

#endif
