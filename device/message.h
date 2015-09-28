#ifndef __MESSAGE_H
#define __MESSAGE_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// project specific headers
#include "position.h"
#include "color.h"
#include "jelly.h"
#include "util.h"
#include "proximity.h"

enum JellyMessageType {
  PROXIMITY_SENSED,
  PROXIMITY_LOST,
  SET_POSITION
};

struct ProximitySensedMessage {
  enum JellyMessageType type;
  JellyTime time_sensed;
  struct Position *position;
};

struct ProximityLostMessage {
  enum JellyMessageType type;
  JellyTime time_sensed;
  struct Position *position;
};

struct SetPositionMessage {
  enum JellyMessageType type;
  struct Position *position;
};

union JellyMessage {
  enum JellyMessageType type;
  struct ProximitySensedMessage proximity_sensed_message;
  struct ProximityLostMessage proximity_lost_message;
  struct SetPositionMessage set_position_message;
};

struct JellyMessageListNode {
  union JellyMessage *message;
  struct JellyMessageListNode *next_message;
};

void m_enqueue_message(struct Jelly *jelly, union JellyMessage *message, bool wake);
void m_process_messages(struct Jelly *jelly);
void m_free_message(union JellyMessage *message);

#endif
