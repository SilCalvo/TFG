#ifndef SERVO_BRIDGE_HPP_
#define SERVO_BRIDGE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include <geometry_msgs/msg/pose.hpp>
#include <string>
#include <vector>

// Asegúrate de que los nombres de las cabeceras coincidan con tu paquete
#include "robot_interfaces/srv/add_obstacle.hpp"
#include "robot_interfaces/srv/solve_dk.hpp"

class ServoBridgeNode : public rclcpp::Node
{
public:
  ServoBridgeNode();
  ~ServoBridgeNode();

  struct Tool_Config {
    std::string name;
    geometry_msgs::msg::Pose offset;
    std::vector<double> dimensions;
    int type; // 0: Caja, 1: Cilindro
  };

private:
  void robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg);
  void timer_callback();
  void add_wall_point(const std::vector<int>& v);
  void configure_serial(int fd);
  
  int serial_port_;
  int robot_mode = 2; // 0 manual, 1 automatic, 2 init
  int wall_count_ = 0; // CORREGIDO: Añadido guión bajo para ser consistente
  std::string read_buffer_; 
  size_t num_joints_;
  
  rclcpp::Subscription<std_msgs::msg::Int16MultiArray>::SharedPtr subscription_;
  rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr publisher_;
  
  // Clientes de servicio
  rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedPtr add_wall_client_;
  rclcpp::Client<robot_interfaces::srv::SolveDK>::SharedPtr dk_client_; // AÑADIDO
  
  rclcpp::TimerBase::SharedPtr timer_;
};

#endif
