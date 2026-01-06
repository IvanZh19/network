// [treesource] This implements the Strategy class and its subclasses.

#include "strategy.hpp"
#include <random>
#include <vector>
#include "sim.hpp"


NodeId RandomNeighborStrategy::choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const
{
  // assume we're not the destination. which is guaranteed when this is called.

  std::vector<Edge>& edges = sim.get_edges(self_id);

  if (edges.empty())
  {
    return self_id; // no neighbors... // TODO: possible bug where self to self does not exist.
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, edges.size()-1);

  size_t index = dist(gen);
  return edges[index].to;

}
