// Copyright 2026 Rapyuta Robotics Co., Ltd.

#include <gtest/gtest.h>

#include "rplidar_reconnect.hpp"

using rplidar_ros::needs_health_check;
using rplidar_ros::should_reconnect;

TEST(RplidarReconnect, GrabOkNeverReconnects)
{
  EXPECT_FALSE(should_reconnect(true, true, false));
  EXPECT_FALSE(should_reconnect(true, false, false));
  EXPECT_FALSE(needs_health_check(true, true));
  EXPECT_FALSE(needs_health_check(true, false));
}

TEST(RplidarReconnect, GrabFailWhileScanningReconnects)
{
  EXPECT_FALSE(needs_health_check(false, true));
  EXPECT_TRUE(should_reconnect(false, true, true));
}

TEST(RplidarReconnect, StoppedAndHealthyDoesNotReconnect)
{
  // stop_motor or auto_standby: no scans, but the lidar answers. The motor stays stopped.
  EXPECT_TRUE(needs_health_check(false, false));
  EXPECT_FALSE(should_reconnect(false, false, true));
}

TEST(RplidarReconnect, StoppedAndNotAnsweringReconnects)
{
  // Unplugged or replugged while stopped.
  EXPECT_TRUE(should_reconnect(false, false, false));
}
