# [treesource] This script visualizes simulation results. Currently does not support step through.

import json
import networkx as nx
import matplotlib.pyplot as plt

# specify the filename of the network json to visualize here.
network_json = "build/networks/test_net_generator.json"

def load_network(path):
  with open(path) as f:
    data = json.load(f)

  G = nx.DiGraph()

  for n in data["nodes"]:
    G.add_node(n["nid"], send_rate=n["send_rate"])

  for l in data["links"]:
    G.add_edge(
      l["from"],
      l["to"],
      propagation_delay=l["propagation_delay"],
      bandwidth=l["bandwidth"]
    )

  return G

def draw_network(G):
  pos = nx.spring_layout(G) # note this is a random layout.
  nx.draw(
    G,
    pos,
    with_labels=True,
    node_size=600
  )
  plt.title("Network")
  plt.show()

def main():
  G = load_network(network_json)
  draw_network(G)

if __name__ == "__main__":
  main()
