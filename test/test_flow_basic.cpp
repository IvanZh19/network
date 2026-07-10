// [treesource] This tests the behavior of Flows, acks, retransmits, etc.

#include <iostream>
#include <assert.h>
#include "../src/sim.hpp"

int main()
{
  std::cout << "test_flow_basic: starting tests" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(nullptr);
  NodeId one = sim.add_node(nullptr);

  sim.add_undirected_link(0, 1, 0.01, 1e6);

  sim.get_node(zero).set_strategy(make_strategy(StrategyType::ShortestPath, zero, sim, {.pd_factor=1}));
  sim.get_node(one).set_strategy(make_strategy(StrategyType::ShortestPath, one, sim, {.pd_factor=1}));

  FlowId fid = sim.add_flow(0, 1, 50000, 1000, CongestionControlType::BBR);

  sim.run();

  sim.export_log("build/logs/test_flow_basic.csv");

  const auto& records = sim.get_event_records();

  // looser assertions about the numbers of events
  int creates = 0, receives = 0, drops = 0, completes = 0;
  for (auto& r : records)
  {
    if (r.type == EventType::PacketCreate) creates++;
    if (r.type == EventType::PacketReceive) receives++;
    if (r.type == EventType::PacketDrop) drops++;
    if (r.type == EventType::FlowComplete) completes++;
  }

  assert(creates > 10);
  assert(drops == 0);
  assert(completes == 1);

  Flow& f = sim.get_flow(fid);
  assert(f.is_complete());

  // further asserts on cwnd?
}
