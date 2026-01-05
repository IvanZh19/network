// [treesource] This defines the Node class, agents that react to Events and handle Packets.

#pragma once
#include "sim_types.hpp"
struct Packet;

class Node
{
public:
  explicit Node(NodeId id)
    : nid(id) {}

  NodeId id() const { return nid; }

  void receive_packet(const Packet &pkt); // to be called by events
  void send_packet(const Packet &pkt, int next_node); // not sure if need for now?

private:
  NodeId nid;
};
