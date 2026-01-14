// [treesource] This defines the interface for the Simulation class, to be used by main to interact with it

#pragma once
#include <vector>
#include <memory>
#include <queue>
#include "sim_types.hpp"
#include "event.hpp"
#include "node.hpp"
#include "link.hpp"
#include "packet.hpp"
#include "strategy.hpp"

struct NetworkDesc;

// this selects which strategy to use when instantiating
enum class StrategyPick { RandomNeighborStrategy_, ShortestPathStrategy_, CongestionAwareStrategy_ };

class Simulation
{
public:
  Simulation();

  void initialize_topology(NetworkDesc& desc); // desc is produced by an instance of NetworkGenerator, for example
  void initialize_strategies(StrategyPick strategy, std::vector<double> strategy_params, Simulation& sim); // assure strategy_params are correct.

  SimTime now() const; // get the current time of the simulation
  bool done() const;   // true if no more events will occur

  void schedule(std::unique_ptr<Event> e); // add an event to the simulation.
  void schedule_all_packets(Simulation& sim); // schedules initial event for all packets.
  void step(); // executes the next event within the simulation.
  void run(); // step() until no more events are left.

  NodeId add_node(SimTime send_rate, std::unique_ptr<Strategy> strategy);
  Node &get_node(NodeId id); // return Node& for the given id

  PacketId add_packet(NodeId source, NodeId dest, int packet_size, SimTime creation_time);
  Packet &get_packet(PacketId id);
  // the owner of a Packet is assumed to be source at creation, and updates when the next node receives it.
  // it's assumed that Nodes and Packets can't be removed, so lookup by id is trivial.

  void add_directed_link(NodeId from, NodeId to, SimTime propagation_delay, double bandwidth); // assumes Nodes exist
  void add_undirected_link(NodeId from, NodeId to, SimTime propagation_delay, double bandwidth); // link both ways.

  std::vector<Link>& get_links(NodeId id); // returns outgoing Links for a Node.
  Link& get_link(NodeId from, NodeId to); // return Link reference, assuming it exists

  std::vector<NodeId> get_nodes() const;

  void print_nodes() const;
  void print_packets() const;
  void print_adj_list() const;

private:
  SimTime current_time;

  std::vector<std::unique_ptr<Node>> nodes;
  std::vector<std::unique_ptr<Packet>> packets;
  std::vector<std::vector<Link>> adj_list;

  std::priority_queue<
      std::unique_ptr<Event>,
      std::vector<std::unique_ptr<Event>>,
      EventCompare>
      event_queue;
};
