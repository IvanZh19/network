#!/bin/bash

# [treesource] This script updates the README.

cat <<EOT > README.md
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

\`\`\`
$(PYTHONIOENCODING=utf-8 python -m treesource)
\`\`\`
EOT
