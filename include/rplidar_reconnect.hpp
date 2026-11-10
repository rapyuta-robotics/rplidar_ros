// Copyright 2026 Rapyuta Robotics Co., Ltd.
//
// Reconnect decision of the rplidar node work loop. ROS 1 rr-devel reconnected on any failed
// grab; here a failed grab while the motor is stopped (stop_motor, auto_standby) is expected,
// so the node asks the lidar for its health before it reconnects.

#ifndef RPLIDAR_RECONNECT_HPP_
#define RPLIDAR_RECONNECT_HPP_

namespace rplidar_ros
{

/// Decide whether to reconnect after one grab of the work loop.
/// @param grab_ok     grabScanDataHq returned a scan
/// @param is_scanning the node started a scan (start() succeeded and no stop() since)
/// @param health_ok   getHealth succeeded; only used when the grab failed while not scanning
inline bool should_reconnect(bool grab_ok, bool is_scanning, bool health_ok)
{
  if (grab_ok) {
    return false;
  }
  if (is_scanning) {
    return true;  // scanning but no data: unplugged or not sending
  }
  return !health_ok;  // stopped: no data is normal, but the lidar must still answer
}

/// True when the work loop must check the lidar health to decide (see should_reconnect).
inline bool needs_health_check(bool grab_ok, bool is_scanning)
{
  return !grab_ok && !is_scanning;
}

}  // namespace rplidar_ros

#endif  // RPLIDAR_RECONNECT_HPP_
