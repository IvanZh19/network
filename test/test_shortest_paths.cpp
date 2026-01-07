// [treesource] This tests that Dijkstra's runs correctly.

#include <iostream>
#include "../src/sim.hpp"

int main()
{
  std::cout << "test_shortest_paths: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(0.0, nullptr);
  NodeId one = sim.add_node(0.0, nullptr);
  NodeId two = sim.add_node(0.0, nullptr);
  NodeId three = sim.add_node(0.0, nullptr);

  // two paths 0-1-3, 0-2-3
  sim.add_directed_link(zero, one, 1.0, 100);
  sim.add_directed_link(one, three, 1.0, 100);
  sim.add_directed_link(zero, two, 5.0, 100);
  sim.add_directed_link(two, three, 1.0, 100);

  // assign strategies after making network. A bit clunky, should fix later.
  sim.get_node(zero).set_strategy(std::make_unique<ShortestPathStrategy>(zero, sim));
  sim.get_node(one).set_strategy(std::make_unique<ShortestPathStrategy>(one, sim));
  sim.get_node(two).set_strategy(std::make_unique<ShortestPathStrategy>(two, sim));
  sim.get_node(three).set_strategy(std::make_unique<ShortestPathStrategy>(three, sim));

  // single packet from 0 to 3
  PacketId pid = sim.add_packet(zero, three, 1, 0.0);

  // ok a bit jank as we should follow Event convention but...
  sim.get_node(zero).receive_packet(pid, sim);

  sim.run();

  std::cout << "test_shortest_paths: Simulation finished at t=" << sim.now() << std::endl;
}
