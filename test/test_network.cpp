// [treesource] This tests that networks can be created.

#include <iostream>
#include "../src/sim.hpp"
#include <assert.h>

int main()
{
  std::cout << "test_network: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  // TEST NODES.

  NodeId zero = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(0, sim));
  NodeId one = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(1, sim));
  NodeId two = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(2, sim));

  assert(zero==0);
  assert(one==1);
  assert(two==2);

  Node& n_zero = sim.get_node(zero);
  Node& n_one = sim.get_node(one);
  Node& n_two = sim.get_node(two);

  assert(n_zero.id()==0);
  assert(n_one.id()==1);
  assert(n_two.id()==2);

  sim.print_nodes();

  // TEST EDGES

  sim.add_directed_link(zero, one, 1.0, 100);
  sim.add_directed_link(one, zero, 2.0, 100);
  sim.add_undirected_link(zero, two, 3.0, 100);

  sim.print_adj_list();

  // TODO: add way to access edge weights and then assert. unneeded for now.

  // TEST PACKETS

  sim.add_packet(zero, one, 10, 0.0);
  sim.add_packet(one, two, 5, 1.0);

  sim.print_packets();

  std::cout << "test_network: Simulation finished at t=" << sim.now() << std::endl;
}
