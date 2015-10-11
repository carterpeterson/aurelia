#include "network.h"

// relevant internal definitions
#define PACKET_CONSUMED 0
#define PACKET_FORWARDED 1

// forward declares
uint8_t n_handle_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port);
void n_enqueue_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet);

void n_init_network(struct Jelly *jelly)
{
  // Do nothing for now i guess, will setup drivers on device
}

#ifdef SIMULATED
void n_packet_available_isr(struct Jelly *jelly, struct JellyNetworkPacket *packet)
#else
void n_packet_available_isr(void)
#endif
{
#ifdef SIMULATED
  pthread_mutex_lock(&(jelly->interrupts_enabled_mutex)); // act like we're actually an ISR
#else
  struct Jelly *jelly = global_jelly_ptr; // On device this points to only instance of jelly
  struct JellyNetworkPacket *packet = n_retreive_packet();
#endif

  struct JellyNetworkPacketListNode *new_node = malloc(sizeof(struct JellyNetworkPacketListNode));
  new_node->packet = packet;
  new_node->next_node = jelly->network_packet_receive_write;
  jelly->network_packet_receive_write = new_node;

#ifdef SIMULATED
  pthread_mutex_unlock(&(jelly->interrupts_enabled_mutex));
#endif

  jelly_wake(jelly);
}

void n_process_packets(struct Jelly *jelly)
{
  struct JellyNetworkPacketListNode *temp, *current_packet;

  u_disable_interrupts(jelly); // critical section, cant get interrupted here
  temp = jelly->network_packet_receive_read;
  jelly->network_packet_receive_read = jelly->network_packet_receive_write;
  jelly->network_packet_receive_write = temp;
  u_enable_interrupts(jelly);

  // process the messages
  uint8_t return_code;
  while (jelly->network_packet_receive_read != NULL) {
    current_packet = jelly->network_packet_receive_read;
    return_code = n_handle_packet(jelly, current_packet->packet, 0);
    jelly->network_packet_receive_read = jelly->network_packet_receive_read->next_node;
    if (return_code == PACKET_CONSUMED)
      free(current_packet->packet);
    free(current_packet); // free the packet list node wrapper
  }
}

uint8_t n_handle_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port)
{
  // first update the routing table
  if (jelly->routing_table_head == NULL) {
    jelly->routing_table_head = malloc(sizeof(struct JellyRoutingTableEntry));
    jelly->routing_table_head->port = port;
    jelly->routing_table_head->hops = packet->hops;
    jelly->routing_table_head->address = packet->src_addr;
    jelly->routing_table_head->current_sequence_num = SEQUENCE_NUM_START;
    jelly->routing_table_head->time_updated = packet->timestamp;
    jelly->routing_table_head->next_entry = NULL;

  } else {
    struct JellyRoutingTableEntry *current_entry = jelly->routing_table_head;
    bool found = false;

    while (current_entry != NULL) {
      if (current_entry->address == packet->src_addr) {
        found = true;
        if ((packet->hops < current_entry->hops) ||
            (packet->timestamp - current_entry->time_updated) > TABLE_ENTRY_TTL) { // TODO: deal with over/underflow
          current_entry->time_updated = packet->timestamp;
          current_entry->hops = packet->hops;
          current_entry->port = port;
        }
        break;
      }
      current_entry = current_entry->next_entry;
    }

    if (!found) { // Add it to the head of the list
      struct JellyRoutingTableEntry *new_entry = malloc(sizeof(struct JellyRoutingTableEntry));
      new_entry->port = port;
      new_entry->hops = packet->hops;
      new_entry->address = packet->src_addr;
      new_entry->current_sequence_num = SEQUENCE_NUM_START;
      new_entry->time_updated = packet->timestamp;
      new_entry->next_entry = jelly->routing_table_head;
      jelly->routing_table_head = new_entry;
    }
  }

  // check if the packet is intended for this jelly
  if (packet->dst_addr == jelly->address || packet->dst_addr == BROADCAST_ADDRESS) {
    // packet for this jelly, deal with the packet
    bool valid = false;
    union JellyMessage *new_message;

    switch (packet->payload.type) {
    case (PAYLOAD_PROXIMITY_SENSED):
    case (PAYLOAD_PROXIMITY_LOST):
      valid = true;
      new_message = malloc(sizeof(union JellyMessage));
      ((struct ProximitySensedMessage *) new_message)->type =
        (packet->payload.type == PAYLOAD_PROXIMITY_SENSED) ? PROXIMITY_SENSED : PROXIMITY_LOST;
      ((struct ProximitySensedMessage *) new_message)->time_sensed = packet->timestamp;
      ((struct ProximitySensedMessage *) new_message)->position = malloc(sizeof(struct Position));
      ((struct ProximitySensedMessage *) new_message)->position->x =
        ((struct JellyNetworkProximitySensedPayload*) &packet->payload)->x_pos;
      ((struct ProximitySensedMessage *) new_message)->position->y =
        ((struct JellyNetworkProximitySensedPayload*) &packet->payload)->y_pos;
      break;
    default:
      // Do nothing for now. TODO: maybe handle error?
      break;
    }

    if (valid) {
      m_enqueue_message(jelly, new_message, false); // jelly is already awake
      if (packet->dst_addr == BROADCAST_ADDRESS) {
        packet->hops++;
        if (packet->hops > MAX_HOPS || packet->src_addr == jelly->address) {
          return PACKET_CONSUMED;
        }
        n_enqueue_packet(jelly, packet);
        return PACKET_FORWARDED;
      }
    }
  } else {
    // packet not for this jelly, forward the packet
    packet->hops++;
    if (packet->hops > MAX_HOPS) {
      return PACKET_CONSUMED;
    }
    n_enqueue_packet(jelly, packet);
    return PACKET_FORWARDED;
  }
  return PACKET_CONSUMED;
}

void n_enqueue_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet)
{
  struct JellyNetworkPacketListNode *new_node = malloc(sizeof(struct JellyNetworkPacketListNode));
  new_node->packet = packet;
  new_node->next_node = jelly->network_packet_send;
  jelly->network_packet_send = new_node;
}

port_t n_port_for_address(struct Jelly *jelly, address_t address)
{
  bool found = false;
  struct JellyRoutingTableEntry *current_entry = jelly->routing_table_head;

  while (current_entry != NULL) {
    if (current_entry->address == address) {
      found = true;
      break;
    }
    current_entry = current_entry->next_entry;
  }

  if (found)
    return current_entry->port;
  return PORT_UNKNOWN;
}

void n_transmit_packet_on_port(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port)
{
  if (jelly->network_ports[port]->alive == false)
    return; // don't try to send out a dead port it'll just lock up the radio

  printf("(%d) Send Packet on port:%d address:%x\n", jelly->address, port, jelly->network_ports[port]->port_address);
  packet->port_address = jelly->network_ports[port]->port_address;

#ifdef SIMULATED
  // copy the packet, simulate the idea of not a unified address space
  struct JellyNetworkPacket *packet_copy = malloc(sizeof(struct JellyNetworkPacket));
  memcpy(packet_copy, packet, sizeof(struct JellyNetworkPacket));
  address_t port_addr_0, port_addr_1;
  port_addr_0 = packet->port_address >> HALF_ADDR_SIZE;
  port_addr_1 = packet->port_address & 0x0000FFFF;
  if (jelly->address == port_addr_0)
    n_packet_available_isr(jelly_threads[port_addr_1]->jelly, packet_copy);
  else
    n_packet_available_isr(jelly_threads[port_addr_0]->jelly, packet_copy);
#else

#endif
}

void n_broadcast_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet)
{
  int i = 0;
  for (; i < NUM_NETWORK_PORTS; i++) {
    if (jelly->network_ports[i]->alive && packet->port_address != jelly->network_ports[i]->port_address) {
      n_transmit_packet_on_port(jelly, packet, i);
    }
  }
}

void n_send_pending_packets(struct Jelly *jelly)
{
  struct JellyNetworkPacketListNode *current_packet_node;
  port_t port;

  // send the packets!
  while (jelly->network_packet_send != NULL) {
    current_packet_node = jelly->network_packet_send;

    // Determine the port / if we should broadcast
    port = PORT_UNKNOWN;
    if (current_packet_node->packet->dst_addr != BROADCAST_ADDRESS)
      port = n_port_for_address(jelly, current_packet_node->packet->dst_addr);

    // send or broadcast the packet
    if (port == PORT_UNKNOWN)
      n_broadcast_packet(jelly, current_packet_node->packet);
    else
      n_transmit_packet_on_port(jelly, current_packet_node->packet, port);

    // free the resources
    jelly->network_packet_send = jelly->network_packet_send->next_node;
    free(current_packet_node->packet); // free the packet
    free(current_packet_node);
  }
}
