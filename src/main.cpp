// [treesource] This is the main entry point. It handles and runs Simulation instances.

#include <iostream>
#include <queue>
#include <functional>

#include "event.hpp"
#include "node.hpp"
#include "packet.hpp"
#include "sim.hpp"

int main()
{
  std::cout << "Main: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  sim.schedule(std::make_unique<Event>(1.0, [&sim](){
    std::cout << "Event at t=1.0, now=" << sim.now() << "\n";
  }));

  sim.schedule(std::make_unique<Event>(0.5, [&sim](){
    std::cout << "Event at t=0.5, now=" << sim.now() << "\n";
  }));

  sim.schedule(std::make_unique<Event>(2.0, [&sim](){
    std::cout << "Event at t=2.0, now=" << sim.now() << "\n";
  }));

  while (!sim.done())
  {
    sim.step();
  }

  std::cout << "Simulation finished at t=" << sim.now() << std::endl;
}
