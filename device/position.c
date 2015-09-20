#include "position.h"

void p_set_position(struct Jelly* jelly, struct Position* position)
{
  jelly->position->x = position->x;
  jelly->position->y = position->y;
}


struct Position* p_random_position(void)
{
  struct Position *random_position = malloc(sizeof(struct Position));
  random_position->x = rand() % POSITION_X_MAX;
  random_position->y = rand() % POSITION_Y_MAX;
  return random_position;
}
