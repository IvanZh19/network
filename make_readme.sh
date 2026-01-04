#!/bin/bash

# [treesource] This updates the README.

echo "This repo is for simulating network traffic." > README.md
echo '```' >> README.md
PYTHONIOENCODING=utf-8 python -m treesource >> README.md
echo '```' >> README.md
