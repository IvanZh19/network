// [treesource] This defines the CongestionControl interface.

#pragma once
#include <algorithm>
#include <cstddef>
#include "sim_types.hpp"
#include <memory>

class CongestionControl
{
public:
  virtual void on_ack(SimTime rtt_sample, size_t bytes_acked, SimTime now) = 0;
  virtual void on_timeout() = 0;
  virtual double get_cwnd() const = 0;

  // time to wait after a send before maybe sending again
  virtual SimTime pacing_interval(int packet_size) const { return 0.0; }

  virtual ~CongestionControl() = default;
};

class AimdCongestionControl : public CongestionControl
{
public:
  explicit AimdCongestionControl(double initial_cwnd = 1.0) : cwnd_(initial_cwnd) {}

  void on_ack(SimTime, size_t, double) override
  {
    cwnd_ += 1.0;
  }

  void on_timeout() override
  {
    cwnd_ = std::max(1.0, cwnd_ / 2.0);
  }

  double get_cwnd() const override { return cwnd_; }

private:
  double cwnd_;
};

enum class CongestionControlType { AIMD, BBR };

struct CongestionControlParams
{
  double initial_cwnd = 1.0;
  // BBR-specific params would be added here
};

inline std::unique_ptr<CongestionControl> make_congestion_control(
  CongestionControlType type, const CongestionControlParams& params)
{
  switch (type)
  {
    case CongestionControlType::AIMD:
      return std::make_unique<AimdCongestionControl>(params.initial_cwnd);
    case CongestionControlType::BBR:
      // not yet impl, use AIMD
      return std::make_unique<AimdCongestionControl>(params.initial_cwnd);
  }
  return std::make_unique<AimdCongestionControl>(params.initial_cwnd);
}
