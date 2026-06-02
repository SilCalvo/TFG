cmake_minimum_required(VERSION 3.8)
project(robot_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(robot_interfaces REQUIRED)
find_package(urdf REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(robot_kinematics REQUIRED)

include_directories(include)

add_executable(move_arm_node src/move_arm_node.cpp)
ament_target_dependencies(move_arm_node 
  rclcpp
  rclcpp_action
  tf2
  tf2_geometry_msgs
  geometry_msgs
  robot_interfaces
  nav2_msgs
  sensor_msgs
  robot_kinematics
)

add_executable(rviz_bridge_node src/rviz_bridge_node.cpp)
ament_target_dependencies(rviz_bridge_node
  rclcpp
  std_msgs
  sensor_msgs
)

add_executable(servo_bridge_node src/servo_bridge_node.cpp)
ament_target_dependencies(servo_bridge_node 
  rclcpp 
  std_msgs
  geometry_msgs
  tf2
  robot_interfaces
)

add_executable(rutine_node src/routine_executor.cpp)
ament_target_dependencies(rutine_node
  rclcpp
  rclcpp_action
  nav2_msgs
  robot_interfaces
  yaml-cpp
  ament_index_cpp
)
target_link_libraries(rutine_node yaml-cpp)

install(TARGETS
  move_arm_node
  rviz_bridge_node
  servo_bridge_node
  rutine_node
  DESTINATION lib/${PROJECT_NAME}
)


install(DIRECTORY
  urdf
  launch
  rutines
  config
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
