#pragma once
#include <vector>
#include "packet.hpp"

struct Node
{
  int id;
  std::vector<int> neighbors;

  void receive_packet(const Packet &pkt);
  void send_packet(const Packet &pkt, int next_node);
};
