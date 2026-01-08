// [treesource] This defines the interface for the Strategy class and subclasses, used by Nodes to make routing decisions.

#pragma once
#include <unordered_map>
#include <memory.h>
#include "sim_types.hpp"
#include "packet.hpp"

class Simulation;

// pair of neighbor, and total distance from us (not the neighbor) to target (key that gave this RouteInfo)
using RouteInfo = std::pair<NodeId, SimTime>;

// Base class
// NOTE: the constructor for these ends up being pretty unwieldy and weird to use, should refactor later.
// maybe can write a helper function file that just calls this, but just to prettify

class Strategy
{
public:
  virtual ~Strategy() = default;

  // returns the neighbor this strategy wants to send to.
  virtual NodeId choose_next_hop(NodeId self, Packet& p, Simulation& sim) const = 0;

  // returns the RouteInfo (neighbor and total distance) for a destination from ourselves. Distance is an arbitrary function of Link costs.
  virtual RouteInfo get_route_info(NodeId dst) const;
};

class RandomNeighborStrategy : public Strategy
{
public:
  RandomNeighborStrategy(NodeId self_id, Simulation& sim)
    : self_id(self_id) {}
  NodeId choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const override;

  // meaningless for this.
  RouteInfo get_route_info(NodeId dst) const override { return std::make_pair(0, 0.0); }

private:
  NodeId self_id;
};

// Build a look-up table from a given node, where Link scores are a function of their values.
// At self_id, using this Link cost function, to get to key=NodeId=target Node, pick neighbor Node Y. Also provides shortest distance from self_id to target.
std::unordered_map<NodeId, RouteInfo> build_lookup(NodeId self_id, Simulation& sim, double propagation_delay_factor, double bandwidth_factor, double fixed);

class ShortestPathStrategy : public Strategy
{
public:
  // The "cost" of a Link is (propagation_delay_factor * propagation_delay) + (bandwidth_factor / bandwidth) + fixed
  ShortestPathStrategy(NodeId self_id, Simulation& sim, double propagation_delay_factor, double bandwidth_factor, double fixed)
    : self_id(self_id)
  {
    next_hop_lookup = build_lookup(self_id, sim, propagation_delay_factor, bandwidth_factor, fixed); // this would modify next hop lookup table.
  }

  NodeId choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const override;

  RouteInfo get_route_info(NodeId dst) const override;

private:
  NodeId self_id;
  std::unordered_map<NodeId, RouteInfo> next_hop_lookup; // key=dest/target, value=best_neighbor
};

class CongestionAwareStrategy : public Strategy
{
public:
  CongestionAwareStrategy(NodeId self_id, Simulation& sim, double propagation_delay_factor, double bandwidth_factor, double fixed, double congestion_factor)
    : self_id(self_id), congestion_factor(congestion_factor)
  {
    next_hop_lookup = build_lookup(self_id, sim, propagation_delay_factor, bandwidth_factor, fixed);
  }

  // estimates time to reach each neighbor, adds that to neighbor's time to target, chooses the minimum amongst these
  NodeId choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const override;

  RouteInfo get_route_info(NodeId dst) const override;

private:
  NodeId self_id;
  double congestion_factor;
  std::unordered_map<NodeId, RouteInfo> next_hop_lookup;
};
