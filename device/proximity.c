#include "proximity.h"

// some forward declarations
void add_proximity_to_list(struct Jelly *jelly, struct Position *position);
void remove_proximity_from_list(struct Jelly *jelly, struct Position *position);
void free_proximity_list_node(struct ProximityListNode *node);

void proximity_sensed(struct Jelly *jelly, struct Position *position)
{
#ifdef DEBUG_PRINT
  printf("jelly: %x, position: %x\n", (unsigned int) jelly, (unsigned int) position);
  printf("(%d) proximity sensed: %d, %d\n", jelly->address, position->x, position->y);
#endif
  add_proximity_to_list(jelly, position);
}

void proximity_lost(struct Jelly *jelly, struct Position *position)
{
#ifdef DEBUG_PRINT
  printf("(%d) proximity lost: %d, %d\n", jelly->address, position->x, position->y);
#endif
  remove_proximity_from_list(jelly, position);
}

// proximity list maintanance functions
void add_proximity_to_list(struct Jelly *jelly, struct Position *position)
{
  // create proximity object and copy values from position reported
  struct Proximity *new_proximity = malloc(sizeof(struct Proximity));
  new_proximity->position = malloc(sizeof(struct Position));
  new_proximity->position->x = position->x;
  new_proximity->position->y = position->y;
  new_proximity->time_sensed = DEFAULT_TIME;

  // TODO: update proximity if it exists already
  struct ProximityListNode *new_node = malloc(sizeof(struct ProximityListNode));
  new_node->proximity = new_proximity;
  new_node->next_node = NULL;

  if (jelly->proximity_locations == NULL) {
    jelly->proximity_locations = new_node;
  } else {
    struct ProximityListNode *tail = jelly->proximity_locations;
    while (tail->next_node != NULL) // find the end of the list
      tail = tail->next_node;
    tail->next_node = new_node;
  }
}

void remove_proximity_from_list(struct Jelly *jelly, struct Position *position)
{
  if (jelly->proximity_locations == NULL) {
    // do nothing, we don't have any living proximities
  } else {
    struct ProximityListNode *prev_node = NULL;
    struct ProximityListNode *delete_node = NULL;
    struct ProximityListNode *current_node = jelly->proximity_locations;
    while (current_node != NULL) {
      if (current_node->proximity->position->x == position->x &&
          current_node->proximity->position->y == position->y) {
        // matching node, let's remove it from the list
        // fix the pointers
        delete_node = current_node;
        current_node = current_node->next_node;
        if (prev_node != NULL)
          prev_node->next_node = current_node->next_node;

        // free the node, first move head pointer if deleting head
        if(jelly->proximity_locations == delete_node)
          jelly->proximity_locations = delete_node->next_node;
        free_proximity_list_node(delete_node);
      } else { // just continue traversing the list
        prev_node = current_node;
        current_node = current_node->next_node;
      }
    }
  }
}

void free_proximity_list_node(struct ProximityListNode *node)
{
  // free the proximity and then the node itself
  free(node->proximity->position);
  free(node->proximity);
  free(node);
}
