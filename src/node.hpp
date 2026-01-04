#pragma once
#include <vector>
#include "packet.hpp"

struct Node
{
  NodeId id;
  std::vector<NodeId> neighbors;

  void receive_packet(const Packet &pkt);
  void send_packet(const Packet &pkt, int next_node);
};
