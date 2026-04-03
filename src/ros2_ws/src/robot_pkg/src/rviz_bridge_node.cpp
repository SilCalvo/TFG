#include "robot_pkg/rviz_bridge_node.hpp"
#include <cmath>
#include <chrono> 

using namespace std::chrono_literals;
const double PI = 3.14159265358979323846;

RvizBridgeNode::RvizBridgeNode() 
: Node("rviz_bridge_node") {
  
  // 1. Suscriptor
  rclcpp::QoS qos_sub(1);
  qos_sub.transient_local();
  subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", qos_sub, 
    std::bind(&RvizBridgeNode::robot_cmd_callback, this, std::placeholders::_1)
  );

  // 2. Publicador a /joint_states
  rclcpp::QoS qos_pub(10);
  publisher_ = this->create_publisher<sensor_msgs::msg::JointState>("/joint_states", qos_pub);

  joint_names_ = {"joint_base_c1", "joint_c1_c2", "joint_c2_c3", "joint_c3_c4", "joint_c4_c5"};

  last_joint_msg_.name = joint_names_;
  last_joint_msg_.position = {0.0, 0.0, 0.0, 0.0, 0.0};

  timer_ = this->create_wall_timer(
    100ms, std::bind(&RvizBridgeNode::timer_callback, this)
  );

  RCLCPP_INFO(this->get_logger(), "Nodo Puente iniciado con publicador continuo a 10Hz");
}

void RvizBridgeNode::robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg) {
  for (size_t i = 0; i < msg->data.size() && i < joint_names_.size(); ++i) {
    last_joint_msg_.position[i] = msg->data[i] * (PI / 180.0);
  }
}

void RvizBridgeNode::timer_callback() {
  last_joint_msg_.header.stamp = this->now();
  publisher_->publish(last_joint_msg_);
}

int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<RvizBridgeNode>());
  rclcpp::shutdown();
  return 0;
}