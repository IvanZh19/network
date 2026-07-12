# [treesource] This script plots Tracker probe output over time.

import argparse
import csv
from collections import defaultdict
import matplotlib.pyplot as plt

DEFAULT_PROBES = "build/probes/test_congestion_cutback.csv"

def load_probes(path):
  series = defaultdict(list)
  with open(path, newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
      series[row["name"]].append((float(row["time"]), float(row["value"])))
  return series

def plot_series(series, names=None, title="Tracked variables over time", ylabel="value", save_file=None):
  plt.figure(figsize=(10, 5))
  for name, points in series.items():
    if names and name not in names:
      continue
    times = [t for t, _ in points]
    values = [v for _, v in points]
    plt.step(times, values, where="post", label=name)

  plt.xlabel("time")
  plt.ylabel(ylabel)
  plt.title(title)
  plt.legend()
  plt.tight_layout()

  if save_file:
    plt.savefig(save_file)
    print(f"Saved to {save_file}")
  else:
    plt.show()

def main():
  parser = argparse.ArgumentParser(description="Plot Tracker probe output over time")
  parser.add_argument("--probes", default=DEFAULT_PROBES)
  parser.add_argument("--names", nargs="*", default=None, help="subset of probe names to plot (default is all)")
  parser.add_argument("--title", default="Tracked variables over time")
  parser.add_argument("--ylabel", default="value")
  parser.add_argument("--save", default=None)
  args = parser.parse_args()

  series = load_probes(args.probes)

  if args.names is None:
    print(f"Available probes: {sorted(series.keys())}")

  plot_series(series, names=args.names, title=args.title, ylabel=args.ylabel, save_file=args.save)

if __name__ == "__main__":
  main()
