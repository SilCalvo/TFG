#ifndef SERVO_BRIDGE_HPP_
#define SERVO_BRIDGE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include <memory>

class ServoBridgeNode : public rclcpp::Node
{
public:
  ServoBridgeNode();
  ~ServoBridgeNode();

private:
  void robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
  void configure_serial(int fd);
  int serial_port_;
  rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr subscription_;
};

#endif // SERVO_BRIDGE_HPP_