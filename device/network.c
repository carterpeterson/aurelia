#include "network.h"

// forward declares
void n_handle_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port);

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
  while (jelly->network_packet_receive_read != NULL) {
    current_packet = jelly->network_packet_receive_read;
    n_handle_packet(jelly, current_packet->packet, 0);
    jelly->network_packet_receive_read = jelly->network_packet_receive_read->next_node;
    free(current_packet); // free the packet
  }
}

void n_handle_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port)
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
      printf("network packet message enqueued\n");
    }
  } else {
    // packet not for this jelly, forward the packet
    //n_send_packet(jelly, packet);
  }
}

void n_send_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet)
{
  // check the routing table for port
  bool found = false;
  struct JellyRoutingTableEntry *current_entry = jelly->routing_table_head;

  while (current_entry != NULL) {
    if (current_entry->address == packet->dst_addr) {
      found = true;
      break;
    }
    current_entry = current_entry->next_entry;
  }

  // send the packet
  if (found) {
    port_t outbound_port = current_entry->port;
#ifdef SIMULATED //

#else
#endif
  } else { // broadcast the packet

  }
}
