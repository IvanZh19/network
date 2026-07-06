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
#include "flow.hpp"
#include "strategy.hpp"
#include "logger.hpp"
#include <cstdint>

struct NetworkDesc;

// TODO maybe handle these differently
constexpr FlowId INVALID_FLOW_ID = 0;
constexpr PacketId INVALID_PACKET_ID = 0;
constexpr int ACK_SIZE = 40;

class Simulation
{
public:
  Simulation();

  void initialize_topology(NetworkDesc& desc); // desc is produced by an instance of NetworkGenerator, for example
  void initialize_strategies(StrategyType strategy, StrategyParams params, Simulation& sim);

  SimTime now() const; // get the current time of the simulation
  bool done() const;   // true if no more events will occur

  void schedule(std::unique_ptr<Event> e); // add an event to the simulation.
  void schedule_all_packets(Simulation& sim); // schedules initial event for all packets.
  void step(); // executes the next event within the simulation.
  void run(); // step() until no more events are left.

  NodeId add_node(std::unique_ptr<Strategy> strategy);
  Node &get_node(NodeId id); // return Node& for the given id

  PacketId add_packet(NodeId source, NodeId dest, int packet_size, SimTime creation_time,
                      FlowId flow_id = INVALID_FLOW_ID, bool is_ack = false, PacketId acked_pid = INVALID_PACKET_ID);
  Packet &get_packet(PacketId id);
  // the owner of a Packet is assumed to be source at creation, and updates when the next node receives it.
  // it's assumed that Nodes and Packets can't be removed, so lookup by id is trivial.

  void add_directed_link(NodeId from, NodeId to, SimTime propagation_delay, double bandwidth, size_t capacity = 64);
  void add_undirected_link(NodeId from, NodeId to, SimTime propagation_delay, double bandwidth, size_t capacity = 64);

  std::vector<Link>& get_links(NodeId id); // returns outgoing Links for a Node.
  Link& get_link(NodeId from, NodeId to); // return Link reference, assuming it exists

  FlowId add_flow(NodeId src, NodeId dst, int64_t total_bytes, int packet_size);
  Flow& get_flow(FlowId id);

  std::vector<NodeId> get_nodes() const;

  void print_nodes() const;
  void print_packets() const;
  void print_adj_list() const;

  void log(EventRecord er) { logger.log(er); }

  // temp pass through
  const std::vector<EventRecord>& get_event_records() const { return logger.get_event_records(); }

  // export as csv
  void export_log(const std::string &filename) const { logger.dump_csv(filename); }

  // export as csv
  void export_packets(const std::string &filename) const;

  // export as json
  void export_network(const std::string &filename) const;

private:
  SimTime current_time;

  std::vector<std::unique_ptr<Node>> nodes;
  std::vector<std::unique_ptr<Packet>> packets; // TODO: garbage collect dropped/delivered packets?
  std::vector<std::vector<Link>> adj_list;
  std::unordered_map<FlowId, Flow> flows;

  std::priority_queue<
      std::unique_ptr<Event>,
      std::vector<std::unique_ptr<Event>>,
      EventCompare>
      event_queue;

  Logger logger;
};
