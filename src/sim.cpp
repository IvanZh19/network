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

Node &Simulation::node(NodeId id)
{
  // nodes.push_back(Node())
}

Packet &Simulation::packet(PacketId id)
{
  // packets.push_back(Packet &&Val)
}
