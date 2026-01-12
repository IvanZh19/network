// [treesource] This defines the Link class, representing the state of network edges.

#pragma once
#include "sim_types.hpp"
#include "packet.hpp"

class Link
{
public:
  Link(NodeId from_node, NodeId to_node, SimTime propagation_delay, double bandwidth)
    : from_(from_node), to_(to_node), propagation_delay_(propagation_delay),
      bandwidth_(bandwidth), next_free_time_(0.0) {}

  NodeId from() const { return from_; }
  NodeId to() const { return to_; }
  SimTime propagation_delay() const { return propagation_delay_; }
  double bandwidth() const { return bandwidth_; }
  SimTime next_free_time() const { return next_free_time_; }
  bool free_at(SimTime now) const { return now >= next_free_time_; }

  // Nodes must call this function so that Packets finish transmitting across this link
  // at the correct time. Returns time when Packet fully arrives at its target.
  SimTime reserve(Packet& p, SimTime now) {
    SimTime tx_start_time = free_at(now) ? now : next_free_time_;
    SimTime tx_time = p.packet_size / bandwidth_;
    next_free_time_ = tx_start_time + tx_time;
    SimTime arrival_time = tx_start_time + tx_time + propagation_delay_;
    return arrival_time;
  }

private:
  NodeId from_;
  NodeId to_;
  SimTime propagation_delay_;
  double bandwidth_;
  SimTime next_free_time_;
};
