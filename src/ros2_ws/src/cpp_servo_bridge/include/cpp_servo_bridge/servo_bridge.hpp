#ifndef SERVO_BRIDGE_HPP_
#define SERVO_BRIDGE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include <string>

class ServoBridgeNode : public rclcpp::Node
{
public:
  ServoBridgeNode();
  ~ServoBridgeNode();

private:
  void robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
  void timer_callback();
  void configure_serial(int fd);
  int serial_port_;
  bool robot_mode = 0; // 0 manual 1 automatic
  std::string read_buffer_; 
  // ROS Interfaces
  rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr subscription_;
  rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
};

#endif // SERVO_BRIDGE_HPP_