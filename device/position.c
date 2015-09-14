#include "position.h"

void set_position(struct Jelly* jelly, struct Position* position)
{
  jelly->position->x = position->x;
  jelly->position->y = position->y;
}
