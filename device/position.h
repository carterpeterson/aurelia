#ifndef __POSITION_H
#define __POSITION_H

// standard c headers
#include <stdint.h>

struct Position {
  uint16_t x, y;
};

void set_position(struct Jelly*, struct Position*);

#endif
