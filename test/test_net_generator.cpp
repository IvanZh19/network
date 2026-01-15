// [treesource] This tests that basic network generation works as expected.

#include <iostream>
#include "../src/sim.hpp"
#include "../src/net_generator.hpp"
#include <assert.h>

int main()
{
  std::cout << "test_net_generator: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  NetworkGenerator net_gen = NetworkGenerator(67);

  NetworkDesc net_desc = net_gen.random_connected(10, 20, 5, 10, 1, 100);

  net_gen.add_uniform_random_packets(net_desc, 20, 1, 10, 0, 0);

  sim.initialize_topology(net_desc);

  sim.initialize_strategies(StrategyPick::RandomNeighborStrategy_, {}, sim);

  sim.print_nodes();
  sim.print_adj_list();
  sim.print_packets();

  sim.schedule_all_packets(sim);

  sim.run();

  sim.dump_csv("build/logs/test_net_generator.csv");

  std::cout << "test_net_generator: Simulation finished at t=" << sim.now() << std::endl;

}
