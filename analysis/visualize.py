# [treesource] This script visualizes simulation results with some basic animation.

import argparse
import csv
import json
import math
import os
from collections import defaultdict

import numpy as np
import matplotlib.pyplot as plt
import networkx as nx
from matplotlib.animation import FuncAnimation

# specify files to use here.
DEFAULT_NETWORK = "build/networks/test_net_generator.json"
DEFAULT_EVENTS = "build/logs/test_net_generator.csv"
DEFAULT_PACKETS = "build/packets/test_net_generator.csv"

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

def load_events(path):
  events = []
  with open(path, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
      events.append({
        "time": float(row["time"]),
        "event_type": row["event_type"],
        "nid1": int(row["nid1"]),
        "nid2": int(row["nid2"]),
        "pid": int(row["pid"])
      })
  return events

def load_packets(path):
  packets = {}
  if not os.path.exists(path):
    return packets
  with open(path, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
      pid = int(row["id"])
      packets[pid] = row
  return packets

def build_movements(events, packets):
  # for each PacketSend, try to find the corresponding PacketReceive (matching pid,nid1,nid2)
  movements = []
  n = len(events)
  for i, e in enumerate(events):
    if e["event_type"] == "PacketSend":
      pid = e["pid"]
      src = e["nid1"]
      dst = e["nid2"]
      t0 = e["time"]

      # find next matching PacketReceive
      t1 = None
      for j in range(i + 1, n):
        r = events[j]
        if r["event_type"] == "PacketReceive" and r["pid"] == pid and r["nid1"] == src and r["nid2"] == dst:
          t1 = r["time"]
          break

      if t1 is None:
        continue

      if t1 <= t0:
        continue

      # attach packet metadata
      pkt_info = packets.get(pid, {})
      movements.append({"pid": pid, "src": src, "dst": dst, "t0": t0, "t1": t1, "pkt": pkt_info})

  return movements

def animate_network(network_file=DEFAULT_NETWORK, events_file=DEFAULT_EVENTS, packets_file=DEFAULT_PACKETS, time_step=0.5, save_file=None, test=False, speed=1.0):
  G = load_network(network_file)
  events = load_events(events_file)
  packets = load_packets(packets_file)

  movements = build_movements(events, packets)

  if test:
    print(f"Loaded network: {len(G.nodes())} nodes, {len(G.edges())} edges")
    print(f"Loaded events: {len(events)}")
    print(f"Built movements: {len(movements)}")
    # show a few movements
    for m in movements[:10]:
      print(m)
    return 0

  if not movements:
    print("No packet movements found in log; drawing static network.")

  pos = nx.spring_layout(G, seed=42)

  fig, ax = plt.subplots(figsize=(15, 6))
  ax.set_title("Network Simulation Animation")
  sidebar_text = fig.text(0, 0.9, "", transform=fig.transFigure, fontsize=8, family="monospace", verticalalignment="top")

  nx.draw_networkx_edges(G, pos, ax=ax, arrows=True, alpha=0.3)
  nx.draw_networkx_labels(G, pos, ax=ax)

  t_min = min((m["t0"] for m in movements), default=0.0)
  t_max = max((m["t1"] for m in movements), default=1.0)

  # build discrete frames
  duration = max(1e-6, t_max - t_min)
  num_frames = int(min(2000, math.ceil(duration / time_step))) + 1
  times = [t_min + i * duration / max(1, num_frames - 1) for i in range(num_frames)]

  # packet scatter
  scat = ax.scatter([], [], s=80)

  # color map for packet ids
  cmap = plt.get_cmap("tab20")

  def frame_at_time(t):
    xs = []
    ys = []
    cs = []
    for m in movements:
      if m["t0"] <= t <= m["t1"]:
        frac = (t - m["t0"]) / (m["t1"] - m["t0"]) if m["t1"] > m["t0"] else 1.0
        p0 = pos[m["src"]]
        p1 = pos[m["dst"]]
        x = p0[0] * (1 - frac) + p1[0] * frac
        y = p0[1] * (1 - frac) + p1[1] * frac
        xs.append(x)
        ys.append(y)
        cs.append(cmap((m["pid"] % 20) / 20.0))
    return xs, ys, cs

  time_text = ax.text(0.02, 0.95, "", transform=ax.transAxes)

  def init():
    scat.set_offsets(np.empty((0, 2)))
    time_text.set_text("")
    return scat, time_text

  def update(frame_idx):
    t = times[frame_idx]
    xs, ys, cs = frame_at_time(t)
    if xs:
      offsets = np.column_stack([xs, ys])
      scat.set_offsets(offsets)
      scat.set_color(cs)
    else:
      scat.set_offsets(np.empty((0, 2)))
    time_text.set_text(f"time: {t:.2f}")

    # update sidebar with in-transit packets
    in_transit = []
    for m in movements:
      if m["t0"] <= t <= m["t1"]:
        pkt = m["pkt"]
        in_transit.append({
          "pid": m["pid"],
          "src": pkt.get("src", "?"),
          "dst": pkt.get("dst", "?"),
          "owner": pkt.get("owner", "?"),
          "size": pkt.get("packet_size", "?"),
          "created": pkt.get("creation_time", "?")
        })

    # sidebar stuffs
    max_packets_shown = 15
    sidebar_lines = ["In Transit Packets:"]
    if in_transit:
      sidebar_lines.append("id|src→dst|owner|sz|created")
      for i, p in enumerate(in_transit):
        if i >= max_packets_shown:
          sidebar_lines.append(f"(+{len(in_transit) - max_packets_shown} more)")
          break
        line = f"{p['pid']}|{p['src']}→{p['dst']}|{p['owner']}|{p['size']}|{p['created']}"
        sidebar_lines.append(line)
    else:
      sidebar_lines.append("(none)")

    sidebar_text.set_text("\n".join(sidebar_lines))
    return scat, time_text

  anim = FuncAnimation(fig, update, frames=len(times), init_func=init, blit=False, interval=int(50 / speed))

  if save_file:
    # try to save,  must have ffmpeg
    ext = os.path.splitext(save_file)[1].lower()
    print(f"Saving animation to {save_file} (this may take a while)...")
    if ext in [".mp4", ".mov"]:
      anim.save(save_file, writer="ffmpeg", fps=30)
    elif ext in [".gif"]:
      anim.save(save_file, writer="ffmpeg", fps=20)
    else:
      anim.save(save_file, writer="ffmpeg", fps=20)
    print("Saved.")
  else:
    plt.show()

def main():
  parser = argparse.ArgumentParser(description="Visualize network simulation logs (animated)")
  parser.add_argument("--network", default=DEFAULT_NETWORK)
  parser.add_argument("--events", default=DEFAULT_EVENTS)
  parser.add_argument("--packets", default=DEFAULT_PACKETS)
  parser.add_argument("--step", type=float, default=0.5, help="approximate timestep (seconds) between frames")
  parser.add_argument("--save", default=None, help="save animation to file (mp4/gif)")
  parser.add_argument("--speed", type=float, default=1.0, help="animation speed multiplier (0.5=slow, 2.0=fast)")
  parser.add_argument("--test", action="store_true", help="run quick load/test and exit")
  args = parser.parse_args()

  return animate_network(network_file=args.network, events_file=args.events, packets_file=args.packets, time_step=args.step, save_file=args.save, test=args.test, speed=args.speed)

if __name__ == "__main__":
  main()
