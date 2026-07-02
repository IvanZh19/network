// [treesource] Util functions for tests, relies on Logger

#pragma once
#include <vector>
#include <map>
#include <cmath>
#include <assert.h>
#include <algorithm>
#include "../src/logger.hpp"

inline bool approx(double a, double b, double eps = 1e-9)
{
  return std::abs(a - b) < eps;
}

inline const EventRecord& find_event(const std::vector<EventRecord>& records, EventType type, PacketId pid)
{
  const EventRecord* found = nullptr;
  int count = 0;
  for (const auto& er : records)
  {
    if (er.type == type && er.pid == pid)
    {
      found = &er;
      count++;
    }
  }
  assert(count == 1);
  return *found;
}

inline std::vector<SimTime> collect_event_times(const std::vector<EventRecord>& records, EventType type, NodeId nid1, NodeId nid2)
{
  std::vector<SimTime> times;
  for (const auto& er : records)
  {
    if (er.type == type && er.nid1 == nid1 && er.nid2 == nid2)
    {
      times.push_back(er.time);
    }
  }
  std::sort(times.begin(), times.end());
  return times;
}

inline std::map<PacketId, SimTime> collect_event_times_by_pid(const std::vector<EventRecord>& records, EventType type, NodeId nid1, NodeId nid2)
{
  std::map<PacketId, SimTime> times;
  for (const auto& er : records)
  {
    if (er.type == type && er.nid1 == nid1 && er.nid2 == nid2)
    {
      times[er.pid] = er.time;
    }
  }
  return times;
}
