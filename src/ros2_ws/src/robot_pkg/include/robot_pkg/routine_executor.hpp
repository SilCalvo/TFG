#ifndef ROUTINE_EXECUTOR_HPP_
#define ROUTINE_EXECUTOR_HPP_

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "rclcpp/parameter_client.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include <ament_index_cpp/get_package_share_directory.hpp>

#include "robot_interfaces/srv/add_obstacle.hpp"
#include "robot_interfaces/srv/remove_obstacle.hpp"
#include "robot_interfaces/srv/manage_tool.hpp"

#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

struct RoutineStep {
    std::string type;
    
    double x = 0.0, y = 0.0, z = 0.0;
    double qx = 0.0, qy = 0.0, qz = 0.0, qw = 1.0;
    
    std::string name = "";
    
    double width = 0.1, depth = 0.1, height = 0.1;
    
    int tool_type = 1;
    std::vector<double> dimensions;
    double off_x = 0.0, off_y = 0.0, off_z = 0.0;
    double off_qx = 0.0, off_qy = 0.0, off_qz = 0.0, off_qw = 1.0;
};

class RoutineExecutor : public rclcpp::Node {
public:
    using NavigateToPose = nav2_msgs::action::NavigateToPose;
    using GoalHandleNavigate = rclcpp_action::ClientGoalHandle<NavigateToPose>;

    RoutineExecutor();

private:
    // Movimiento
    rclcpp_action::Client<NavigateToPose>::SharedPtr client_moveJ_;
    rclcpp_action::Client<NavigateToPose>::SharedPtr client_moveL_;

    // Obstáculos y Herramientas
    rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedPtr client_add_wall_;
    rclcpp::Client<robot_interfaces::srv::RemoveObstacle>::SharedPtr client_remove_wall_;
    rclcpp::Client<robot_interfaces::srv::ManageTool>::SharedPtr client_add_tool_;
    rclcpp::Client<robot_interfaces::srv::ManageTool>::SharedPtr client_delete_tool_;

    // Cliente para modificar parámetros asíncronamente en move_arm_node
    rclcpp::AsyncParametersClient::SharedPtr param_client_;

    std::vector<RoutineStep> routine_steps_;
    size_t current_step_;

    void load_routine(const std::string& file_path);
    void execute_next_step();

    void goal_response_callback(const GoalHandleNavigate::SharedPtr& goal_handle);
    void result_callback(const GoalHandleNavigate::WrappedResult& result);
};

#endif