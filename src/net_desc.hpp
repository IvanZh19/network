// [treesource] This defines the structs used to aid the NetworkGenerator class.

#pragma once

#include <vector>
#include <memory>
#include "sim_types.hpp"
#include "strategy.hpp"

// The Spec structs are essentially POD structs holding parameters defining the network in a Simulation.

struct NodeSpec
{
  SimTime send_rate;
  std::unique_ptr<Strategy> strategy; // this is always nullptr for now.
};

struct LinkSpec
{
  NodeId from;
  NodeId to;
  SimTime propagation_delay;
  double bandwidth;
};

// PacketSpec is the same as Packet... but without the constructor and without ID.
struct PacketSpec
{
  NodeId src;
  NodeId dst;
  int packet_size;
  SimTime creation_time;
};

struct NetworkDesc
{
  std::vector<NodeSpec> nodes;
  std::vector<LinkSpec> links;
  std::vector<PacketSpec> packets;
};
