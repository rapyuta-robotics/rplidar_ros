################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.10)
project(rplidar_ros)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(MSVC)
  add_compile_definitions(
    _USE_MATH_DEFINES
  )
endif()

set(RPLIDAR_SDK_PATH "${PROJECT_SOURCE_DIR}/sdk/")

if(APPLE)
add_compile_definitions(_MACOS)
FILE(GLOB RPLIDAR_SDK_SRC
  "${RPLIDAR_SDK_PATH}/src/arch/macOS/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/dataunpacker/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/dataunpacker/unpacker/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/hal/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/*.cpp"
)
else()
FILE(GLOB RPLIDAR_SDK_SRC 
  "${RPLIDAR_SDK_PATH}/src/arch/linux/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/dataunpacker/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/dataunpacker/unpacker/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/hal/*.cpp"
  "${RPLIDAR_SDK_PATH}/src/*.cpp"
)
endif()

################################################################################
# Find ament packages and libraries for ament and system dependencies
################################################################################
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(diagnostic_msgs REQUIRED)
find_package(Threads REQUIRED)

################################################################################
# Build
################################################################################
# The vendored Slamtec SDK is built once, as a static library. It is third-party code:
# its warnings are not ours to fix, so they are disabled and its headers are SYSTEM.
add_library(rplidar_sdk STATIC ${RPLIDAR_SDK_SRC})
target_include_directories(rplidar_sdk SYSTEM PUBLIC
  ${RPLIDAR_SDK_PATH}/include
  ${RPLIDAR_SDK_PATH}/src
)
target_compile_options(rplidar_sdk PRIVATE -w)
set_target_properties(rplidar_sdk PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(rplidar_sdk PUBLIC Threads::Threads)

add_executable(rplidar_node src/rplidar_node.cpp)
target_include_directories(rplidar_node PRIVATE include)
target_link_libraries(rplidar_node
  rplidar_sdk
  rclcpp::rclcpp
  ${std_srvs_TARGETS}
  ${sensor_msgs_TARGETS}
  ${diagnostic_msgs_TARGETS}
)

add_executable(rplidar_composition src/rplidar_node.cpp)
target_include_directories(rplidar_composition PRIVATE include)
target_link_libraries(rplidar_composition
  rplidar_sdk
  rclcpp::rclcpp
  ${std_srvs_TARGETS}
  ${sensor_msgs_TARGETS}
  ${diagnostic_msgs_TARGETS}
)

add_executable(rplidar_client src/rplidar_client.cpp)
target_link_libraries(rplidar_client
  rclcpp::rclcpp
  ${std_srvs_TARGETS}
  ${sensor_msgs_TARGETS}
)
################################################################################
# Install
################################################################################
install(DIRECTORY launch rviz
  DESTINATION share/${PROJECT_NAME}
)

install(
  TARGETS rplidar_node rplidar_composition rplidar_client
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

################################################################################
# Test
################################################################################
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_diagnostics test/test_diagnostics.cpp)
  target_include_directories(test_diagnostics PRIVATE include)
  target_link_libraries(test_diagnostics ${diagnostic_msgs_TARGETS})
  ament_add_gtest(test_reconnect test/test_reconnect.cpp)
  target_include_directories(test_reconnect PRIVATE include)
endif()

################################################################################
# Macro for ament package
################################################################################
ament_export_dependencies(rclcpp)
ament_export_dependencies(std_msgs)
ament_export_dependencies(sensor_msgs)
ament_export_include_directories(include)
ament_package()
