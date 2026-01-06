// [treesource] This defines the Link class, representing the state of network edges.

#pragma once
#include "sim_types.hpp"

class Link
{
public:
  Link(NodeId from_node, NodeId to_node, SimTime latency)
    : from_(from_node), to_(to_node), latency_(latency) {}

  NodeId from() const { return from_; }
  NodeId to() const { return to_; }
  SimTime latency() const { return latency_; }

private:
  NodeId from_;
  NodeId to_;
  SimTime latency_;
};
