// [treesource] This defines the behavior of Nodes.

#include "node.hpp"
#include "packet.hpp"
#include "sim.hpp"
#include "strategy.hpp"

void Node::add_port(NodeId neighbor, size_t capacity)
{
  // emplace constructs in place, so handed-out port references can remain valid
  ports.emplace(neighbor, OutputPort(nid, neighbor, capacity));
}

void Node::receive_packet(PacketId pid, Simulation& sim)
{
  // std::cout << "Node " << nid << " received Packet " << pid << " at t=" << sim.now() << std::endl;

  Packet& p = sim.get_packet(pid);

  if (p.owner == nid)
  {
    // note that if from == nid, this usually means that we are src, and a receive_packet event was triggered on us to begin
    // this is assumed to be equivalent to create packet
    sim.log({sim.now(), EventType::PacketCreate, p.src, p.dst, p.id});
  }
  else
  {
    sim.log({sim.now(), EventType::PacketReceive, p.owner, nid, p.id});
  }

  p.owner = nid; // we're now the owner.

  if (p.dst == nid)
  {
    // delivered! do nothing. unless want to.
    return;
  }

  NodeId next = choose_next_hop(p, sim);

  // route straight to the desired port.
  ports.at(next).enqueue_and_drain(pid, sim);
}

NodeId Node::choose_next_hop(Packet& p, Simulation& sim) const
{
  // just access our strategy's decision.
  return strategy->choose_next_hop(nid, p, sim);
}
