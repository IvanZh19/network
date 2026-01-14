// [treesource] This is the NetworkGenerator class, meant to help construct network info that helps set up a Simulation.

#pragma once

#include "net_desc.hpp"
#include <random>

class NetworkGenerator
{
public:
  explicit NetworkGenerator(unsigned seed = std::random_device{}());

  // this is guaranteed to contain an undirected spanning tree
  // but nodes all have nullptr for strategy.
  // call sim.initialize_strategies after.
  NetworkDesc random_connected(
    int num_nodes,
    int extra_edges,
    SimTime min_delay,
    SimTime max_delay,
    double min_bandwidth,
    double max_bandwidth
  );

  void add_uniform_random_packets(
    NetworkDesc& desc,
    int num_packets,
    int min_size,
    int max_size,
    SimTime start_time,
    SimTime end_time
  );

private:
  std::mt19937 rng;

  SimTime rand_delay(SimTime min, SimTime max);
  double rand_bandwidth(double min, double max);
};
