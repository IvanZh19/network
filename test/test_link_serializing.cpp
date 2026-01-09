// [treesource] This tests that Links serialize Packets properly.

#include <iostream>
#include "../src/sim.hpp"
#include <assert.h>

// NOTE: these tests are not as thorough as they could be, but there are a ton of parameters that could be split among the tests.

// Tests burst of Packets down a single link, with propagation delay. Link slower than Node.
void back_to_back(int num_packets, double prop_delay, double bandwidth)
{
  Simulation sim = Simulation();

  NodeId zero = sim.add_node(0.1, nullptr);
  NodeId one = sim.add_node(0.1, nullptr);

  sim.add_directed_link(zero, one, prop_delay, bandwidth);

  sim.get_node(zero).set_strategy(std::make_unique<ShortestPathStrategy>(zero, sim, 1, 0, 0));
  sim.get_node(one).set_strategy(std::make_unique<ShortestPathStrategy>(one, sim, 1, 0, 0));

  for (int i = 0; i < num_packets; i++)
  {
    PacketId p_i = sim.add_packet(zero, one, 1, 0);
    sim.get_node(zero).receive_packet(p_i, sim);
  }

  sim.run();

  // plus one because of propagation delay. params mean that transmission is 1 per.
  assert(sim.now() == (num_packets / bandwidth) + prop_delay);
}

// Test that Node send_rate is respected, with a fast Link (low pd)
// Assumes send_rate larger than 1/100 (1/bandwidth)
void fast_link(int num_packets, SimTime send_rate)
{
  Simulation sim = Simulation();
  NodeId zero = sim.add_node(send_rate, nullptr);
  NodeId one = sim.add_node(send_rate, nullptr);
  sim.add_directed_link(zero, one, 0, 100);

  sim.get_node(zero).set_strategy(std::make_unique<ShortestPathStrategy>(zero, sim, 1, 0, 0));
  sim.get_node(one).set_strategy(std::make_unique<ShortestPathStrategy>(one, sim, 1, 0, 0));

  for (int i = 0; i < num_packets; i++)
  {
    PacketId p_i = sim.add_packet(zero, one, 0, 0);
    sim.get_node(zero).receive_packet(p_i, sim);
  }

  sim.run();

  assert(sim.now() == (num_packets-1) * send_rate);
}

// Tests that a 3-Node chain of Links accepts a small burst of Packets.
void burst_chain(int num_packets)
{
  Simulation sim = Simulation();

  NodeId zero = sim.add_node(0.1, nullptr);
  NodeId one = sim.add_node(0.1, nullptr);
  NodeId two = sim.add_node(0.1, nullptr);

  sim.add_directed_link(zero, one, 1, 10);
  sim.add_directed_link(one, two, 1, 10);

  sim.get_node(zero).set_strategy(std::make_unique<RandomNeighborStrategy>(zero, sim));
  sim.get_node(one).set_strategy(std::make_unique<RandomNeighborStrategy>(one, sim));
  sim.get_node(two).set_strategy(std::make_unique<RandomNeighborStrategy>(two, sim));

  for (int i = 0; i < num_packets; i++)
  {
    PacketId p_i = sim.add_packet(zero, two, 1, 0);
    sim.get_node(zero).receive_packet(p_i, sim);
  }

  sim.run();

  // 10.0 bc of floating point...
  assert(std::abs(sim.now() - ((num_packets - 1)/10.0 + 2.2)) < 1e-9);

}

// Tests for a network 1 <-- 0 --> 2 that 0 can send bursts of Packets to both 1 and 2.
// This has slightly undesirable behavior, as one queue per Node means that Node will factor in a Link's next_free_time into its
// next time of sending, even though there might be another Link, i.e. the streams are not truly parallel but staggered.
void fork(int num_packets)
{
  Simulation sim = Simulation();

  NodeId zero = sim.add_node(0, nullptr);
  NodeId one = sim.add_node(0, nullptr);
  NodeId two = sim.add_node(0, nullptr);

  sim.add_directed_link(zero, one, 0, 10);
  sim.add_directed_link(zero, two, 0, 10);

  sim.get_node(zero).set_strategy(std::make_unique<ShortestPathStrategy>(zero, sim, 1, 0, 0));
  sim.get_node(one).set_strategy(std::make_unique<ShortestPathStrategy>(one, sim, 1, 0, 0));
  sim.get_node(two).set_strategy(std::make_unique<ShortestPathStrategy>(two, sim, 1, 0, 0));

  for (int i = 0; i < num_packets; i++)
  {
    PacketId p_i1 = sim.add_packet(zero, one, 1, 0);
    sim.get_node(zero).receive_packet(p_i1, sim);

    PacketId p_i2 = sim.add_packet(zero, two, 1, 0);
    sim.get_node(zero).receive_packet(p_i2, sim);
  }

  sim.run();

  std::cout << sim.now() << std::endl;

}

int main()
{
  /*
  There are a few critical Link behaviors that must be confirmed here.
  1. The time to travel across a Link is accurate w.r.t. bandwidth and propagation_delay for a single Packet.
  2. Nodes get the correct next_free_time and properly reserve a block of time on the Link.
    This represents that Packets are serialized, never sent in parallel but instead taking the earliest free time.

  These behaviors must be maintained under both high load (long burst of Packets) and varying send_rates (the cooldown with which Nodes
  try to push Packets onto the Link).
  */

  std::cout << "test_link_serializing: starting tests" << std::endl;

  for (int np = 5; np < 10; np ++)
  {
    for (double pd = 0; pd < 5; pd ++)
    {
      for (double bw = 1; bw < 3; bw ++)
      {
        back_to_back(np, pd, bw);
      }
    }
  }

  for (int np = 5; np < 10; np++)
  {
    for (SimTime sr = 0.5; sr < 5; sr = sr + 0.5)
    {
      fast_link(np, sr);
    }
  }

  for (int np = 5; np < 10; np ++)
  {
    burst_chain(np);
  }

  for (int np = 5; np < 10; np++)
  {
    fork(np);
  }





}
