// [treesource] This implements the Strategy class and its subclasses.

#include "strategy.hpp"
#include <random>
#include <vector>
#include "sim.hpp"
#include <iostream>


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

void ShortestPathStrategy::build_lookup(Simulation& sim)
{
  using pq_element = std::pair<SimTime, NodeId>;
  std::priority_queue<pq_element, std::vector<pq_element>, std::greater<>> pq;

  std::unordered_map<NodeId, SimTime> dist;
  std::unordered_map<NodeId, NodeId> first_hop;

  for (const auto& node_id : sim.get_nodes())
  {
    dist[node_id] = std::numeric_limits<SimTime>::infinity();
  }

  dist[self_id] = 0.0;
  pq.push({0.0, self_id});

  for (const Link& l : sim.get_links(self_id))
  {
    first_hop[l.to()] = l.to();
  }

  while (!pq.empty())
  {
    pq_element e = pq.top();
    SimTime cur_dist = e.first;
    NodeId u = e.second;
    pq.pop();

    if (cur_dist > dist[u]) continue;

    for (const Link& l : sim.get_links(u))
    {
      NodeId v = l.to();
      SimTime cost = l.propagation_delay();
      SimTime new_dist = cur_dist + cost;

      if (new_dist < dist[v])
      {
        dist[v] = new_dist;

        // update first hop
        first_hop[v] = (u == self_id) ? v : first_hop[u];
        pq.push({new_dist, v});
      }
    }


  }

  next_hop_lookup = std::move(first_hop);

}

NodeId ShortestPathStrategy::choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const
{
  auto it = next_hop_lookup.find(p.dst);
  if (it != next_hop_lookup.end())
  {
    return it->second;
  }
  else
  {
    // not reachable from us... just return self_id for now
    std::cout << "Node has no route to destination. Returning self" << std::endl;
    return self_id;
  }
}
