// [treesource] This implements the Strategy class and its subclasses.

#include "strategy.hpp"
#include <random>
#include <vector>
#include "sim.hpp"


NodeId RandomNeighborStrategy::choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const
{
  // assume we're not the destination. which is guaranteed when this is called.

  std::vector<Link>& links = sim.get_links(self_id);

  if (links.empty())
  {
    return self_id; // no neighbors... // TODO: possible bug where self to self does not exist.
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, links.size()-1);

  size_t index = dist(gen);
  return links[index].to();

}

// NodeId ShortestPathStrategy::choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const
// {
//   return next_hop_lookup[p.dst];
// }
