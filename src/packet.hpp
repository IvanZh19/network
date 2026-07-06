// [treesource] This defines the Packet struct.

#pragma once
#include "sim_types.hpp"

struct Packet
{
  PacketId id;
  NodeId src;
  NodeId dst;
  NodeId owner; // note: assume that a packet is "delivered" when dst==owner. may want to change later.
  int packet_size;
  SimTime creation_time;
  FlowId flow_id;
  bool is_ack;
  PacketId acked_pid;

  Packet(PacketId pid, NodeId source, NodeId dest, NodeId current_owner, int packet_size, SimTime creation_time,
          FlowId flow_id, bool is_ack, PacketId acked_pid)
    : id(pid), src(source), dst(dest), owner(current_owner), packet_size(packet_size), creation_time(creation_time),
      flow_id(flow_id), is_ack(is_ack), acked_pid(acked_pid) {}
};
