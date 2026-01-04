// [treesource] This defines the interface for the Simulation class, to be used by main to interact with it

#pragma once
#include <vector>
#include <memory>
#include <queue>
#include "sim_types.hpp"
#include "event.hpp"

// forward decl so it knows these exist
class Node;
class Packet;
class Event;

class Simulation
{
public:
  Simulation();

  SimTime now() const; // get the current time of the simulation
  bool done() const;   // true if no more events will occur

  void schedule(std::unique_ptr<Event> e);
  void step(); // executes one event within the simulation.

  Node &node(NodeId id);       // add a node to the simulation
  Packet &packet(PacketId id); // add a packet to the simulation.

private:
  SimTime current_time;

  std::vector<Node> nodes;
  std::vector<Packet> packets;
  std::vector<std::vector<SimTime>> adj_list; // to store edge weights and network topology

  std::priority_queue<
      std::unique_ptr<Event>,
      std::vector<std::unique_ptr<Event>>,
      EventCompare>
      event_queue;
};
