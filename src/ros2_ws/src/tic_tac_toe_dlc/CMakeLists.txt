cmake_minimum_required(VERSION 3.8)
project(tic_tac_toe_dlc)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(robot_interfaces REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED) 
find_package(OpenCV REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)

include_directories(include)

add_executable(tic_tac_toe_node src/tic_tac_toe_node.cpp)

ament_target_dependencies(tic_tac_toe_node
  rclcpp
  robot_interfaces
  tf2
  tf2_ros
  tf2_geometry_msgs
  geometry_msgs
  sensor_msgs
  std_srvs
  rclcpp_action
  nav2_msgs
)

target_link_libraries(tic_tac_toe_node ${OpenCV_LIBRARIES})

install(DIRECTORY
  urdf
  models
  launch
  DESTINATION share/${PROJECT_NAME}
)

# Instalar el nodo para poder hacer "ros2 run"
install(TARGETS
  tic_tac_toe_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()