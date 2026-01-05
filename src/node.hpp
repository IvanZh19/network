// [treesource] This defines the Node class, agents that react to Events and handle Packets.

#pragma once
#include "sim_types.hpp"

struct Packet;
class Simulation; // note we have to forward decl here, include would break stuffs.

class Node
{
public:
  Node(NodeId id) : nid(id) {}

  NodeId id() const { return nid; }

  void receive_packet(PacketId pid, Simulation& sim);
  // typically an Event triggers this,
  // and the Node creates+schedules a new Event with the receive_packet of the next Node.

private:
  NodeId nid;

  NodeId choose_next_hop(const Packet& p, Simulation& sim) const;
};
