#ifndef MOVE_ARM_NODE_HPP_
#define MOVE_ARM_NODE_HPP_

#include <memory>
#include <vector>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "robot_interfaces/srv/solve_ik.hpp"
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
  rclcpp_action::Server<NavigateToPose>::SharedPtr action_server_;
  rclcpp::Publisher<std_msgs::msg::Int16MultiArray>::SharedPtr publisher_;
  rclcpp::Client<robot_interfaces::srv::SolveIK>::SharedPtr ik_client_;

  std::vector<double> current_angles_;
  Point calculate_fk(const std::vector<double>& angles);
  std::vector<std::vector<double>> get_trajectory(Point target);

  rclcpp_action::GoalResponse handle_goal(
      const rclcpp_action::GoalUUID & uuid,
      std::shared_ptr<const NavigateToPose::Goal> goal);

  rclcpp_action::CancelResponse handle_cancel(
      const std::shared_ptr<GoalHandleNav> goal_handle);

  void handle_accepted(const std::shared_ptr<GoalHandleNav> goal_handle);
  void execute(const std::shared_ptr<GoalHandleNav> goal_handle);
};

#endif 