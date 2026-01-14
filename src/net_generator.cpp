// [treesource] This implements the NetworkGenerator class.

#include "net_generator.hpp"

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
    double max_bandwidth)
{
  NetworkDesc desc;

  desc.nodes.reserve(num_nodes);
  for (int i = 0; i < num_nodes; ++i)
  {
    desc.nodes.push_back({1.0, nullptr});
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
      random_bandwidth});
    desc.links.push_back({
      i,
      parent,
      random_delay,
      random_bandwidth});
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
      rand_bandwidth(min_bandwidth, max_bandwidth)});
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
