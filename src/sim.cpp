// [treesource] This is the implementation of the Simulation class.

#include "sim.hpp"
#include "node.hpp"
#include "packet.hpp"
#include "event.hpp"
#include "sim_types.hpp"
#include <iostream>
#include <memory>
#include <assert.h>
#include "net_desc.hpp"
#include <fstream>

Simulation::Simulation()
{
  current_time = 0.0;
  logger = Logger();
  packets.push_back(nullptr); // sentinel for invalid
  // note that other fields are fine as is, will get changed by methods.
}

void Simulation::initialize_topology(NetworkDesc& desc)
{
  for (auto& n : desc.nodes)
  {
    add_node(nullptr);
  }

  for (auto& l : desc.links)
  {
    add_directed_link(l.from, l.to, l.propagation_delay, l.bandwidth, l.capacity);
  }

  for (auto& p : desc.packets)
  {
    add_packet(p.src, p.dst, p.packet_size, p.creation_time);
  }
}

void Simulation::initialize_strategies(StrategyType strategy, StrategyParams params, Simulation& sim)
{
  for (auto& n : nodes)
  {
    n->set_strategy(make_strategy(strategy, n->id(), sim, params));
  }
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

void Simulation::schedule_all_packets(Simulation& sim)
{
  for (auto& p : packets)
  {
    PacketId pid = p->id;
    NodeId psrc = p->src;
    sim.schedule(std::make_unique<Event>(sim.now(),
    [pid, psrc, &sim](){sim.get_node(psrc).receive_packet(pid, sim); }));

  }
}

void Simulation::step()
{
  assert(!done());

  std::unique_ptr<Event> e = std::move(const_cast<std::unique_ptr<Event>&>(event_queue.top()));
  event_queue.pop();

  current_time = e->time;
  e->action(); // if e requires the sim context, it should be a lambda using [&sim]
  tracker.sample(current_time);
}

void Simulation::run()
{
  while (!done())
  {
    step();
  }
}

bool Simulation::run_bounded(size_t max_steps)
{
  size_t steps = 0;
  while (!done())
  {
    if (steps++ >= max_steps) return false;
    step();
  }
  return true;
}

NodeId Simulation::add_node(std::unique_ptr<Strategy> strategy)
{
  NodeId id = nodes.size();
  auto n = std::make_unique<Node>(nodes.size(), std::move(strategy));
  nodes.push_back(std::move(n));
  adj_list.emplace_back(); // make empty edge list for this node
  return id;
}

Node &Simulation::get_node(NodeId id)
{
  assert (id < nodes.size());
  return *nodes.at(id);
}

PacketId Simulation::add_packet(NodeId source, NodeId dest, int packet_size, SimTime creation_time,
                                FlowId flow_id, bool is_ack, PacketId acked_pid)
{
  PacketId pid = packets.size();
  auto p = std::make_unique<Packet>(pid, source, dest, source, packet_size, creation_time, flow_id, is_ack, acked_pid);
  packets.push_back(std::move(p));
  return pid;
}

Packet &Simulation::get_packet(PacketId id)
{
  assert (id < packets.size());
  return *packets.at(id);
}

void Simulation::add_directed_link(NodeId from, NodeId to, SimTime propagation_delay, double bandwidth, size_t capacity)
{
  assert (from < nodes.size() && to < nodes.size());
  adj_list[from].push_back(Link(from, to, propagation_delay, bandwidth));
  get_node(from).add_port(to, capacity);
}

void Simulation::add_undirected_link(NodeId from, NodeId to, SimTime propagation_delay, double bandwidth, size_t capacity)
{
  Simulation::add_directed_link(from, to, propagation_delay, bandwidth, capacity);
  Simulation::add_directed_link(to, from, propagation_delay, bandwidth, capacity);
}

std::vector<Link>& Simulation::get_links(NodeId id)
{
  assert(id < adj_list.size());
  return adj_list[id];
}

Link& Simulation::get_link(NodeId from, NodeId to)
{
  std::vector<Link>& links = get_links(from);
  for (Link& l : links)
  {
    if (l.to() == to)
    {
      return l;
    }
  }
  // TODO. might fall to here and be buggy. Throw for now.
  throw std::runtime_error(
    "No link found..."
  );
}

FlowId Simulation::add_flow(NodeId src, NodeId dst, int64_t total_bytes, int packet_size,
                            CongestionControlType cc_type, CongestionControlParams cc_params)
{
  FlowId id = flows.size() + 1; // 0 is invalid
  flows.emplace(id, Flow(id, src, dst, total_bytes, packet_size, cc_type, cc_params));
  flows.at(id).start(*this);
  return id;
}

Flow& Simulation::get_flow(FlowId id)
{
  return flows.at(id);
}

std::vector<NodeId> Simulation::get_nodes() const
{
  std::vector<NodeId> ids;
  ids.reserve(nodes.size());
  for (const auto& node_ptr : nodes)
  {
    ids.push_back(node_ptr->id());
  }
  return ids;
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
      std::cout << "(" << l.from() << ", " << l.to() << ", " << l.propagation_delay() << ", " << l.bandwidth() << ") ";
    }
    std::cout << "\n";
  }
}

void Simulation::export_packets(const std::string &filename) const
{
  std::ofstream out(filename);
  if (!out)
  {
    throw std::runtime_error("Failed to open packets file");
  }

  out << "id,src,dst,owner,packet_size,creation_time\n";

  for (const auto& p : packets)
  {
    out << p->id << ","
        << p->src << ","
        << p->dst << ","
        << p->owner << ","
        << p->packet_size << ","
        << p->creation_time << "\n";
  }
}

void Simulation::export_network(const std::string &filename) const
{
  std::ofstream out(filename);
  if (!out)
  {
    throw std::runtime_error("Failed to open network file");
  }

  out << "{\n";

  out << "  \"nodes\": [\n";
  for (size_t i = 0; i < nodes.size(); ++i)
  {
    const auto& n = nodes[i];
    out << "    { "
        << "\"nid\": " << n->id()
        << " }";
    if (i + 1 < nodes.size()) out << ",";
    out << "\n";
  }
  out << "  ],\n";

  out << "  \"links\": [\n";
  bool first = true;
  for (const auto& links : adj_list)
  {
    for (const auto& l : links)
    {
      if (!first) out << ",\n";
      first = false;

      out << "    { "
          << "\"from\": " << l.from()
          << ", \"to\": " << l.to()
          << ", \"propagation_delay\": " << l.propagation_delay()
          << ", \"bandwidth\": " << l.bandwidth()
          << " }";
    }
  }

  out << "\n  ]\n";
  out << "}\n";
}
