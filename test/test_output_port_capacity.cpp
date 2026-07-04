// [treesource] This tests that OutputPort queues drop packets properly.

#include <iostream>
#include "test_utils.hpp"
#include <assert.h>
#include "../src/sim.hpp"

int main()
{
  std::cout << "test_output_port_capacity: starting tests" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(nullptr);
  NodeId one = sim.add_node(nullptr);

  sim.add_directed_link(zero, one, 1, 1, 2);

  sim.get_node(zero).set_strategy(make_strategy(StrategyType::ShortestPath, zero, sim, {.pd_factor=1}));
  sim.get_node(one).set_strategy(make_strategy(StrategyType::ShortestPath, one, sim, {.pd_factor=1}));

  std::vector<PacketId> pids;
  for (int i = 0; i < 4; i++)
  {
    PacketId p_i = sim.add_packet(zero, one, 1, 0);
    pids.push_back(p_i);
    sim.schedule(std::make_unique<Event>(0.0,
      [p_i, zero, &sim]() { sim.get_node(zero).receive_packet(p_i, sim); }
    ));
  }

  sim.run();

  sim.export_log("build/logs/test_output_port_capacity.csv");

  const auto& records = sim.get_event_records();
  SimTime tx_time = 1; // bw = 1, expect spacing of 1 between sends

  auto send_times = collect_event_times_by_pid(records, EventType::PacketSend, zero, one);
  auto recv_times = collect_event_times_by_pid(records, EventType::PacketReceive, zero, one);
  auto drop_times = collect_event_times_by_pid(records, EventType::PacketDrop, zero, one);

  assert(send_times.size() == 2);
  assert(recv_times.size() == 2);
  assert(drop_times.size() == 2);

  // TODO: more asserts here later
}
