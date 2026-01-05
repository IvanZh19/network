This repo is for simulating network traffic.
```
.
├── src/
│   ├── event.hpp (This defines the basic Event struct.)
│   ├── main.cpp (This is the main entry point. It handles and runs Simulation instances.)
│   ├── node.cpp (This defines the basic behavior of Nodes.)
│   ├── node.hpp (This defines the Node class, agents that react to Events and handle Packets.)
│   ├── packet.hpp (This defines the POD Packet struct.)
│   ├── sim.cpp (This is the implementation of the Simulation class.)
│   ├── sim.hpp (This defines the interface for the Simulation class, to be used by main to interact with it)
│   └── sim_types.hpp (These are the relevant types, mostly for human legibility and sanity.)
├── make_readme.sh (This script updates the README.)
└── run.sh (This script makes and runs the compiled code.)

```
