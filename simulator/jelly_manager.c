/*
 * jelly_manager.c - Manages all the threads for the jellys.
 */
#include "jelly_manager.h"

// defines
#define JM_EVENT_QUEUE_SEM_NAME "jm_event_queue_sem"
#define JM_EVENT_QUEUE_SEM_PERMISSIONS 0600

// jelly threads
struct JellyThread {
  struct Jelly* jelly;
  pthread_t run_thread;
};

// jelly manager thread control
pthread_t jm_main_thread;
sem_t *jm_event_queue_sem;
pthread_mutex_t jm_event_queue_mutex;
bool jm_teardown;

// jelly manager event queue
struct JellyEvent *jm_event_queue_head;

// thread control for all jelly device threads
struct JellyThread *jelly_threads[NUM_JELLYS];

// private forward declarations
void jm_create_jelly_threads(void);
void jm_teardown_jelly_threads(void);
void jm_process_events(void);
void *jm_manager_run_loop(void *);

void jm_manager_init(void)
{
  // For now just init the thread that is responsible for all jelly threads
  printf("--jelly manager init--\n");

  //sem_init(&jm_event_queue_sem, 0, 0);
  jm_event_queue_sem = sem_open(JM_EVENT_QUEUE_SEM_NAME, O_CREAT, JM_EVENT_QUEUE_SEM_PERMISSIONS, 0);
  jm_event_queue_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
  jm_teardown = false;
  jm_event_queue_head = NULL;

  pthread_create(&jm_main_thread, NULL, jm_manager_run_loop, NULL);
}

void *jm_manager_run_loop(void *argument) // idk if i'll need an argument
{
  jm_create_jelly_threads();

  while (jm_teardown == false) {
    printf("jm_thread_going_to_sleep\n");
    sem_wait(jm_event_queue_sem);
    printf("jm_thread_woken_up\n");
    jm_process_events();
  }

  // jelly manager received teardown message
  jm_teardown_jelly_threads();
  return NULL; // no return code atm
}

/*
 * jelly manager event handling
 */
void jm_queue_event(struct JellyEvent *jelly_event, bool wake)
{
  printf("--jm_queue_event--\n");
  pthread_mutex_lock(&jm_event_queue_mutex);

  if (jm_event_queue_head == NULL) {
    jm_event_queue_head = jelly_event;
  } else {
    struct JellyEvent* jm_event_queue_tail = jm_event_queue_head;
    while (jm_event_queue_tail->next_event != NULL)
      jm_event_queue_tail = jm_event_queue_tail->next_event;
    jm_event_queue_tail->next_event = jelly_event;
  }

  pthread_mutex_unlock(&jm_event_queue_mutex);

  if (wake) {
    sem_post(jm_event_queue_sem);
    printf("wake jm_thread\n");
  }
  printf("++jm_queue_event++\n");
}

void jm_process_events(void)
{
  printf("--jm_process_events--\n");
  // We should already have the jm_event_queue lock at this point
  struct JellyEvent *next;

  pthread_mutex_lock(&jm_event_queue_mutex);
  //printf("jm_process_events aquired_lock\n");
  while (jm_event_queue_head != NULL) {
    switch (jm_event_queue_head->type) {
    case(TEARDOWN):
      jm_teardown = true;
      //printf("teardown seen\n");
      break;
    default:
      // do nothing for now
      break;
    }

    // Free the processed event and move onto the next
    next = jm_event_queue_head->next_event;
    free(jm_event_queue_head);
    jm_event_queue_head = next;
  }
  pthread_mutex_unlock(&jm_event_queue_mutex);
}

struct JellyEvent* jm_create_event(enum jelly_event_type type)
{
  printf("--jm_create_event--\n");
  struct JellyEvent* new_jelly_event = (struct JellyEvent*) malloc(sizeof(struct JellyEvent));
  new_jelly_event->type = type;
  new_jelly_event->payload = NULL;
  new_jelly_event->next_event = NULL;
  return new_jelly_event;
}

/*
 *  jelly thread creation/teardown
 */
void jm_create_jelly_threads(void)
{
  //printf("--jelly create threads--\n");
  int i;
  for (i = 0; i < NUM_JELLYS; i++) {
    jelly_threads[i] = (struct JellyThread*) malloc(sizeof(struct JellyThread));
    jelly_threads[i]->jelly = (struct Jelly*) malloc(sizeof(struct Jelly));
    pthread_create(&(jelly_threads[i]->run_thread), NULL, jelly_init, (void *) jelly_threads[i]->jelly);
  }
}

void jm_teardown_jelly_threads(void)
{
  // Do nothing for now, need to incorporate teardown into the jellys before this will work
  printf("jm_teardown_jelly_threads\n");
}
