# [treesource] This script computes and plots metrics from simulation results. May switch to NumPy.

import csv
import json
import sys
from collections import defaultdict
import matplotlib.pyplot as plt
import statistics as stat

def load_packets(path):
  packets = {}
  with open(path, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
      pid = int(row["id"])
      packets[pid] = {
        "src": int(row["src"]),
        "dst": int(row["dst"]),
        "owner": int(row["owner"]),
        "packet_size": int(row["packet_size"]),
        "creation_time": float(row["creation_time"])
      }
  return packets

def load_log(path):
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

def compute_packet_latencies(events, packets):
  delivered_time = {}
  for e in events:
    # delivered is equivalent to dst receives the packet.
    if e["event_type"] == "PacketReceive":
      pid = e["pid"]
      if e["nid2"] == packets[pid]["dst"]:
        delivered_time[pid] = e["time"]

  latencies = []
  for pid, pkt in packets.items():
    if pid in delivered_time:
      latencies.append(
        delivered_time[pid] - pkt["creation_time"]
      )

  return latencies

def latency_stats(latencies):
  if not latencies:
    return {}

  return {
    "count": len(latencies),
    "mean": stat.mean(latencies),
    "median": stat.median(latencies),
    "p95": stat.quantiles(latencies, n=20)[18],
    "max": max(latencies)
  }

def plot_latency_hist(latencies):
  plt.figure()
  plt.hist(latencies, bins=30)
  plt.xlabel("Latency")
  plt.ylabel("Packets")
  plt.title("Packet Latency Distribution")
  plt.show()

def main():
  base = "build"

  packets = load_packets(f"{base}/packets/test_net_generator.csv")
  events = load_log(f"{base}/logs/test_net_generator.csv")

  latencies = compute_packet_latencies(events, packets)
  stats = latency_stats(latencies)

  print("Latency Stats:")
  for k, v in stats.items():
    print(f"{k}: {v}")

  plot_latency_hist(latencies)

if __name__ == "__main__":
  main()
