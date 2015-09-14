/*
 * jelly_manager.c - Manages all the threads for the jellys.
 */
#include "jelly_manager.h"

int main(void)
{
  struct Jelly jelly;
  printf("start\n");
  jelly_init(&jelly);
  return 0;
}
