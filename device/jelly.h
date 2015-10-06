#ifndef __JELLY_H
#define __JELLY_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Forward declarations for cirular dependancy w/ jelly struct
struct Jelly;
struct JellyInitFrame;

// project specific headers
#include "message.h"
#include "position.h"
#include "color.h"
#include "network.h"

struct Jelly {
  bool color_invalid;
  bool local_proximity;
  address_t address;
  struct RGBColor* color;
  struct Position* position;
  struct JellyNetworkPort **network_ports;

  // Living Proximities
  struct ProximityListNode *proximity_locations;

  // Message queues
  struct JellyMessageListNode *jelly_message_read_head;
  struct JellyMessageListNode *jelly_message_write_head;

  // Packet routing lists & queues
  struct JellyRoutingTableEntry *routing_table_head;
  struct JellyNetworkPacketListNode *network_packet_receive_read;
  struct JellyNetworkPacketListNode *network_packet_receive_write;

#ifdef SIMULATED
  // sleep simulation locks
  pthread_cond_t simulator_sleep_cond;
  pthread_mutex_t simulator_sleep_mutex;
  bool sleeping;

  // interupt enabling / disabling simulation
  pthread_mutex_t interrupts_enabled_mutex;

  // x11 render buffer
  struct RGBColor* render_color;
  pthread_mutex_t render_color_mutex;

  // proximity flag
  bool local_proximity_sensed;
#else
  // don't need any way to wake up device externally cause interrupts
#endif
};

#ifndef SIMULATED
struct Jelly *global_jelly_ptr; // this is mostly to be used with ISRs when not simulated.
#endif

struct JellyInitFrame {
  address_t address;
  struct Jelly *jelly;
  struct Position *position;
  struct RGBColor* color;
  struct JellyNetworkPort **network_ports;
};

void *jelly_init(void *jelly_init_frame);
void jelly_sleep(struct Jelly *jelly);
void jelly_reset(struct Jelly *jelly);
void jelly_wake(struct Jelly *jelly);

#endif
