// [treesource] This defines the CongestionControl interface.

#pragma once
#include <algorithm>
#include <cstddef>
#include "sim_types.hpp"
#include <memory>
#include <deque>
#include <limits>
#include <cmath>
#include <unordered_map>
#include <string>

class CongestionControl
{
public:
  virtual void on_ack(SimTime rtt_sample, size_t bytes_acked, SimTime now,
                        SimTime send_time, double delivered_at_send) = 0;
  virtual void on_timeout() = 0;
  virtual double get_cwnd() const = 0;

  // time to wait after a send before maybe sending again
  virtual SimTime pacing_interval(int packet_size) const { return 0.0; }

  virtual std::unordered_map<std::string, double> stats() const { return {}; }

  virtual ~CongestionControl() = default;
};

class AimdCongestionControl : public CongestionControl
{
public:
  AimdCongestionControl(double initial_cwnd, int packet_size) : floor_(initial_cwnd), cwnd_(initial_cwnd), packet_size_(packet_size) {}

  void on_ack(SimTime rtt_sample, size_t bytes_acked, SimTime now, SimTime send_time, double delivered_at_send) override
  {
    // each ACK increases by roughly (1/cwnd) packets worth of bytes
    cwnd_ += (static_cast<double>(bytes_acked) * packet_size_) / cwnd_;
  }

  void on_timeout() override
  {
    cwnd_ = std::max(floor_, cwnd_ / 2.0);
  }

  double get_cwnd() const override { return cwnd_; }

private:
  double floor_;
  double cwnd_;
  int packet_size_;
};

class BbrCongestionControl : public CongestionControl
{
public:
  explicit BbrCongestionControl(double initial_cwnd, double gain, int bw_window_rtts, SimTime rtt_window)
    : floor_(initial_cwnd), gain_(gain), bw_window_rtts_(bw_window_rtts), rtt_window_(rtt_window) {}

  void on_ack(SimTime rtt_sample, size_t bytes_acked, SimTime now, SimTime send_time, double delivered_at_send) override
  {
    // bandwidth sample anchored to packet's original send time, avoids clustering issue
    delivered_bytes_ += bytes_acked;
    if (now > send_time)
    {
      SimTime interval = now - send_time;
      double rate = (delivered_bytes_ - delivered_at_send) / interval;
      bw_samples_.emplace_back(now, rate);
    }

    // window bandwidth samples by RTT count
    // remove anything older than bw_window_rtts_ * current rtt estimate
    SimTime bw_window_time = bw_window_rtts_ * std::max(rtt_sample, 1e-6);
    while (!bw_samples_.empty() && bw_samples_.front().first < now - bw_window_time)
    {
      bw_samples_.pop_front();
    }
    max_bandwidth_bps_ = 0.0;
    for (auto& s : bw_samples_) max_bandwidth_bps_ = std::max(max_bandwidth_bps_, s.second);

    // rtt sample, min filtered over a fixed sim time window
    rtt_samples_.emplace_back(now, rtt_sample);
    while (!rtt_samples_.empty() && rtt_samples_.front().first < now - rtt_window_)
    {
      rtt_samples_.pop_front();
    }
    min_rtt_ = std::numeric_limits<double>::infinity();
    for (auto& s : rtt_samples_) min_rtt_ = std::min(min_rtt_, s.second);
  }

  void on_timeout() override
  {
    // in BBR, this is a no op. Flow does still do the retransmit for us
  }

  double get_cwnd() const override
  {
    if (max_bandwidth_bps_ <= 0.0 || !std::isfinite(min_rtt_))
    {
      return floor_; // no samples, fall back
    }
    return std::max(floor_, max_bandwidth_bps_ * min_rtt_ * gain_);
  }

  SimTime pacing_interval(int packet_size) const override
  {
    if (max_bandwidth_bps_ <= 0.0)
    {
      return 0.0;
    }
    return static_cast<double>(packet_size) / max_bandwidth_bps_;
  }

  std::unordered_map<std::string, double> stats() const override
  {
    return {{"max_bandwidth_bps", max_bandwidth_bps_}, {"min_rtt", min_rtt_}};
  }

private:
  double floor_;
  double gain_;
  int bw_window_rtts_;
  SimTime rtt_window_;

  double delivered_bytes_ = 0.0; // like Flow's total_delivered_
  std::deque<std::pair<SimTime, double>> bw_samples_;
  double max_bandwidth_bps_ = 0.0;

  std::deque<std::pair<SimTime, SimTime>> rtt_samples_;
  double min_rtt_ = std::numeric_limits<double>::infinity();
};

enum class CongestionControlType { AIMD, BBR };

struct CongestionControlParams
{
  double initial_cwnd = 0.0; // "one packet's worth", Flow construction will handle
  int packet_size = 0;
  // BBR-specific params
  double bbr_gain = 2.0; // cwnd = gain * BDP estimate
  int bbr_bw_window_rtts = 8; // max-filter window for bandwidth, in RTTs
  SimTime bbr_rtt_window = 10.0; // min-filter window for RTT, in SimTime
};

inline std::unique_ptr<CongestionControl> make_congestion_control(
  CongestionControlType type, const CongestionControlParams& params)
{
  switch (type)
  {
    case CongestionControlType::AIMD:
      return std::make_unique<AimdCongestionControl>(params.initial_cwnd, params.packet_size);
    case CongestionControlType::BBR:
      return std::make_unique<BbrCongestionControl>(
        params.initial_cwnd, params.bbr_gain, params.bbr_bw_window_rtts, params.bbr_rtt_window);
  }
  return std::make_unique<AimdCongestionControl>(params.initial_cwnd, params.packet_size);
}
