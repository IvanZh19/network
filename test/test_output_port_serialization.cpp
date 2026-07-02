// [treesource] This tests that OutputPorts preserve serialization properties.

#include <iostream>
#include <assert.h>
#include <map>
#include "test_utils.hpp"
#include "../src/logger.hpp"
#include "../src/sim.hpp"

void back_to_back_per_packet(int num_packets, SimTime prop_delay, double bandwidth)
{
  Simulation sim = Simulation();

  NodeId zero = sim.add_node(nullptr);
  NodeId one = sim.add_node(nullptr);

  sim.add_directed_link(zero, one, prop_delay, bandwidth);

  sim.get_node(zero).set_strategy(std::make_unique<ShortestPathStrategy>(zero, sim, 1, 0, 0));
  sim.get_node(one).set_strategy(std::make_unique<ShortestPathStrategy>(one, sim, 1, 0, 0));

  std::vector<PacketId> pids;
  for (int i = 0; i < num_packets; i++)
  {
    PacketId p_i = sim.add_packet(zero, one, 1, 0);
    pids.push_back(p_i);
    // sim.get_node(zero).receive_packet(p_i, sim);
    sim.schedule(std::make_unique<Event>(0.0,
      [p_i, zero, &sim]() { sim.get_node(zero).receive_packet(p_i, sim); }
    ));
  }

  sim.run();

  sim.export_log("build/logs/test_output_port_serialization_back.csv");

  const auto& records = sim.get_event_records();
  SimTime tx_time = 1.0 / bandwidth;

  auto send_times = collect_event_times_by_pid(records, EventType::PacketSend, zero, one);
  auto recv_times = collect_event_times_by_pid(records, EventType::PacketReceive, zero, one);

  assert(send_times.size() == num_packets);
  assert(recv_times.size() == num_packets);

  auto sorted_sends = collect_event_times(records, EventType::PacketSend, zero, one);
  auto sorted_recvs = collect_event_times(records, EventType::PacketReceive, zero, one);

  // assert back to back spacing
  for (int i = 0; i < num_packets; i++)
  {
    assert(approx(sorted_sends[i], i * tx_time));
    assert(approx(sorted_recvs[i], i * tx_time + tx_time + prop_delay));
  }

  // assert transit time per packet
  for (PacketId pid : pids)
  {
    assert(approx(recv_times[pid] - send_times[pid], tx_time + prop_delay));
  }
}

// fork test, confirms packets to neighbor "one" and "two" interleave correctly
// within each port, should be back to back on their respective links
// the two ports timings should not interfere with each other
void fork_per_packet(int num_packets)
{
  Simulation sim = Simulation();

  NodeId zero = sim.add_node(nullptr);
  NodeId one = sim.add_node(nullptr);
  NodeId two = sim.add_node(nullptr);

  SimTime prop_delay = 0;
  double bandwidth = 10;
  SimTime tx_time = 1.0 / bandwidth;

  sim.add_directed_link(zero, one, prop_delay, bandwidth);
  sim.add_directed_link(zero, two, prop_delay, bandwidth);

  sim.get_node(zero).set_strategy(std::make_unique<ShortestPathStrategy>(zero, sim, 1, 0, 0));
  sim.get_node(one).set_strategy(std::make_unique<ShortestPathStrategy>(one, sim, 1, 0, 0));
  sim.get_node(two).set_strategy(std::make_unique<ShortestPathStrategy>(two, sim, 1, 0, 0));

  std::vector<PacketId> to_one, to_two;
  for (int i = 0; i < num_packets; i++)
  {
    PacketId p_i1 = sim.add_packet(zero, one, 1, 0);
    to_one.push_back(p_i1);
    sim.schedule(std::make_unique<Event>(0.0,
      [p_i1, zero, &sim]() { sim.get_node(zero).receive_packet(p_i1, sim); }
    ));

    PacketId p_i2 = sim.add_packet(zero, two, 1, 0);
    to_two.push_back(p_i2);
    sim.schedule(std::make_unique<Event>(0.0,
      [p_i2, zero, &sim]() { sim.get_node(zero).receive_packet(p_i2, sim); }
    ));
  }

  sim.run();

  sim.export_log("build/logs/test_output_port_serialization_fork.csv");

  const auto& records = sim.get_event_records();

  auto sends_to_one = collect_event_times(records, EventType::PacketSend, zero, one);
  auto sends_to_two = collect_event_times(records, EventType::PacketSend, zero, two);
  auto send_times_one = collect_event_times_by_pid(records, EventType::PacketSend, zero, one);
  auto send_times_two = collect_event_times_by_pid(records, EventType::PacketSend, zero, two);
  auto recv_times_one = collect_event_times_by_pid(records, EventType::PacketReceive, zero, one);
  auto recv_times_two = collect_event_times_by_pid(records, EventType::PacketReceive, zero, two);

  assert(sends_to_one.size() == num_packets);
  assert(sends_to_two.size() == num_packets);

  // both streams shouold be back to back at same spacing, independently
  for (int i = 0; i < num_packets; i++)
  {
    assert(approx(sends_to_one[i], i * tx_time));
    assert(approx(sends_to_two[i], i * tx_time));
  }

  for (PacketId pid : to_one)
  {
    assert(approx(recv_times_one[pid] - send_times_one[pid], tx_time + prop_delay));
  }
  for (PacketId pid : to_two)
  {
    assert(approx(recv_times_two[pid] - send_times_two[pid], tx_time + prop_delay));
  }
}

int main()
{
  std::cout << "test_output_port_serialization: starting tests" << std::endl;

  for (int np = 5; np < 10; np++)
  {
    for (double pd = 0; pd < 5; pd++)
    {
      for (double bw = 1; bw < 3; bw++)
      {
        // this test writes log once per run, so only last run is actually saved
        back_to_back_per_packet(np, pd, bw);
      }
    }
  }

  for (int np = 5; np < 10; np++)
  {
    // this test writes log once per run, so only last run is saved
    fork_per_packet(np);
  }

  std::cout << "test_output_port_serialization: all tests passed" << std::endl;
}
