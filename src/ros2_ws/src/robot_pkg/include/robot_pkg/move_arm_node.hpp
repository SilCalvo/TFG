#ifndef MOVE_ARM_NODE_HPP_
#define MOVE_ARM_NODE_HPP_

#include <memory>
#include <vector>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "robot_interfaces/srv/solve_ik.hpp"
#include "robot_interfaces/srv/add_obstacle.hpp"
#include "robot_interfaces/srv/remove_obstacle.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"

class MoveArmNode : public rclcpp::Node {
public:
  using NavigateToPose = nav2_msgs::action::NavigateToPose;
  using GoalHandleNav = rclcpp_action::ServerGoalHandle<NavigateToPose>;

  struct Point {
    double x;
    double y;
    double z;
  };

  MoveArmNode();

private:
  rclcpp_action::Server<NavigateToPose>::SharedPtr action_server_moveJ;
  rclcpp_action::Server<NavigateToPose>::SharedPtr action_server_moveL;
  rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr publisher_;

  rclcpp::Client<robot_interfaces::srv::SolveIK>::SharedPtr ik_client_;
  rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedPtr add_wall_client_;
  rclcpp::Client<robot_interfaces::srv::RemoveObstacle>::SharedPtr remove_wall_client_;

  std::vector<double> current_angles_ = std::vector<double>(5, 0.0);
  Point calculate_dk(const std::vector<double>& angles);
  std::vector<std::vector<double>> get_trajectory_moveJ(Point target);
  std::vector<std::vector<double>> get_trajectory_moveL(Point target);

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