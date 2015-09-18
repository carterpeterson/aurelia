#ifndef __UTIL_H
#define __UTIL_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// project specific headers
#include "jelly.h"

void u_disable_interrupts(struct Jelly* jelly);
void u_enable_interrupts(struct Jelly* jelly);
void u_util_init(struct Jelly* jelly);

#endif
