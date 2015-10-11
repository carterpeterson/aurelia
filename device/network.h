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
#include "position.h"

#ifdef SIMULATED
#include "../simulator/jelly_manager.h"
#endif

#define BROADCAST_ADDRESS 0xDEADBEEF // Would go with all F but want edges
 // 30 minute TTL, hopefully if a jelly dies in this window it will send out the death notice
#define TABLE_ENTRY_TTL 1800
#define NUM_NETWORK_PORTS 6
#define SEQUENCE_NUM_START 0
#define PORT_UNKNOWN 0xFF
#define HALF_ADDR_SIZE 16
#define MAX_HOPS 5

struct JellyNetworkPort {
  bool alive;
  address_t port_address;
};

enum JellyNetworkPacketPayloadType {
  PAYLOAD_PROXIMITY_SENSED,
  PAYLOAD_PROXIMITY_LOST,
  PAYLOAD_SET_POSITION,
  PAYLOAD_HEARTBEAT,
  PAYLOAD_DEATH_NOTICE
};

struct JellyNetworkProximitySensedPayload {
  enum JellyNetworkPacketPayloadType type;
  position_t x_pos, y_pos;
};

struct JellyNetworkProximityLostPayload {
  enum JellyNetworkPacketPayloadType type;
  position_t x_pos, y_pos;
};

union JellyNetworkPacketPayload {
  enum JellyNetworkPacketPayloadType type;
  struct JellyNetworkProximitySensedPayload proximity_sensed_payload;
  struct JellyNetworkProximityLostPayload proximity_lost_payload;
};

struct JellyNetworkPacket {
  address_t src_addr, dst_addr;
  address_t port_address;
  uint16_t sequence_num;
  JellyTime timestamp;
  uint8_t hops;
  union JellyNetworkPacketPayload payload;
};

struct JellyNetworkPacketListNode {
  struct JellyNetworkPacket *packet;
  struct JellyNetworkPacketListNode *next_node;
};

struct JellyRoutingTableEntry {
  port_t port;
  uint8_t hops;
  uint16_t current_sequence_num;
  address_t address;
  JellyTime time_updated;
  struct JellyRoutingTableEntry *next_entry;
};

void n_init_network(struct Jelly *jelly);
void n_process_packets(struct Jelly *jelly);
void n_enqueue_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet);
void n_send_pending_packets(struct Jelly *jelly);

#ifdef SIMULATED
void n_packet_available_isr(struct Jelly *jelly, struct JellyNetworkPacket *packet);
#else
void n_packet_available_isr(void);
#endif


#endif
