// [treesource] This defines the Flow class, a stateful flow of data.

#pragma once
#include "sim_types.hpp"
#include "congestion_control.hpp"
#include <unordered_map>
#include <cstdint>
#include <memory>

class Simulation;
struct Packet;

struct FlightInfo
{
  SimTime send_time;
  int packet_size;
};

class Flow
{
public:
  Flow(FlowId id, NodeId src, NodeId dst, int64_t total_bytes, int packet_size,
      CongestionControlType cc_type = CongestionControlType::AIMD,
      CongestionControlParams cc_params = CongestionControlParams{});

  FlowId id() const { return flow_id; }
  NodeId src() const { return src_; }
  NodeId dst() const { return dst_; }
  double get_cwnd() const { return cc_->get_cwnd(); }
  size_t in_flight_count() const { return in_flight.size(); }
  int64_t in_flight_bytes() const { return in_flight_bytes_; }

  // kick off the flow, call once right after construction
  void start(Simulation& sim);

  // called by Node::receive_packet when ACK for us arrives
  void on_ack(Packet& ack, Simulation& sim);

  // called by scheduled timeout event, no-op if already ACKed
  void on_timeout(PacketId pid, Simulation& sim);

  // true only when every byte sent and in_flight is empty
  bool is_complete() const { return bytes_remaining <= 0 && in_flight.empty(); }

private:
  void maybe_send(Simulation& sim); // may schedule itself
  void send_one_packet(Simulation& sim);
  void schedule_timeout(PacketId pid, Simulation& sim);

  FlowId flow_id;
  NodeId src_;
  NodeId dst_;
  int packet_size;
  int64_t bytes_remaining;

  std::unique_ptr<CongestionControl> cc_;
  SimTime rtt_estimate = 0.0;

  // maps pid to send time and size, for sent but not ACKed
  std::unordered_map<PacketId, FlightInfo> in_flight;
  int64_t in_flight_bytes_ = 0;
};
