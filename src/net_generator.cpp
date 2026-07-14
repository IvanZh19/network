// [treesource] This implements the NetworkGenerator class.

#include "net_generator.hpp"
#include <unordered_set>
#include <algorithm>

NetworkGenerator::NetworkGenerator(unsigned seed)
  : rng(seed) {}

SimTime NetworkGenerator::rand_delay(SimTime min, SimTime max)
{
  std::uniform_real_distribution<SimTime> dist(min, max);
  return dist(rng);
}

double NetworkGenerator::rand_bandwidth(double min, double max)
{
  std::uniform_real_distribution<double> dist(min, max);
  return dist(rng);
}

NetworkDesc NetworkGenerator::random_connected(
    int num_nodes,
    int extra_edges,
    SimTime min_delay,
    SimTime max_delay,
    double min_bandwidth,
    double max_bandwidth,
    size_t capacity)
{
  NetworkDesc desc;

  desc.nodes.reserve(num_nodes);
  for (int i = 0; i < num_nodes; ++i)
  {
    desc.nodes.push_back({nullptr});
  }

  // guarantee connectivity, spanning tree with root at node 0.
  // undirected edges, with same delay and bandwidth.
  for (NodeId i = 1; i < num_nodes; ++i)
  {
    std::uniform_int_distribution<NodeId> parent_dist(0, i-1);
    NodeId parent = parent_dist(rng);
    SimTime random_delay = rand_delay(min_delay, max_delay);
    double random_bandwidth = rand_bandwidth(min_bandwidth, max_bandwidth);
    desc.links.push_back({
      parent,
      i,
      random_delay,
      random_bandwidth,
      capacity});
    desc.links.push_back({
      i,
      parent,
      random_delay,
      random_bandwidth,
      capacity});
  }

  std::uniform_int_distribution<NodeId> node_dist(0, num_nodes-1);

  // add other eddges
  for (int i = 0; i < extra_edges; ++i)
  {
    NodeId a = node_dist(rng);
    NodeId b = node_dist(rng);
    if (a == b) continue;

    desc.links.push_back({
      a,
      b,
      rand_delay(min_delay, max_delay),
      rand_bandwidth(min_bandwidth, max_bandwidth),
      capacity});
  }

  return desc;
}

void NetworkGenerator::add_uniform_random_packets(
    NetworkDesc& desc,
    int num_packets,
    int min_size,
    int max_size,
    SimTime start_time,
    SimTime end_time)
{
  if (desc.nodes.empty())
  {
    return;
  }

  std::uniform_int_distribution<int> node_dist(0, desc.nodes.size() - 1);
  std::uniform_int_distribution<int> size_dist(min_size, max_size);
  std::uniform_real_distribution<double> time_dist(start_time, end_time);

  int added = 0;
  while (added < num_packets)
  {
    NodeId src = node_dist(rng);
    NodeId dst = node_dist(rng);
    if (src == dst) continue;

    desc.packets.push_back({
      src,
      dst,
      size_dist(rng),
      time_dist(rng)
    });

    ++added;
  }
}

void NetworkGenerator::add_undirected_edge(
    NetworkDesc& desc,
    NodeId a,
    NodeId b,
    SimTime min_delay,
    SimTime max_delay,
    double min_bandwidth,
    double max_bandwidth,
    size_t capacity)
{
  SimTime delay = rand_delay(min_delay, max_delay);
  double bandwidth = rand_bandwidth(min_bandwidth, max_bandwidth);
  desc.links.push_back({a, b, delay, bandwidth, capacity});
  desc.links.push_back({b, a, delay, bandwidth, capacity});
}

NetworkDesc NetworkGenerator::scale_free(
  int num_nodes, int m,
  SimTime min_delay, SimTime max_delay,
  double min_bandwidth, double max_bandwidth,
  size_t capacity)
{
  NetworkDesc desc;
  if (num_nodes <= 0) return desc;

  desc.nodes.reserve(num_nodes);
  for (int i = 0; i < num_nodes; i++)
  {
    desc.nodes.push_back({nullptr});
  }

  // needed to guarantee progress
  m = std::min(m, std::max(1, num_nodes-1));

  // repeated_nodes holds each node once per edge endpoint it currently owns
  // sampling uniformly from this then samples proportional to degree
  std::vector<NodeId> repeated_nodes;

  // seed with small fully connected core, so can start with something nonzero degree
  int core_size = std::min(num_nodes, m + 1);
  for (NodeId i = 0; i < static_cast<NodeId>(core_size); i++)
  {
    for (NodeId j = i + 1; j < static_cast<NodeId>(core_size); j++)
    {
      add_undirected_edge(desc, i, j, min_delay, max_delay, min_bandwidth, max_bandwidth, capacity);
      repeated_nodes.push_back(i);
      repeated_nodes.push_back(j);
    }
  }

  for (NodeId new_node = core_size; new_node < static_cast<NodeId>(num_nodes); new_node++)
  {
    std::unordered_set<NodeId> targets;
    while (static_cast<int>(targets.size()) < m)
    {
      std::uniform_int_distribution<size_t> dist(0, repeated_nodes.size() - 1);
      targets.insert(repeated_nodes[dist(rng)]);
    }

    for (NodeId target : targets)
    {
      add_undirected_edge(desc, new_node, target, min_delay, max_delay, min_bandwidth, max_bandwidth, capacity);
      repeated_nodes.push_back(new_node);
      repeated_nodes.push_back(target);
    }
  }

  return desc;
}
