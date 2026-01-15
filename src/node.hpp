// [treesource] This defines the Node class, agents that react to Events and handle Packets.

#pragma once
#include "sim_types.hpp"
#include <queue>
#include <memory>

struct Packet;
class Simulation; // note we have to forward decl here, include would break stuffs.
class Strategy;

class Node
{
public:
  Node(NodeId id, SimTime rate, std::unique_ptr<Strategy> strat) : nid(id), is_busy(false), send_rate(rate), strategy(std::move(strat)) {}

  NodeId id() const { return nid; }
  bool busy() const { return is_busy; }
  SimTime rate() const { return send_rate; }

  // packet-arrival Event triggers this: Node adds the packet to its queue.
  void receive_packet(PacketId pid, Simulation& sim);

  // send Event triggers this: Node pops the next packet and schedules another send if not empty.
  void send_packet(Simulation& sim);

  void set_strategy(std::unique_ptr<Strategy> strat)
  {
    strategy = std::move(strat);
  }

  // used by other Nodes to access this one's strategy
  Strategy& get_strategy() const { return *strategy; }

private:
  NodeId nid;
  bool is_busy; // equivalent to "do we have a send-Event scheduled"
  SimTime send_rate; // "cooldown" time between sending each Packet.

  std::queue<PacketId> packet_queue;

  std::unique_ptr<Strategy> strategy;

  NodeId choose_next_hop(Packet& p, Simulation& sim) const;

};
