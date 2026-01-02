#include <iostream>
#include <queue>
#include <functional>

#include "event.hpp"
#include "node.hpp"
#include "link.hpp"
#include "packet.hpp"

double current_time = 0.0;

int main()
{
  std::cout << "Sanity..." << std::endl;

  std::priority_queue<Event> event_queue;

  // add some events here before running loop

  event_queue.push(Event{1.0, []()
                         {
                           std::cout << "Event at time 1.0 executed." << std::endl;
                         }});

  event_queue.push(Event{0.5, []()
                         {
                           std::cout << "Event at time 0.5 executed." << std::endl;
                         }});

  event_queue.push(Event{2.0, []()
                         {
                           std::cout << "Event at time 2.0 executed." << std::endl;
                         }});

  // start simulating
  while (!event_queue.empty())
  {
    Event e = event_queue.top();
    event_queue.pop();

    current_time = e.time;
    e.action();
  }

  std::cout << "Simulation finished at time " << current_time << std::endl;

  return 0;
}
