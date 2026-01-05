// [treesource] This is the main entry point. It handles and runs Simulation instances.

#include <iostream>
#include "sim.hpp"

int main()
{
  std::cout << "Main: starting Simulation" << std::endl;

  Simulation sim = Simulation();

  sim.schedule(std::make_unique<Event>(5.0, [&sim](){
    std::cout << "Event at t=5.0, now=" << sim.now() << "\n";
  }));

  sim.schedule(std::make_unique<Event>(0.5, [&sim](){
    std::cout << "Event at t=0.5, now=" << sim.now() << "\n";
  }));

  sim.schedule(std::make_unique<Event>(20.0, [&sim](){
    std::cout << "Event at t=20.0, now=" << sim.now() << "\n";
  }));

  sim.schedule(std::make_unique<Event>(10.0, [&sim](){
    std::cout << "Event at t=10.0, now=" << sim.now() << "\n";
  }));

  while (!sim.done())
  {
    sim.step();
  }

  std::cout << "Main: Simulation finished at t=" << sim.now() << std::endl;
}
