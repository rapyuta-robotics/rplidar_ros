// Copyright 2026 Rapyuta Robotics Co., Ltd.

#include <gtest/gtest.h>

#include <string>

#include "rplidar_diagnostics.hpp"

using diagnostic_msgs::msg::DiagnosticStatus;
using rplidar_ros::make_diagnostic_status;

namespace
{
std::string value_of(const DiagnosticStatus & status, const std::string & key)
{
  for (const auto & kv : status.values) {
    if (kv.key == key) {
      return kv.value;
    }
  }
  ADD_FAILURE() << "missing key " << key;
  return "";
}
}  // namespace

TEST(RplidarDiagnostics, KeepsRos1NameAndKeys)
{
  const auto status = make_diagnostic_status("ABCD", true, 10, 1.0, 0);
  EXPECT_EQ(status.name, "rplidar Status");
  EXPECT_EQ(status.hardware_id, "ABCD");
  ASSERT_EQ(status.values.size(), 2u);
  EXPECT_EQ(status.values[0].key, "Frequency (HZ)");
  EXPECT_EQ(status.values[1].key, "Error Code");
}

TEST(RplidarDiagnostics, FrequencyIsScansPerSecond)
{
  const auto status = make_diagnostic_status("", true, 25, 2.5, 0);
  EXPECT_FLOAT_EQ(std::stof(value_of(status, "Frequency (HZ)")), 10.0f);
  EXPECT_EQ(status.level, DiagnosticStatus::OK);
}

TEST(RplidarDiagnostics, ZeroPeriodGivesZeroFrequency)
{
  const auto status = make_diagnostic_status("", true, 5, 0.0, 0);
  EXPECT_FLOAT_EQ(std::stof(value_of(status, "Frequency (HZ)")), 0.0f);
}

TEST(RplidarDiagnostics, ReportsHealthErrorCode)
{
  const auto status = make_diagnostic_status("", true, 10, 1.0, 0x8001);
  EXPECT_EQ(value_of(status, "Error Code"), "32769");
}

TEST(RplidarDiagnostics, NoScansIsWarn)
{
  const auto status = make_diagnostic_status("", true, 0, 1.0, 0);
  EXPECT_EQ(status.level, DiagnosticStatus::WARN);
}

TEST(RplidarDiagnostics, DisconnectedIsError)
{
  const auto status = make_diagnostic_status("", false, 0, 1.0, 0);
  EXPECT_EQ(status.level, DiagnosticStatus::ERROR);
}
