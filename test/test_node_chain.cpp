// [treesource] This tests that Events and Nodes correctly trigger each other.

#include <iostream>
#include "../src/sim.hpp"

int main()
{
  std::cout << "test_node_chain: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(0, sim));
  NodeId one = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(1, sim));
  NodeId two = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(2, sim));
  NodeId three = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(3, sim));

  sim.add_directed_link(zero, one, 1.0, 100);
  sim.add_directed_link(one, two, 2.0, 100);
  sim.add_directed_link(two, three, 3.0, 100);

  // sim.add_directed_link(one, zero, 4.0, 100); // tests randomness

  PacketId p_zero = sim.add_packet(zero, three, 5, 0.0);

  // so we have a chain 0 --> 1 --> 2 --> 3
  // these edges imply that we should just end up at 3, at t=6.0

  // send a "trigger" to Node 0
  sim.schedule(std::make_unique<Event>(sim.now(),
  [p_zero, zero, &sim] {sim.get_node(zero).receive_packet(p_zero, sim); }
    ));

  sim.run();

  std::cout << "test_node_chain: Simulation finished at t=" << sim.now() << std::endl;
}
