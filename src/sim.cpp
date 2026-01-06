// [treesource] This is the implementation of the Simulation class.

#include "sim.hpp"
#include "node.hpp"
#include "packet.hpp"
#include "event.hpp"
#include "sim_types.hpp"
#include <iostream>
#include <memory>
#include <assert.h>

Simulation::Simulation()
{
  current_time = 0.0;
  // note that other fields are fine as is, will get changed by methods.
}

SimTime Simulation::now() const
{
  return current_time;
}

bool Simulation::done() const
{
  return event_queue.empty();
}

void Simulation::schedule(std::unique_ptr<Event> e)
{
  event_queue.push(std::move(e)); // move ownership into queue
}

void Simulation::step()
{
  assert(!done());

  std::unique_ptr<Event> e = std::move(const_cast<std::unique_ptr<Event>&>(event_queue.top()));
  event_queue.pop();

  current_time = e->time;
  e->action(); // if e requires the sim context, it should be a lambda using [&sim]
}

void Simulation::run()
{
  while (!done())
  {
    step();
  }
}

NodeId Simulation::add_node(SimTime send_rate, std::unique_ptr<Strategy> strategy)
{
  NodeId id = nodes.size();
  auto n = std::make_unique<Node>(nodes.size(), send_rate, std::move(strategy));
  nodes.push_back(std::move(n));
  adj_list.emplace_back(); // make empty edge list for this node
  return id;
}

Node &Simulation::get_node(NodeId id)
{
  assert (id < nodes.size());
  return *nodes.at(id);
}

PacketId Simulation::add_packet(NodeId source, NodeId dest, int packet_size, SimTime creation_time)
{
  PacketId pid = packets.size();
  auto p = std::make_unique<Packet>(pid, source, dest, source, packet_size, creation_time);
  packets.push_back(std::move(p));
  return pid;
}

Packet &Simulation::get_packet(PacketId id)
{
  assert (id < packets.size());
  return *packets.at(id);
}

void Simulation::add_directed_link(NodeId from, NodeId to, SimTime latency)
{
  assert (from < nodes.size() && to < nodes.size());
  adj_list[from].push_back(Link(from, to, latency));
}

void Simulation::add_undirected_link(NodeId from, NodeId to, SimTime latency)
{
  Simulation::add_directed_link(from, to, latency);
  Simulation::add_directed_link(to, from, latency);
}

std::vector<Link>& Simulation::get_links(NodeId id)
{
  assert(id < adj_list.size());
  return adj_list[id];
}

SimTime Simulation::get_weight(NodeId from, NodeId to)
{
  std::vector<Link> links = get_links(from);
  for (Link l : links)
  {
    if (l.to() == to)
    {
      return l.latency();
    }
  }
  // TODO. might fall to here and be buggy. add Throw and FIX.
  assert(false);
  return 0.0;
}

void Simulation::print_nodes() const
{
  std::cout << "Nodes: \n";
  for (const auto& n_ptr : nodes) {
    std::cout << "- Node " << n_ptr->id() << "\n";
  }
}

void Simulation::print_packets() const
{
  std::cout << "Packets: \n";
  for (const auto& p_ptr : packets) {
    std::cout << "- Packet " << p_ptr->id
              << "  - owner: " << p_ptr->owner
              << "  - src: " << p_ptr->src
              << "  - dst: " << p_ptr->dst
              << "  - size: " << p_ptr->packet_size
              << "  - created at: " << p_ptr->creation_time
              << "\n";
  }
}

void Simulation::print_adj_list() const
{
  std::cout << "Adjacency list: \n";
  for (NodeId i = 0; i < adj_list.size(); ++i) {
    std::cout << "- Node " << i << ": ";
    for (const Link &l : adj_list[i]) {
      std::cout << "(" << l.from() << ", " << l.to() << ", " << l.latency() << ") ";
    }
    std::cout << "\n";
  }
}
