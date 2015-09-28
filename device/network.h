#ifndef __NETWORK_H
#define __NETWORK_H

// standard c headers
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef uint32_t address_t; // Address is 4 bytes long
typedef uint8_t port_t; // Index into the network port arrays

// project specific headers
#include "jelly.h"
#include "util.h"
#include "proximity.h"
#include "message.h"

#define BROADCAST_ADDRESS 0xDEADBEEF // Would go with all F but want edges
 // 30 minute TTL, hopefully if a jelly dies in this window it will send out the death notice
#define TABLE_ENTRY_TTL 1800
#define NUM_NETWORK_PORTS 6

struct JellyNetworkPort {
  address_t port_address;
};

struct JellyNetworkPacket {
  address_t src_addr, dst_addr;
  JellyTime timestamp;
  void *payload;
};

struct JellyRoutingTableEntry {
  address_t address;
  port_t port;
  JellyTime time_updated;
  struct JellyRoutingTableEntry *next_entry;
};

void n_packet_received(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port);
void n_send_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet);
void n_init_network(struct Jelly *jelly);

#endif
