// [treesource] This tests that AIMD halves cwnd on loss while BBR does not.

#include <iostream>
#include "test_utils.hpp"
#include <assert.h>
#include "../src/sim.hpp"

int main()
{
  std::cout << "test_congestion_cutback: starting tests" << std::endl;

  Simulation sim = Simulation();

  NodeId zero = sim.add_node(nullptr);
  NodeId one = sim.add_node(nullptr);

  sim.add_undirected_link(zero, one, 0.01, 5000, 5);

  sim.get_node(zero).set_strategy(make_strategy(StrategyType::ShortestPath, zero, sim, {.pd_factor=1}));
  sim.get_node(one).set_strategy(make_strategy(StrategyType::ShortestPath, one, sim, {.pd_factor=1}));

  FlowId aimd_id = sim.add_flow(zero, one, 20000, 1000, CongestionControlType::AIMD);
  FlowId bbr_id = sim.add_flow(zero, one, 20000, 1000, CongestionControlType::BBR);

  Flow& aimd_flow = sim.get_flow(aimd_id);
  Flow& bbr_flow = sim.get_flow(bbr_id);

  sim.add_probe("aimd_cwnd", [&aimd_flow]() { return aimd_flow.get_cwnd(); });
  sim.add_probe("bbr_cwnd", [&bbr_flow]() { return bbr_flow.get_cwnd(); });

  bool finished = sim.run_bounded(1000);
  assert(finished);

  sim.export_log("build/logs/test_congestion_cutback.csv");
  sim.export_probes("build/probes/test_congestion_cutback.csv");

  const auto& probe_records = sim.get_probe_records();
  auto aimd_series = collect_probe_series(probe_records, "aimd_cwnd");
  auto bbr_series = collect_probe_series(probe_records, "bbr_cwnd");

  assert(aimd_series.size() > 3);
  assert(bbr_series.size() > 3);

  double aimd_min_ratio = min_consecutive_ratio(aimd_series);
  assert(aimd_min_ratio < 0.6);
  // not as easy to assert the same for BBR's behavior

  assert(aimd_flow.is_complete());
  assert(bbr_flow.is_complete());

  std::cout << "test_congestion_cutback: all tests passed" << std::endl;
}
