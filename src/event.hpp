#pragma once
#include <functional>

struct Event
{
  double time;
  std::function<void()> action;

  bool operator<(const Event &other) const
  {
    return time > other.time;
  }
};
