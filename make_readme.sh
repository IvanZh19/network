#!/bin/bash

# [treesource] This script updates the README.

cat <<EOT > README.md
# network

This repo is for simulating network traffic.

It centers around the Simulation class.
Some features are:
- Use randomly generated topologies and Packet distributions
- Model Node routing strategies and Link constraints
- Step through Events in the Simulation
- Log results for analysis

For analysis, be sure to download the requirements.
The analysis scripts have some simple visualization and metrics capabilities.

\`\`\`
$(PYTHONIOENCODING=utf-8 python -m treesource)
\`\`\`
EOT
