#ifndef RVIZ_BRIDGE_NODE_HPP_
#define RVIZ_BRIDGE_NODE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include <vector>
#include <string>

class RvizBridgeNode : public rclcpp::Node {
public:
  RvizBridgeNode();

private:
  
  void robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
  void timer_callback();

  rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr subscription_;
  rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_; 

  std::vector<std::string> joint_names_;
  sensor_msgs::msg::JointState last_joint_msg_;
};

#endif 