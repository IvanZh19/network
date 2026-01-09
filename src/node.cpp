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
  // std::cout << "Node " << nid << " handling next Packet " << " at t=" << sim.now() << std::endl;

  // if somehow nothing to send, not busy anymore
  if (packet_queue.empty())
  {
    std::cout << "Node " << nid << " has no Packets" << std::endl;

    is_busy = false;
    return;
  }

  is_busy = true;

  // pop next Packet to send
  PacketId pid = packet_queue.front();
  packet_queue.pop();

  std::cout << "Node " << nid << " handling Packet " << pid << " at t=" << sim.now() << std::endl;

  // get Packet and next hop.
  Packet& p = sim.get_packet(pid);
  NodeId next = choose_next_hop(p, sim);

  // reserve/schedule this Packet on that Link
  Link& link = sim.get_link(nid, next);
  SimTime arrival_time = link.reserve(p, sim.now()); // time when it fully arrives at next

  // next ready when both link and us are free. because of how scheduling Events and sim.now()
  // works, this max is necessary to also account for the link's actual next start time.
  SimTime node_ready_time = std::max(sim.now() + send_rate, link.next_free_time());

  // packet arrival for next node
  sim.schedule(std::make_unique<Event>(arrival_time,
          [&sim, next, pid]() {sim.get_node(next).receive_packet(pid, sim); }));

  // schedule send another check later.
  // sim.schedule(std::make_unique<Event>(node_ready_time,
  //       [this, &sim]()
  //           {
  //             is_busy = false;

  //             if (!packet_queue.empty()) {
  //               this->send_packet(sim);
  //             }
  //           }));


  // // if queue still has stuff, schedule another send attempt
  if (!packet_queue.empty())
  {
    sim.schedule(std::make_unique<Event>(
      node_ready_time, [this, &sim]() { this->send_packet(sim); }
    ));
  }
  else
  {
    is_busy = false;
  }
}

NodeId Node::choose_next_hop(Packet& p, Simulation& sim) const
{
  // just access our strategy's decision.
  return strategy->choose_next_hop(nid, p, sim);
}
