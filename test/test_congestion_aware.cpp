// [treesource] This tests that congestion influnces routing decisions.

#include <iostream>
#include "../src/sim.hpp"
#include <assert.h>

int main()
{
  std::cout << "test_congestion_aware: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(0.1, nullptr);
  NodeId one = sim.add_node(0.0, nullptr);
  NodeId two = sim.add_node(0.0, nullptr);
  NodeId three = sim.add_node(0.0, nullptr);
  NodeId four = sim.add_node(0.0, nullptr);

  sim.add_directed_link(zero, one, 1.0, 10);
  sim.add_directed_link(zero, two, 1.0, 10);
  sim.add_directed_link(zero, three, 1.0, 10);
  sim.add_directed_link(one, four, 1.0, 10);
  sim.add_directed_link(two, four, 1.0, 10);
  sim.add_directed_link(three, four, 1.0, 10);

  sim.get_node(zero).set_strategy(std::make_unique<CongestionAwareStrategy>(zero, sim, 0, 0, 0, 100));
  sim.get_node(one).set_strategy(std::make_unique<CongestionAwareStrategy>(one, sim, 0, 0, 0, 100));
  sim.get_node(two).set_strategy(std::make_unique<CongestionAwareStrategy>(two, sim, 0, 0, 0, 100));
  sim.get_node(three).set_strategy(std::make_unique<CongestionAwareStrategy>(three, sim, 0, 0, 0, 100));
  sim.get_node(four).set_strategy(std::make_unique<CongestionAwareStrategy>(four, sim, 0, 0, 0, 100));

  for (int i = 0; i < 50; i++)
  {
    PacketId p_i = sim.add_packet(zero, four, 100, 0.0);
    std::cout << p_i << std::endl;
    // TODO: look into this, might be slightly buggy here... with aliasing things
    sim.schedule(std::make_unique<Event>(sim.now(), [p_i, zero, &sim](){sim.get_node(zero).receive_packet(p_i, sim); }));
    // sim.get_node(zero).receive_packet(p_i, sim);
  }

  sim.print_packets();

  sim.run();

  sim.export_log("build/logs/test_congestion_aware.csv");
  sim.export_packets("build/packets/test_congestion_aware.csv");
  sim.export_network("build/networks/test_congestion_aware.json");



  std::cout << "test_congestion_aware: Simulation finished at t=" << sim.now() << std::endl;

}
