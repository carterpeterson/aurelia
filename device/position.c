#include "position.h"

void set_position(struct Jelly* jelly, struct Position* position)
{
  jelly->posiiton.x = position.x;
  jelly->position.y = position.y;
}
