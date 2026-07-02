// [treesource] This defines the behavior of OutputPorts.

#include "output_port.hpp"
#include "sim.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "event.hpp"

void OutputPort::enqueue_and_drain(PacketId pid, Simulation& sim)
{
  packet_queue_.push(pid);
  if (!busy_)
  {
    busy_ = true;
    sim.schedule(std::make_unique<Event>(sim.now(),
      [this, &sim]() { this->do_send(sim); }
    ));
  }
}

void OutputPort::do_send(Simulation& sim)
{
  if (packet_queue_.empty())
  {
    busy_ = false;
    return;
  }

  PacketId pid = packet_queue_.front();
  packet_queue_.pop();

  Packet& p = sim.get_packet(pid);
  Link& link = sim.get_link(owner_, neighbor_);

  SimTime arrival_time = link.reserve(p, sim.now());

  sim.log({sim.now(), EventType::PacketSend, owner_, neighbor_, pid});

  // schedule arrival at next node
  NodeId dst = neighbor_;
  sim.schedule(std::make_unique<Event>(arrival_time,
    [dst, pid, &sim]() { sim.get_node(dst).receive_packet(pid, sim); }
  ));

  // to chain do_sends.
  if (!packet_queue_.empty())
  {
    SimTime next_send_time = link.next_free_time();
    sim.schedule(std::make_unique<Event>(next_send_time,
      [this, &sim]() { this->do_send(sim); }
    ));
  }
  else
  {
    busy_ = false;
  }
}
