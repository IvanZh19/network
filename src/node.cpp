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
    if (p.is_ack)
    {
      sim.get_flow(p.flow_id).on_ack(p, sim);
    }
    else if (p.flow_id != INVALID_FLOW_ID)
    {
      PacketId ack_pid = sim.add_packet(nid, p.src, ACK_SIZE, sim.now(), p.flow_id, true, p.id);
      originate_packet(ack_pid, sim);
    }
    return; // do nothing, as before
  }

  NodeId next = choose_next_hop(p, sim);
  ports.at(next).enqueue_and_drain(pid, sim);
}

void Node::originate_packet(PacketId pid, Simulation& sim)
{
  Packet& p = sim.get_packet(pid);
  sim.log({sim.now(), EventType::PacketCreate, p.src, p.dst, p.id});
  p.owner = nid;

  if (p.dst == nid) return; // edge case, should not normally happen

  NodeId next = choose_next_hop(p, sim);
  ports.at(next).enqueue_and_drain(pid, sim);
}

NodeId Node::choose_next_hop(Packet& p, Simulation& sim) const
{
  // just access our strategy's decision.
  return strategy->choose_next_hop(nid, p, sim);
}
