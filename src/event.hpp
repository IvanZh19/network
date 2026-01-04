#pragma once
#include <functional>
#include <memory>
#include "sim_types.hpp"

class Simulation;

struct Event
{
  SimTime time;
  std::function<void()> action; // function called when popped off queue.

  Event(SimTime t, std::function<void()> act)
      : time(t), action(std::move(act)) {}

  virtual ~Event() = default; // destructor for polymorphism stuffs
};

struct EventCompare
{
  bool operator()(const std::unique_ptr<Event> &a, const std::unique_ptr<Event> &b) const
  {
    return a->time > b->time;
  }
};
