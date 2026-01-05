// [treesource] This defines the Packet struct.

#pragma once
#include "sim_types.hpp"

struct Packet
{
  PacketId id;
  NodeId src;
  NodeId dst;
  NodeId owner;
  int packet_size;
  SimTime creation_time;

  Packet(PacketId pid, NodeId source, NodeId dest, NodeId current_owner, int packet_size, SimTime creation_time)
    : id(pid), src(source), dst(dest), owner(current_owner), packet_size(packet_size), creation_time(creation_time) {}
};
