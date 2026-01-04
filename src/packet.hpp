#pragma once
#include "sim_types.hpp"

struct Packet
{
  PacketId id;
  NodeId src;
  NodeId dst;
  int packet_size;
  SimTime creation_time;

  Packet(PacketId packet_id, NodeId source, NodeId dest, int packet_size, SimTime creation_time);
};
