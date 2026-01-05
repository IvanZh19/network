// [treesource] This defines the behavior of Nodes.

#include <iostream>
#include "node.hpp"
#include "packet.hpp"
#include "sim.hpp"
#include <random>
// #include <cassert>

void Node::receive_packet(PacketId pid, Simulation& sim)
{
  std::cout << "Node " << nid << " received Packet " << pid << " at t=" << sim.now() << std::endl;

  Packet& p = sim.get_packet(pid);

  p.owner = nid; // owner is now us

  // if we are not dst, schedule next event
  if (p.dst != nid)
  {
    NodeId next = choose_next_hop(p, sim);
    sim.schedule(std::make_unique<Event>(sim.now() + sim.get_weight(nid, next),
  [pid, next, &sim] {sim.get_node(next).receive_packet(pid, sim); }
    ));
  }


}

NodeId Node::choose_next_hop(const Packet& p, Simulation& sim) const
{
  // um implement stuff here.
  // for now just do a random pick...
  // assumes that we're not the dst.
  std::vector<Edge>& edges = sim.get_edges(nid);

  if (edges.empty())
  {
    return nid; // no neighbors... // TODO: possible bug where self to self does not exist.
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> dist(0, edges.size()-1);

  size_t index = dist(gen);
  return edges[index].to;

}
