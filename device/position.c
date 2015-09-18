#include "position.h"

void p_set_position(struct Jelly* jelly, struct Position* position)
{
  jelly->position->x = position->x;
  jelly->position->y = position->y;
}
