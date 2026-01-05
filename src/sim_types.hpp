// [treesource] These are the relevant types, mostly for human legibility and sanity.

#pragma once

using SimTime = double;
using PacketId = long long unsigned int;
using NodeId = long long unsigned int;

// Edge structs used in adjacency list.
struct Edge {
  NodeId to;
  SimTime delay;
};
