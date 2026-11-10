// Copyright 2026 Rapyuta Robotics Co., Ltd.
//
// Diagnostic status of the rplidar node, published on /diagnostics at 1 Hz.
// Port of the ROS 1 rr-devel diagnostics (rapyuta-robotics/rplidar_ros PR #6): same status name
// and keys, so existing consumers keep working.

#ifndef RPLIDAR_DIAGNOSTICS_HPP_
#define RPLIDAR_DIAGNOSTICS_HPP_

#include <cstddef>
#include <cstdint>
#include <string>

#include <diagnostic_msgs/msg/diagnostic_status.hpp>
#include <diagnostic_msgs/msg/key_value.hpp>

namespace rplidar_ros
{

constexpr const char * kDiagnosticName = "rplidar Status";
constexpr const char * kFrequencyKey = "Frequency (HZ)";
constexpr const char * kErrorCodeKey = "Error Code";

/// Build the status for one diagnostics period.
/// @param hardware_id lidar serial number (hex), empty before the first connection
/// @param connected   true when the driver is connected to the lidar
/// @param scan_count  number of scans published in the period
/// @param period_s    length of the period in seconds
/// @param error_code  error code of the last health check
inline diagnostic_msgs::msg::DiagnosticStatus make_diagnostic_status(
  const std::string & hardware_id, bool connected, std::size_t scan_count, double period_s,
  std::uint16_t error_code)
{
  using diagnostic_msgs::msg::DiagnosticStatus;
  DiagnosticStatus status;
  status.name = kDiagnosticName;
  status.hardware_id = hardware_id;
  if (!connected) {
    status.level = DiagnosticStatus::ERROR;
    status.message = "Not connected";
  } else if (scan_count == 0) {
    status.level = DiagnosticStatus::WARN;
    status.message = "Connected, no scans";
  } else {
    status.level = DiagnosticStatus::OK;
    status.message = "Scanning";
  }

  const double frequency = period_s > 0.0 ? static_cast<double>(scan_count) / period_s : 0.0;
  diagnostic_msgs::msg::KeyValue kv;
  kv.key = kFrequencyKey;
  kv.value = std::to_string(static_cast<float>(frequency));
  status.values.push_back(kv);
  kv.key = kErrorCodeKey;
  kv.value = std::to_string(error_code);
  status.values.push_back(kv);
  return status;
}

}  // namespace rplidar_ros

#endif  // RPLIDAR_DIAGNOSTICS_HPP_
