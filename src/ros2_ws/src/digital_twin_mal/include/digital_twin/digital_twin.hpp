#ifndef DIGITAL_TWIN_HPP
#define DIGITAL_TWIN_HPP

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64.hpp"

class DigitalTwinNode : public rclcpp::Node {
public:
    DigitalTwinNode();

private:
    // Callback que recibe todos los grados del puente o del IK
    void joint_states_callback(const sensor_msgs::msg::JointState::SharedPtr msg);

    // Suscriptor principal a las articulaciones calculadas
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr sub_joint_states_;
    
    // Publicadores hacia cada motor del gemelo digital en Webots
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_j1_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_j2_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_j3_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_j4_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr pub_j5_;
};

#endif // DIGITAL_TWIN_HPP