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

std::unordered_map<NodeId, RouteInfo> build_lookup(NodeId self_id, Simulation& sim, double propagation_delay_factor, double bandwidth_factor, double fixed)
{
  using pq_element = std::pair<SimTime, NodeId>;
  std::priority_queue<pq_element, std::vector<pq_element>, std::greater<>> pq;

  std::unordered_map<NodeId, SimTime> dist;
  std::unordered_map<NodeId, NodeId> first_hop;
  std::unordered_map<NodeId, RouteInfo> lookup;

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
      SimTime cost = (propagation_delay_factor * l.propagation_delay()) + (bandwidth_factor / l.bandwidth()) + fixed;
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

  for (const auto& kv_pair : first_hop)
  {
    NodeId target = kv_pair.first;
    NodeId neighbor = kv_pair.second;
    lookup[target] = {neighbor, dist[target]};
  }

  return lookup;
}

RouteInfo ShortestPathStrategy::get_route_info(NodeId dst) const
{
  if (self_id == dst)
  {
    return {self_id, 0};
  }
  auto it = next_hop_lookup.find(dst);
  if (it != next_hop_lookup.end())
  {
    return it->second; // returns NEXT NEIGHBOR and TOTAL DISTANCE
  }
  else
  {
    // not reachable from us... just return self_id for now
    std::cout << "Node " << self_id << " has no route to destination. Returning self" << std::endl;
    return std::make_pair(self_id, 0.0);
  }
}

NodeId ShortestPathStrategy::choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const
{
  return get_route_info(p.dst).first;
}

// note this is the same as ShortestPathStrategy
RouteInfo CongestionAwareStrategy::get_route_info(NodeId dst) const
{
  if (self_id == dst)
  {
    return {self_id, 0};
  }
  auto it = next_hop_lookup.find(dst);
  if (it != next_hop_lookup.end())
  {
    return it->second;
  }
  else
  {
    // not reachable from us... just return self_id for now
    std::cout << "Node " << self_id << " has no route to destination. Returning self" << std::endl;
    return std::make_pair(self_id, 0.0);
  }
}

NodeId CongestionAwareStrategy::choose_next_hop(NodeId self_id, Packet& p, Simulation& sim) const
{
  NodeId best_neighbor = self_id;
  SimTime best_estimate = std::numeric_limits<SimTime>::infinity();

  // go through the Links. Return neighbor with minimized "estimate to get there" + "distance metric after that"
  for (const Link& l : sim.get_links(self_id))
  {
    NodeId n = l.to();
    const Strategy& neighbor_strat = sim.get_node(n).get_strategy();

    SimTime estimate = (this->congestion_factor * (l.next_free_time() - sim.now()))
                      + l.propagation_delay()
                      + p.packet_size / l.bandwidth()
                      + neighbor_strat.get_route_info(p.dst).second;
    if (estimate < best_estimate)
    {
      best_neighbor = n;
      best_estimate = estimate;
    }
  }

  return best_neighbor;
}
