// [treesource] This defines the Node class, agents that react to Events and handle Packets.

#pragma once
#include "sim_types.hpp"
#include "output_port.hpp"
#include "unordered_map"
#include <memory>

struct Packet;
class Simulation; // note we have to forward decl here, include would break stuffs.
class Strategy;

class Node
{
public:
  Node(NodeId id, std::unique_ptr<Strategy> strat) : nid(id), strategy(std::move(strat)) {}

  NodeId id() const { return nid; }

  // packet-arrival Event triggers this: Node routes to correct
  // OutputPort, which handles queueing and draining
  void receive_packet(PacketId pid, Simulation& sim);

  // represents packets originating here
  void originate_packet(PacketId pid, Simulation& sim);

  // has to be called once per outgoing link before sim runs
  void add_port(NodeId neighbor, size_t capacity = 64);

  void set_strategy(std::unique_ptr<Strategy> strat)
  {
    strategy = std::move(strat);
  }

  // used by other Nodes to access this one's strategy
  Strategy& get_strategy() const { return *strategy; }

private:
  NodeId nid;

  // one OutputPort per outgoing link, keyed by NodeId of neighbor
  // do not insert ports after sim starts as this may mess with map
  std::unordered_map<NodeId, OutputPort> ports;

  std::unique_ptr<Strategy> strategy;

  NodeId choose_next_hop(Packet& p, Simulation& sim) const;

};
