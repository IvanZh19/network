// [treesource] This defines the Tracker class, for recording state over time.

#pragma once
#include "sim_types.hpp"
#include <string>
#include <functional>
#include <vector>
#include <optional>
#include <fstream>
#include <iomanip>

struct Probe
{
  std::string name;
  std::function<double()> getter;
  std::optional<double> last_value;
};

struct ProbeRecord
{
  SimTime time;
  std::string name;
  double value;
};

class Tracker
{
public:
  void add_probe(const std::string& name, std::function<double()> getter)
  {
    probes.push_back({name, std::move(getter), std::nullopt});
  }

  void sample(SimTime now)
  {
    for (auto& p : probes)
    {
      // note we are comparing doubles, but should not face floating point error
      double v = p.getter();
      if (!p.last_value.has_value() || v != *p.last_value)
      {
        probe_records.push_back({now, p.name, v});
        p.last_value = v;
      }
    }
  }

  const std::vector<ProbeRecord>& get_probe_records() const
  {
    return probe_records;
  }

  void dump_csv(const std::string& filename) const
  {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Failed to open tracker file");
    out << std::setprecision(10);
    out << "time,name,value\n";
    for (const auto& pr : probe_records)
    {
      out << pr.time << "," << pr.name << "," << pr.value << "\n";
    }
  }

private:
  std::vector<Probe> probes;
  std::vector<ProbeRecord> probe_records;
};
