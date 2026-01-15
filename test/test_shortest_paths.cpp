// [treesource] This tests that Dijkstra's runs correctly.

#include <iostream>
#include "../src/sim.hpp"
#include <assert.h>

int main()
{
  std::cout << "test_shortest_paths: starting Simulation 1" << std::endl;

  // SIM1 TESTS PURE DIJKSTRA'S, NO BANDWIDTH FACTORED IN.

  Simulation sim1 = Simulation();

  NodeId zero1 = sim1.add_node(0.0, nullptr);
  NodeId one1 = sim1.add_node(0.0, nullptr);
  NodeId two1 = sim1.add_node(0.0, nullptr);
  NodeId three1 = sim1.add_node(0.0, nullptr);

  // two paths 0-1-3, 0-2-3
  sim1.add_directed_link(zero1, one1, 1.0, 100);
  sim1.add_directed_link(one1, three1, 1.0, 100);
  sim1.add_directed_link(zero1, two1, 5.0, 100);
  sim1.add_directed_link(two1, three1, 1.0, 100);

  // assign strategies after making network. A bit clunky, should fix later.
  sim1.get_node(zero1).set_strategy(std::make_unique<ShortestPathStrategy>(zero1, sim1, 1, 0, 0));
  sim1.get_node(one1).set_strategy(std::make_unique<ShortestPathStrategy>(one1, sim1, 1, 0, 0));
  sim1.get_node(two1).set_strategy(std::make_unique<ShortestPathStrategy>(two1, sim1, 1, 0, 0));
  sim1.get_node(three1).set_strategy(std::make_unique<ShortestPathStrategy>(three1, sim1, 1, 0, 0));

  // single packet from 0 to 3
  PacketId pid1 = sim1.add_packet(zero1, three1, 1, 0.0);

  // ok a bit jank as we should follow Event convention but...
  sim1.get_node(zero1).receive_packet(pid1, sim1);
  sim1.run();

  sim1.export_log("build/logs/test_shortest_paths_1.csv");
  sim1.export_packets("build/packets/test_shortest_paths_1.csv");
  sim1.export_network("build/networks/test_shortest_paths_1.json");

  assert(sim1.now() == 2.02);

  std::cout << "test_shortest_paths: Simulation 1 finished at t=" << sim1.now() << std::endl;

  std::cout << "test_shortest_paths: starting Simulation 2" << std::endl;

  // SIM2 TESTS PURE BANDWIDTH

  Simulation sim2 = Simulation();

  NodeId zero2 = sim2.add_node(0.0, nullptr);
  NodeId one2 = sim2.add_node(0.0, nullptr);
  NodeId two2 = sim2.add_node(0.0, nullptr);
  NodeId three2 = sim2.add_node(0.0, nullptr);

  // two paths 0-1-3, 0-2-3, should choose 0-1-3 because only bandwidth
  sim2.add_directed_link(zero2, one2, 1.0, 100);
  sim2.add_directed_link(one2, three2, 1.0, 100);
  sim2.add_directed_link(zero2, two2, 0, 1);
  sim2.add_directed_link(two2, three2, 0, 1);

  sim2.get_node(zero2).set_strategy(std::make_unique<ShortestPathStrategy>(zero2, sim2, 0, 1, 0));
  sim2.get_node(one2).set_strategy(std::make_unique<ShortestPathStrategy>(one2, sim2, 0, 1, 0));
  sim2.get_node(two2).set_strategy(std::make_unique<ShortestPathStrategy>(two2, sim2, 0, 1, 0));
  sim2.get_node(three2).set_strategy(std::make_unique<ShortestPathStrategy>(three2, sim2, 0, 1, 0));

  PacketId pid2 = sim2.add_packet(zero2, three2, 1, 0.0);

  // jank... technically forcing this without matching Event but.
  sim2.get_node(zero2).receive_packet(pid2, sim2);
  sim2.run();

  sim2.export_log("build/logs/test_shortest_paths_2.csv");
  sim2.export_packets("build/packets/test_shortest_paths_2.csv");
  sim2.export_network("build/networks/test_shortest_paths_2.json");

  assert(sim2.now() == 2.02);

  std::cout << "test_shortest_paths: Simulation 2 finished at t=" << sim2.now() << std::endl;

  std::cout << "test_shortest_paths: starting Simulation 3" << std::endl;

  // SIM3 TESTS A MIX OF BOTH, BUT NO FIXED COST

  Simulation sim3 = Simulation();

  NodeId zero3 = sim3.add_node(0.0, nullptr);
  NodeId one3 = sim3.add_node(0.0, nullptr);
  NodeId two3 = sim3.add_node(0.0, nullptr);
  NodeId three3 = sim3.add_node(0.0, nullptr);

  // (1 + 1/100) * 2 is smaller than (0.5 + 1/1) * 2
  sim3.add_directed_link(zero3, one3, 1.0, 100);
  sim3.add_directed_link(one3, three3, 1.0, 100);
  sim3.add_directed_link(zero3, two3, 0.5, 1);
  sim3.add_directed_link(two3, three3, 0.5, 1);

  sim3.get_node(zero3).set_strategy(std::make_unique<ShortestPathStrategy>(zero3, sim3, 1, 1, 0));
  sim3.get_node(one3).set_strategy(std::make_unique<ShortestPathStrategy>(one3, sim3, 1, 1, 0));
  sim3.get_node(two3).set_strategy(std::make_unique<ShortestPathStrategy>(two3, sim3, 1, 1, 0));
  sim3.get_node(three3).set_strategy(std::make_unique<ShortestPathStrategy>(three3, sim3, 1, 1, 0));

  PacketId pid3 = sim3.add_packet(zero3, three3, 1, 0.0);

  // jank
  sim3.get_node(zero3).receive_packet(pid3, sim3);
  sim3.run();

  sim3.export_log("build/logs/test_shortest_paths_3.csv");
  sim3.export_packets("build/packets/test_shortest_paths_3.csv");
  sim3.export_network("build/networks/test_shortest_paths_3.json");

  assert(sim3.now() == 2.02);

  std::cout << "test_shortest_paths: Simulation 3 finished at t=" << sim3.now() << std::endl;

  std::cout << "test_shortest_paths: starting Simulation 4" << std::endl;

  // SIM4 TESTS FIXED HOP COUNT, SO SHORTEST HOPS PREFERRED

  Simulation sim4 = Simulation();

  NodeId zero4 = sim4.add_node(0.0, nullptr);
  NodeId one4 = sim4.add_node(0.0, nullptr);
  NodeId two4 = sim4.add_node(0.0, nullptr);
  NodeId three4 = sim4.add_node(0.0, nullptr);

  // 1-3 has delay 1000, but is just one hop
  sim4.add_directed_link(zero4, one4, 1.0, 100);
  sim4.add_directed_link(one4, two4, 1.0, 100);
  sim4.add_directed_link(two4, three4, 1.0, 100);
  sim4.add_directed_link(one4, three4, 1000, 100);

  // fixed hop count strategy
  sim4.get_node(zero4).set_strategy(std::make_unique<ShortestPathStrategy>(zero4, sim4, 0, 0, 1));
  sim4.get_node(one4).set_strategy(std::make_unique<ShortestPathStrategy>(one4, sim4, 0, 0, 1));
  sim4.get_node(two4).set_strategy(std::make_unique<ShortestPathStrategy>(two4, sim4, 0, 0, 1));
  sim4.get_node(three4).set_strategy(std::make_unique<ShortestPathStrategy>(three4, sim4, 0, 0, 1));

  PacketId pid4 = sim4.add_packet(zero4, three4, 1, 0.0);

  sim4.get_node(zero4).receive_packet(pid4, sim4);
  sim4.run();

  sim4.export_log("build/logs/test_shortest_paths_4.csv");
  sim4.export_packets("build/packets/test_shortest_paths_4.csv");
  sim4.export_network("build/networks/test_shortest_paths_4.json");

  assert(sim4.now() == 1001.02);

  std::cout << "test_shortest_paths: Simulation 4 finished at t=" << sim4.now() << std::endl;

  std::cout << "test_shortest_paths: All Simulations finished!" << std::endl;

}
