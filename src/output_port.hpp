// [treesource] This defines the OutputPort class, representing a single outgoing port on a Node.

#pragma once
#include "sim_types.hpp"
#include <queue>

class Simulation;

class OutputPort
{
public:
  OutputPort(NodeId owner, NodeId neighbor)
    : owner_(owner), neighbor_(neighbor), busy_(false) {}

  NodeId neighbor() const { return neighbor_; }
  bool busy() const { return busy_; }
  int queue_depth() const { return packet_queue_.size(); }

  // called by Node::receive_packet after routing decision is made,
  // enqueues the packet and starts draining if idle
  void enqueue_and_drain(PacketId pid, Simulation& sim);

private:
  NodeId owner_;
  NodeId neighbor_;
  bool busy_;
  std::queue<PacketId> packet_queue_;

  // pops the front packet, schedules its arrival event, then schedules
  // next drain if queue not empty, sets busy_ as needed
  void do_send(Simulation& sim);
};
