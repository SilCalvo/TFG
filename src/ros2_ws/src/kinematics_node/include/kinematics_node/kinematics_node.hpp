#ifndef KINEMATICS_NODE_HPP_
#define KINEMATICS_NODE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "tf2/LinearMath/Transform.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include <vector>
#include <cmath>

class KinematicsNode : public rclcpp::Node
{
public:
  KinematicsNode();

private:
  // Variables miembro
  std::vector<int16_t> current_angles_; // Guardamos los ángulos aquí
  
  // Publicador y Suscriptor
  rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr subscription_;
  rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_publisher_;

  // Métodos
  void servos_status_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
  
  geometry_msgs::msg::Pose calculate_fk_directly(const std::vector<double>& q);
};

#endif // KINEMATICS_NODE_HPP_