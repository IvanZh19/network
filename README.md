# network

This repo is for simulating network traffic.

It centers around the Simulation class.
Some features are:
- Use randomly generated topologies and Packet distributions
- Model Node routing strategies and Link constraints
- Log results for analysis
- Visualize network topology and Simulation results

For analysis, be sure to download the requirements.
The analysis scripts have some simple visualization and metrics capabilities.

### Example Visualizations

Random Neighbor
![Random Neighbor Strategy](build/animations/animation2.gif)

Shortest Path
![Shortest Path Strategy](build/animations/animation.gif)

### What's in this repo?

```
.
├── analysis/
│   ├── analyze.py (This script computes and plots metrics from simulation results. May switch to NumPy.)
│   ├── requirements.txt (These are the requirements for the analysis scripts.)
│   └── visualize.py (This script visualizes simulation results with some basic animation.)
├── src/
│   ├── event.hpp (This defines the basic Event struct.)
│   ├── link.hpp (This defines the Link class, representing the state of network edges.)
│   ├── logger.hpp (This defines the interface for Logger, for recording simulation behavior.)
│   ├── main.cpp (This is the main entry point. It handles and runs Simulation instances.)
│   ├── net_desc.hpp (This defines the structs used to aid the NetworkGenerator class.)
│   ├── net_generator.cpp (This implements the NetworkGenerator class.)
│   ├── net_generator.hpp (This is the NetworkGenerator class, meant to help construct network info that helps set up a Simulation.)
│   ├── node.cpp (This defines the behavior of Nodes.)
│   ├── node.hpp (This defines the Node class, agents that react to Events and handle Packets.)
│   ├── packet.hpp (This defines the Packet struct.)
│   ├── sim.cpp (This is the implementation of the Simulation class.)
│   ├── sim.hpp (This defines the interface for the Simulation class, to be used by main to interact with it)
│   ├── sim_types.hpp (These are the relevant types, mostly for human legibility and sanity.)
│   ├── strategy.cpp (This implements the Strategy class and its subclasses.)
│   └── strategy.hpp (This defines the interface for the Strategy class and subclasses, used by Nodes to make routing decisions.)
├── test/
│   ├── test_congestion_aware.cpp (This tests that congestion influnces routing decisions.)
│   ├── test_event_queue.cpp (This tests that the event queue works with basic Events.)
│   ├── test_link_serializing.cpp (This tests that Links serialize Packets properly.)
│   ├── test_net_generator.cpp (This tests that basic network generation works as expected.)
│   ├── test_network.cpp (This tests that networks can be created.)
│   ├── test_node_chain.cpp (This tests that Events and Nodes correctly trigger each other.)
│   ├── test_node_queues.cpp (This tests that Node queues and send_rate limit Packets.)
│   └── test_shortest_paths.cpp (This tests that Dijkstra's runs correctly.)
├── Makefile (This is the Makefile for building with main.cpp)
├── make_readme.sh (This script updates the README.)
├── run.sh (This script makes and runs the compiled code.)
└── test.sh (This script runs all the tests in the tests folder.)
```
