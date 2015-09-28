#include "network.h"

void n_init_network(struct Jelly *jelly)
{

}

void n_packet_received(struct Jelly *jelly, struct JellyNetworkPacket *packet, port_t port)
{
  // first update the routing table
  if (jelly->routing_table_head == NULL) {
    jelly->routing_table_head = malloc(sizeof(struct JellyRoutingTableEntry));
    jelly->routing_table_head->address = packet->src_addr;
    jelly->routing_table_head->port = port;
    jelly->routing_table_head->time_updated = packet->timestamp;
    jelly->routing_table_head->next_entry = NULL;

  } else {
    struct JellyRoutingTableEntry *current_entry = jelly->routing_table_head;
    bool found = false;

    while (current_entry != NULL) {
      if (current_entry->address == packet->src_addr) {
        found = true;
        if ((packet->timestamp - current_entry->time_updated) > TABLE_ENTRY_TTL) { // TODO: deal with over/underflow
          current_entry->time_updated = packet->timestamp;
          current_entry->port = port;
        }
        break;
      }
      current_entry = current_entry->next_entry;
    }

    if (!found) { // Add it to the head of the list
      struct JellyRoutingTableEntry *new_entry = malloc(sizeof(struct JellyRoutingTableEntry));
      new_entry->address = packet->src_addr;
      new_entry->port = port;
      new_entry->time_updated = packet->timestamp;
      new_entry->next_entry = jelly->routing_table_head;
      jelly->routing_table_head = new_entry;
    }
  }

  // check if the packet is intended for this jelly
  if (packet->dst_addr == jelly->address || packet->dst_addr == BROADCAST_ADDRESS) {
    // packet for this jelly, deal with the packet
    m_enqueue_message(jelly, (union JellyMessage*) packet->payload, true);
  } else {
    // packet not for this jelly, forward the packet
    n_send_packet(jelly, packet);
  }

  // free the packet, but not the payload as it should be dealt with by a message handler
  free(packet);
}

void n_send_packet(struct Jelly *jelly, struct JellyNetworkPacket *packet)
{
  // check the routing table for port
  // send the packet / broadcast the packet
}
