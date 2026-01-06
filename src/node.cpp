// [treesource] This defines the behavior of Nodes.

#include <iostream>
#include "node.hpp"
#include "packet.hpp"
#include "sim.hpp"
#include "strategy.hpp"

// NAIVE, INSTANT FORWARD IMPLEMENTATION OF RECEIVE_PACKET.
// void Node::receive_packet(PacketId pid, Simulation& sim)
// {
//   std::cout << "Node " << nid << " received Packet " << pid << " at t=" << sim.now() << std::endl;

//   Packet& p = sim.get_packet(pid);

//   p.owner = nid; // owner is now us

//   // if we are not dst, schedule a packet-arrival Event (calls receive_packet of the next Node)
//   if (p.dst != nid)
//   {
//     NodeId next = choose_next_hop(p, sim);
//     sim.schedule(std::make_unique<Event>(sim.now() + sim.get_weight(nid, next),
//   [pid, next, &sim] {sim.get_node(next).receive_packet(pid, sim); }
//     ));
//   }

// }

void Node::receive_packet(PacketId pid, Simulation& sim)
{
  std::cout << "Node " << nid << " received Packet " << pid << " at t=" << sim.now() << std::endl;

  Packet& p = sim.get_packet(pid);

  p.owner = nid; // we're now the owner.

  if (p.dst == nid)
  {
    // delivered! do nothing. unless want to.
    return;
  }

  packet_queue.push(pid);

  // if idle, i.e. no send-Event scheduled, schedule a send-Event, which will trigger immediately.
  if (!is_busy)
  {
    is_busy = true;
    sim.schedule(std::make_unique<Event>(sim.now(), [this, &sim]() {this->send_packet(sim); }));
  }

}

void Node::send_packet(Simulation& sim)
{
  std::cout << "Node " << nid << " sending next Packet " << " at t=" << sim.now() << std::endl;

  // if somehow nothing to send, not busy anymore
  if (packet_queue.empty())
  {
    std::cout << "Nevermind, Node " << nid << " has no Packets" << std::endl;

    is_busy = false;
    return;
  }

  is_busy = true;

  PacketId pid = packet_queue.front();
  packet_queue.pop();

  Packet& p = sim.get_packet(pid);
  NodeId next = choose_next_hop(p, sim);

  SimTime arrival_time = sim.now() + sim.get_weight(nid, next);
  SimTime next_send_time = sim.now() + send_rate;

  // packet arrival for next node
  sim.schedule(std::make_unique<Event>(arrival_time,
          [&sim, next, pid]() {sim.get_node(next).receive_packet(pid, sim); }));

  // schedule event to try sending again later if still busy, or mark as not busy anymore.
  sim.schedule(std::make_unique<Event>(next_send_time,
          [this, &sim]()
              {
                is_busy = false;

                if (!packet_queue.empty()) {
                  this->send_packet(sim);
                }
              }));
}

NodeId Node::choose_next_hop(Packet& p, Simulation& sim) const
{
  // just access our strategy's decision.
  return strategy->choose_next_hop(nid, p, sim);
}
