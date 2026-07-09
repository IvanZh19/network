// [treesource] This is the implementation of the Flow class.

#include "flow.hpp"
#include "packet.hpp"
#include "sim.hpp"
#include "event.hpp"

// TODO: cleanup these hardcoded values?
constexpr double RTT_ALPHA = 0.2;
constexpr SimTime MIN_RTO = 0.05;
constexpr SimTime DEFAULT_RTO = 1.0;

Flow::Flow(FlowId id, NodeId src, NodeId dst, int64_t total_bytes, int packet_size,
            CongestionControlType cc_type, CongestionControlParams cc_params)
  : flow_id(id), src_(src), dst_(dst), bytes_remaining(total_bytes), packet_size(packet_size),
    cc_(make_congestion_control(cc_type, cc_params)) {}

void Flow::start(Simulation& sim)
{
  maybe_send(sim);
}

void Flow::maybe_send(Simulation& sim)
{
  while (in_flight.size() < static_cast<size_t>(cc_->get_cwnd()) && bytes_remaining > 0)
  {
    send_one_packet(sim);
  }
}

void Flow::send_one_packet(Simulation& sim)
{
  int this_size = std::min<int64_t>(packet_size, bytes_remaining);
  bytes_remaining -= this_size;

  PacketId pid = sim.add_packet(src_, dst_, this_size, sim.now(), flow_id);
  in_flight[pid] = {.send_time=sim.now(), .packet_size=this_size};

  sim.get_node(src_).originate_packet(pid, sim);
  schedule_timeout(pid, sim);
}

void Flow::schedule_timeout(PacketId pid, Simulation& sim)
{
  SimTime rto = (rtt_estimate > 0.0) ? std::max(2.0 * rtt_estimate, MIN_RTO) : DEFAULT_RTO;
  sim.schedule(std::make_unique<Event>(sim.now() + rto,
    [this, pid, &sim]() { this->on_timeout(pid, sim); }
  ));
}

void Flow::on_ack(Packet& ack, Simulation& sim)
{
  auto it = in_flight.find(ack.acked_pid);
  if (it == in_flight.end())
  {
    return; // stale ACK
  }

  SimTime sample_rtt = sim.now() - it->second.send_time;
  size_t acked_bytes = it->second.packet_size;
  rtt_estimate = (rtt_estimate <= 0.0) ? sample_rtt : (1 - RTT_ALPHA) * rtt_estimate + RTT_ALPHA * sample_rtt;
  in_flight.erase(it);

  cc_->on_ack(sample_rtt, acked_bytes, sim.now());

  if (is_complete())
  {
    sim.log({sim.now(), EventType::FlowComplete, src_, dst_, INVALID_PACKET_ID});
    return; // don't call maybe_send, nothing to send
  }

  maybe_send(sim);
}

void Flow::on_timeout(PacketId pid, Simulation& sim)
{
  auto it = in_flight.find(pid);
  if (it == in_flight.end())
  {
    return; // already ACKed
  }

  in_flight.erase(it);
  cc_->on_timeout();

  // treat as lost, resend a fresh packet. pids are meant to be unique for sim purposes, so
  // for now they represent the same data but are distinct packets.
  int this_size = it->second.packet_size; // use the size stored in in_flight
  PacketId new_pid = sim.add_packet(src_, dst_, this_size, sim.now(), flow_id);
  in_flight[new_pid] = {.send_time=sim.now(), .packet_size=this_size};
  sim.get_node(src_).originate_packet(new_pid, sim);
  schedule_timeout(new_pid, sim);
}
