// [treesource] This defines the interface for the Strategy class and subclasses, used by Nodes to make routing decisions.

#pragma once
#include <unordered_map>
#include <memory.h>
#include "sim_types.hpp"

class Simulation;
struct Packet;

// Base class
// NOTE: the constructor for these ends up being pretty unwieldy and weird to use, should refactor later.

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
  ShortestPathStrategy(NodeId self_id, Simulation& sim)
    : self_id(self_id)
  {
    build_lookup(sim); // this would modify next hop lookup table.
  }
  NodeId choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const override;

private:
  NodeId self_id;
  std::unordered_map<NodeId, NodeId> next_hop_lookup;

  void build_lookup(const Simulation& sim);

};
