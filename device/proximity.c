#include "proximity.h"

// Simulator proximity ISR alerting thread variables
#ifdef SIMULATED
#define PROXIMITY_SAMPLE_ISR_SLEEP_TIME 100000 // 10 Hz

struct Jelly *current_proximity_isr_jelly;
pthread_t proximity_sample_isr_thread;
void *proximity_sample_isr_run_loop(void *argument);
#endif


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

void proximity_init(void)
{
#ifdef SIMULATED
  pthread_create(&proximity_sample_isr_thread, NULL, proximity_sample_isr_run_loop, NULL);
#endif
}

/***************************************************
 *      local proximity sensing / messaging        *
 ***************************************************/

void proximity_queue_message(struct Jelly *jelly, bool sensed)
{
  // can make it a *SensedMessage for both as they're identical structs
  struct ProximitySensedMessage *proximity_message = malloc(sizeof(struct ProximitySensedMessage));
  proximity_message->type = (sensed == true) ? PROXIMITY_SENSED : PROXIMITY_LOST;
  proximity_message->position = malloc(sizeof(struct Position));
  proximity_message->position->x = jelly->position->x;
  proximity_message->position->y = jelly->position->y;
  m_enqueue_message(jelly, (union JellyMessage*) proximity_message, true);
}

void proximity_sample_isr(void)
{
  struct Jelly* jelly;
  bool current_proximity_sample;

#ifdef SIMULATED
  jelly = current_proximity_isr_jelly;
  current_proximity_sample = jelly->local_proximity_sensed;
  pthread_mutex_lock(&(jelly->interrupts_enabled_mutex)); // act like we're actually an ISR
#else
  jelly = global_jelly_ptr; // On device this points to only instance of jelly
  current_proximity_sample = *PROXIMITY_ACTIVE_GPIO_PIN;
#endif

  if (current_proximity_sample) {
    if (!jelly->local_proximity)
      proximity_queue_message(jelly, true);
    jelly->local_proximity = true;
  } else if (!current_proximity_sample) {
    if (jelly->local_proximity)
      proximity_queue_message(jelly, false);
    jelly->local_proximity = false;
  }

#ifdef SIMULATED
  pthread_mutex_unlock(&(jelly->interrupts_enabled_mutex));
#endif
}

#ifdef SIMULATED
void *proximity_sample_isr_run_loop(void *argument)
{
  int i;
  for (;;) {
    usleep(PROXIMITY_SAMPLE_ISR_SLEEP_TIME);

    for (i = 0; i < NUM_JELLYS; i++) {
      current_proximity_isr_jelly = jelly_threads[i]->jelly;
      proximity_sample_isr();
    }
  }
}
#endif

/***************************************************
 *     proximity list maintanance functions        *
 ***************************************************/

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
