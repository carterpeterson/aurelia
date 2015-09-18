#include "util.h"

void u_util_init(struct Jelly* jelly)
{
#ifdef SIMULATED
  jelly->interrupts_enabled_mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
#endif
}

void u_disable_interrupts(struct Jelly* jelly)
{
#ifdef SIMULATED
  pthread_mutex_lock(&(jelly->interrupts_enabled_mutex));
#else
  // disable the interrupts on the msp430
#endif
}

void u_enable_interrupts(struct Jelly* jelly)
{
#ifdef SIMULATED
  pthread_mutex_unlock(&(jelly->interrupts_enabled_mutex));
#else
  // enable the interrupts on the msp430
#endif
}
