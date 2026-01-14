// [treesource] This defines the interface for Logger, for recording simulation behavior.

#pragma once
#include "sim_types.hpp"
#include <vector>
#include <fstream>
#include <filesystem>

// it's necessary that these EventRecords are capable of capturing all basic details about network behavior, as aside from
// topology recorded this is the only other data the visualizer gets.

enum class EventType
{
  PacketCreate,
  PacketSend,
  PacketReceive
};

inline const char* to_string(EventType t)
{
  switch (t) {
    case EventType::PacketCreate: return "PacketCreate";
    case EventType::PacketSend: return "PacketSend";
    case EventType::PacketReceive: return "PacketReceive";
  }
  return "Unknown";
}

struct EventRecord
{
  SimTime time;
  EventType type;

  // PacketCreate: src/dst, PacketSend: from/to, PacketReceive: from/to
  NodeId nid1;
  NodeId nid2;
  PacketId pid;
};

class Logger
{
public:
  void log(EventRecord er)
  {
    event_records.push_back(std::move(er));
  }

  const std::vector<EventRecord>& get_event_records() const
  {
    return event_records;
  }

  void dump_csv(const std::string& filename) const
  {
    std::ofstream out(filename);
    if (!out)
    {
      throw std::runtime_error("Failed to open log file");
    }

    out << "time,event_type,nid1,nid2,pid\n";

    for (const auto& er : event_records)
    {
      out << er.time << ","
          << to_string(er.type) << ","
          << er.nid1 << ","
          << er.nid2 << ","
          << er.pid << "\n";
    }
  }

private:
  std::vector<EventRecord> event_records;
};
