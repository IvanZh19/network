// [treesource] This defines the Flow class, a stateful flow of data.

#pragma once
#include "sim_types.hpp"
#include <unordered_map>

class Simulation;
struct Packet;

class Flow
{
public:
  Flow(FlowId id, NodeId src, NodeId dst, int64_t total_bytes, int packet_size);

}
