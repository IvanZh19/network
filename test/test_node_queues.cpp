// [treesource] This tests that Node queues and send_rate limit Packets.

#include <iostream>
#include "../src/sim.hpp"

int main()
{
  std::cout << "test_node_queues: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(5.0, std::make_unique<RandomNeighborStrategy>(0, sim));
  NodeId one = sim.add_node(1.0, std::make_unique<RandomNeighborStrategy>(1, sim));
  NodeId two = sim.add_node(0, std::make_unique<RandomNeighborStrategy>(2, sim));
  NodeId three = sim.add_node(0, std::make_unique<RandomNeighborStrategy>(3, sim));

  sim.add_directed_link(zero, one, 1.0, 100);
  sim.add_directed_link(one, two, 2.0, 100);
  sim.add_directed_link(two, three, 3.0, 100);

  // sim.add_directed_link(one, zero, 4.0, 100); // tests randomness

  for (int i = 0; i < 10; i++)
  {
    PacketId p_i = sim.add_packet(zero, three, i, 0.0);
    // send a "trigger" to Node 0
    sim.schedule(std::make_unique<Event>(sim.now(),
  [p_i, zero, &sim] {sim.get_node(zero).receive_packet(p_i, sim); }
      ));
  }

  sim.run();

  sim.export_log("build/logs/test_node_queues.csv");
  sim.export_packets("build/packets/test_node_queues.csv");
  sim.export_network("build/networks/test_node_queues.json");

  std::cout << "test_node_queues: Simulation finished at t=" << sim.now() << std::endl;
}
