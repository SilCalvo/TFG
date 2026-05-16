#ifndef MOVE_ARM_NODE_HPP_
#define MOVE_ARM_NODE_HPP_

#include <memory>
#include <vector>
#include <thread>
#include <cmath>
#include <algorithm>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "robot_interfaces/srv/solve_ik.hpp"
#include "robot_interfaces/srv/solve_dk.hpp"
#include "robot_interfaces/srv/add_obstacle.hpp"
#include "robot_interfaces/srv/remove_obstacle.hpp"
#include "robot_interfaces/srv/manage_tool.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"
#include "sensor_msgs/msg/joint_state.hpp"

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>


class MoveArmNode : public rclcpp::Node {
public:
  using NavigateToPose = nav2_msgs::action::NavigateToPose;
  using GoalHandleNav = rclcpp_action::ServerGoalHandle<NavigateToPose>;

  struct Point {
    double x;
    double y;
    double z;
    double roll;  
    double pitch; 
    double yaw;
  };

  struct Tool_Config {
    std::string name;
    geometry_msgs::msg::Pose offset;
    std::vector<double> dimensions;
    int type; // 0: Caja, 1: Cilindro
  };

  MoveArmNode();

private:
  rclcpp_action::Server<NavigateToPose>::SharedPtr action_server_moveJ;
  rclcpp_action::Server<NavigateToPose>::SharedPtr action_server_moveL;
  rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr publisher_;
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr subscriber_;

  rclcpp::Client<robot_interfaces::srv::SolveIK>::SharedPtr ik_client_;
  rclcpp::Client<robot_interfaces::srv::SolveDK>::SharedPtr dk_client_;
  rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedPtr add_wall_client_;
  rclcpp::Client<robot_interfaces::srv::RemoveObstacle>::SharedPtr remove_wall_client_;

  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg);

  std::vector<double> current_angles_ = std::vector<double>(5, 0.0);
  Point calculate_dk(const std::vector<double>& angles);
  std::vector<std::vector<double>> get_trajectory_moveJ(geometry_msgs::msg::Pose target_pose);
  std::vector<std::vector<double>> get_trajectory_moveL(Point target);


  std::map<std::string, Tool_Config> tool_library_;
  // Métodos para gestionar la librería
  rclcpp::Service<robot_interfaces::srv::ManageTool>::SharedPtr add_tool_service_;
  rclcpp::Service<robot_interfaces::srv::ManageTool>::SharedPtr delete_tool_service_;
  OnSetParametersCallbackHandle::SharedPtr parameter_callback_handle_;
  // Callbacks
  void handle_add_tool(const std::shared_ptr<robot_interfaces::srv::ManageTool::Request> request,
    std::shared_ptr<robot_interfaces::srv::ManageTool::Response> response);
                       
  void handle_delete_tool(const std::shared_ptr<robot_interfaces::srv::ManageTool::Request> request,
    std::shared_ptr<robot_interfaces::srv::ManageTool::Response> response);

  rcl_interfaces::msg::SetParametersResult on_set_parameters(const std::vector<rclcpp::Parameter> &parameters);


  rclcpp_action::GoalResponse handle_goal_moveJ(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const NavigateToPose::Goal> goal);

  rclcpp_action::CancelResponse handle_cancel_moveJ(
    const std::shared_ptr<GoalHandleNav> goal_handle);

  void handle_accepted_moveJ(const std::shared_ptr<GoalHandleNav> goal_handle);
  void execute_moveJ(const std::shared_ptr<GoalHandleNav> goal_handle);

  rclcpp_action::GoalResponse handle_goal_moveL(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const NavigateToPose::Goal> goal);

  rclcpp_action::CancelResponse handle_cancel_moveL(
    const std::shared_ptr<GoalHandleNav> goal_handle);

  void handle_accepted_moveL(const std::shared_ptr<GoalHandleNav> goal_handle);
  void execute_moveL(const std::shared_ptr<GoalHandleNav> goal_handle);
};

#endif 