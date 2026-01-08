// [treesource] This defines the interface for the Strategy class and subclasses, used by Nodes to make routing decisions.

#pragma once
#include <unordered_map>
#include <memory.h>
#include "sim_types.hpp"
#include "packet.hpp"

class Simulation;

// Base class
// NOTE: the constructor for these ends up being pretty unwieldy and weird to use, should refactor later.
// maybe can write a helper function file that just calls this, but just to prettify

class Strategy
{
public:
  virtual ~Strategy() = default;

  virtual NodeId choose_next_hop(NodeId self, Packet& p, Simulation& sim) const = 0;
};

class RandomNeighborStrategy : public Strategy
{
public:
  RandomNeighborStrategy(NodeId self_id, Simulation& sim)
    : self_id(self_id) {}
  NodeId choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const override;

private:
  NodeId self_id;
};


class ShortestPathStrategy : public Strategy
{
public:
  // The "cost" of a Link is (propagation_delay_factor * propagation_delay) + (bandwidth_factor / bandwidth) + fixed
  ShortestPathStrategy(NodeId self_id, Simulation& sim, double propagation_delay_factor, double bandwidth_factor, double fixed)
    : self_id(self_id)
  {
    build_lookup(sim, propagation_delay_factor, bandwidth_factor, fixed); // this would modify next hop lookup table.
  }
  NodeId choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const override;

private:
  NodeId self_id;
  std::unordered_map<NodeId, NodeId> next_hop_lookup; // key=dest/target, value=best_neighbor

  void build_lookup(Simulation& sim, double propagation_delay_factor, double bandwidth_factor, double fixed);

};
