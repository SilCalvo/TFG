#include "robot_pkg/move_arm_node.hpp"

const double PI = 3.14159265358979323846;

MoveArmNode::MoveArmNode()
: Node("move_arm_node") {

  // MoveJ Action Server
  action_server_moveJ = rclcpp_action::create_server<NavigateToPose>(
    this,
    "moveJ",
    std::bind(&MoveArmNode::handle_goal_moveJ, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&MoveArmNode::handle_cancel_moveJ, this, std::placeholders::_1),
    std::bind(&MoveArmNode::handle_accepted_moveJ, this, std::placeholders::_1)
  );

  // MoveL Action Server
  action_server_moveL = rclcpp_action::create_server<NavigateToPose>(
    this,
    "moveL",
    std::bind(&MoveArmNode::handle_goal_moveL, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&MoveArmNode::handle_cancel_moveL, this, std::placeholders::_1),
    std::bind(&MoveArmNode::handle_accepted_moveL, this, std::placeholders::_1)
  );
  
  // Control Joint Service
  control_joint_service_ = this->create_service<robot_interfaces::srv::MoveJoint>(
    "control_joint", 
    std::bind(&MoveArmNode::handle_control_joint, this, std::placeholders::_1, std::placeholders::_2)
  );

  // Go Home Service
  go_home_service_ = this->create_service<robot_interfaces::srv::MoveJoint>(
    "go_home", 
    std::bind(&MoveArmNode::handle_go_home, this, std::placeholders::_1, std::placeholders::_2)
  );

  // Initialize current angles vector based on a parameter (default to 7 joints)
  this->declare_parameter("num_joints", 7);
  int num_joints = this->get_parameter("num_joints").as_int();
  current_angles_.resize(num_joints, 0.0);

  // Init default tool in the library
  geometry_msgs::msg::Pose default_pose;
  default_pose.position.x = 0.0;
  default_pose.position.y = 0.0;
  default_pose.position.z = 0.0; 
  default_pose.orientation.w = 1.0; 

  Tool_Config default_cfg;
  default_cfg.name = "default";
  default_cfg.type = 1; 
  default_cfg.dimensions = {0.01, 0.01}; 
  default_cfg.offset = default_pose;
  tool_library_["default"] = default_cfg;

  // Set up parameter for active tool and callback for changes
  this->declare_parameter("active_tool", "default");
  parameter_callback_handle_ = this->add_on_set_parameters_callback(
    std::bind(&MoveArmNode::on_set_parameters, this, std::placeholders::_1));

  // Add Tool Service
  add_tool_service_ = this->create_service<robot_interfaces::srv::ManageTool>(
    "add_tool", std::bind(&MoveArmNode::handle_add_tool, this, std::placeholders::_1, std::placeholders::_2));

  // Delete Tool Service
  delete_tool_service_ = this->create_service<robot_interfaces::srv::ManageTool>(
    "delete_tool", std::bind(&MoveArmNode::handle_delete_tool, this, std::placeholders::_1, std::placeholders::_2));

  // IK and DK Service Clients
  ik_client_ = this->create_client<robot_interfaces::srv::SolveIK>("solve_ik");
  dk_client_ = this->create_client<robot_interfaces::srv::SolveDK>("solve_dk");
  // Obstacles Service Clients
  add_wall_client_ = this->create_client<robot_interfaces::srv::AddObstacle>("add_wall");
  remove_wall_client_ = this->create_client<robot_interfaces::srv::RemoveObstacle>("remove_wall");

  // Publisher
  rclcpp::QoS qos_profile(1);
  qos_profile.transient_local();
  qos_profile.reliable();

  publisher_ = this->create_publisher<std_msgs::msg::Int16MultiArray>("robot_cmd", qos_profile);

  rclcpp::QoS qos_sub(10);
  subscriber_ = this->create_subscription<sensor_msgs::msg::JointState>(
    "/joint_states",
    qos_sub,
    std::bind(&MoveArmNode::joint_state_callback, this, std::placeholders::_1)
);

  RCLCPP_INFO(this->get_logger(), "Move Arm Node started. Publishing on 'robot_cmd'.");
}

void MoveArmNode::joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
{
    // Save latest joint positions from the sensor
    size_t n = std::min(current_angles_.size(), msg->position.size());
    for (size_t i = 0; i < n; ++i) {
        current_angles_[i] = msg->position[i];  
    }
}
void MoveArmNode::handle_control_joint(
  const std::shared_ptr<robot_interfaces::srv::MoveJoint::Request> request,
  std::shared_ptr<robot_interfaces::srv::MoveJoint::Response> response) 
{
  // Validate request
  if (current_angles_.empty()) {
    response->success = false;
    response->message = "Error: The current_angles_ vector is empty. Cannot determine the current position.";
    RCLCPP_WARN(this->get_logger(), "Cannot send command because current_angles_ has no data.");
    return;
  }
  if (request->index < 0 || static_cast<size_t>(request->index) >= current_angles_.size()) {
    response->success = false;
    response->message = "Error: Index out of range.";
    return;
  }

  // Get the current angle and the target angle in degrees for the requested joint
  int target_index = request->index;
  int16_t target_deg = static_cast<int16_t>(std::round(request->degrees));
  int16_t current_step_deg = static_cast<int16_t>(std::round(current_angles_[target_index] * 180.0 / PI));

  RCLCPP_INFO(this->get_logger(), "Moving Motor [%d] from %d to %d degrees...", target_index, current_step_deg, target_deg);

  // Loop to move in 5 degree increments
  while (current_step_deg != target_deg) {
    int diff = target_deg - current_step_deg;
    if (std::abs(diff) > 5) {
      current_step_deg += (diff > 0) ? 5 : -5;
    } else {
      current_step_deg = target_deg;
    }

    std_msgs::msg::Int16MultiArray cmd_msg;
    
    for (size_t i = 0; i < current_angles_.size(); ++i) {
      if (i == static_cast<size_t>(target_index)) {
        cmd_msg.data.push_back(current_step_deg);
      } else {
        int16_t angle_deg = static_cast<int16_t>(std::round(current_angles_[i] * 180.0 / PI));
        cmd_msg.data.push_back(angle_deg);
      }
    }

    // Publish the array with the current step
    publisher_->publish(cmd_msg);
    RCLCPP_INFO(this->get_logger(), "Step sent: Motor [%d] to %d degrees.", target_index, current_step_deg);

    // Pause between each small step to allow the servos to move
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
  }

  response->success = true;
  response->message = "Mixed command sent: Stepwise movement completed.";
  RCLCPP_INFO(this->get_logger(), "Movement completed successfully.");
}


void MoveArmNode::handle_go_home(
  const std::shared_ptr<robot_interfaces::srv::MoveJoint::Request> request,
  std::shared_ptr<robot_interfaces::srv::MoveJoint::Response> response) 
{
  (void)request; 

  if (current_angles_.empty()) {
    response->success = false;
    response->message = "Error: current_angles_ is empty. Cannot go to home.";
    return;
  }

  // Move all joints smoothly back to zero
  std::vector<double> target_angles(current_angles_.size(), 0.0);

  // Calculate the maximum step to determine the necessary steps
  double max_step_rad = 10.0 * PI / 180.0; // 10 degrees maximum per step
  double max_diff = 0.0;
  
  for (size_t i = 0; i < current_angles_.size(); ++i) {
    double diff = std::abs(target_angles[i] - current_angles_[i]);
    if (diff > max_diff) max_diff = diff;
  }

  int steps = std::max(1, static_cast<int>(std::ceil(max_diff / max_step_rad)));
  RCLCPP_INFO(this->get_logger(), "Moving to HOME in %d smooth steps...", steps);

  // Sequential loop that calculates, converts, and publishes step by step
  for (int step = 1; step <= steps; ++step) {
    double percent = static_cast<double>(step) / steps;
    
    std_msgs::msg::Int16MultiArray cmd_msg;

    for (size_t i = 0; i < current_angles_.size(); ++i) {
      double ang_rad = current_angles_[i] + (target_angles[i] - current_angles_[i]) * percent;
      int16_t ang_deg = static_cast<int16_t>(std::round(ang_rad * 180.0 / PI));
      cmd_msg.data.push_back(ang_deg);
    }

    // Publish the current step 
    publisher_->publish(cmd_msg);

    // Pause 100 milliseconds to allow the servos to move
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  RCLCPP_INFO(this->get_logger(), "Final target angles:");
  for (size_t i = 0; i < target_angles.size(); ++i) {
    RCLCPP_INFO(this->get_logger(), "  Joint %zu: %f rad (0 deg)", i, target_angles[i]);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(750));

  RCLCPP_INFO(this->get_logger(), "The robot has safely reached HOME!");
  
  response->success = true;
  response->message = "The robot has moved to HOME successfully step by step.";
}

MoveArmNode::Point MoveArmNode::calculate_dk(const std::vector<double>& angles) {
  // Denavit Hartenberg table parameters for the 4-DOF arm
  /*Point p;
  p.x = 0.0;
  p.y = 0.0;
  p.z = 0.0;

  if (angles.size() == 4) {
    double q1 = angles[0], q2 = angles[1], q3 = angles[2], q4 = angles[3];
    
    // Robot parameters (meters)
    double l1 = 0.07;
    double l2 = 0.07;
    double l3 = 0.07;
    double l4 = 0.05;
    const double PI = 3.141592653589793;

    // Each trigonometric operation is calculated only once
    double q4_offset = q4 + (PI / 4.0); // q4 + 45 degrees
    double cos_q4_offset = cos(q4_offset);
    double sin_q4_offset = sin(q4_offset);
    
    double sin_q3_q2 = sin(q3 - q2);
    double cos_q2_q3 = cos(q2 - q3); 

    // 4. Simplified final equations
    double common_factor = (l4 * cos_q4_offset * sin_q3_q2) - (l2 * sin(q2)) + (l3 * sin_q3_q2);

    double x =  sin(q1) * common_factor + (l4 * cos(q1) * sin_q4_offset);
    double y = -cos(q1) * common_factor + (l4 * sin(q1) * sin_q4_offset);
    double z = l1 + (l2 * cos(q2)) + cos_q2_q3 * (l3 + l4 * cos_q4_offset);
    
    p.x = x;
    p.y = y;
    p.z = z;
  }

  return p;*/

  // Using the DK service for accurate calculation
  Point p_out = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  auto request = std::make_shared<robot_interfaces::srv::SolveDK::Request>();
  request->joint_angles = angles;
  
  std::string current_tool_name = this->get_parameter("active_tool").as_string();
  auto current_tcp = tool_library_[current_tool_name];
  request->tcp_offset = current_tcp.offset;
  request->tool_dimensions = current_tcp.dimensions;

  if (!dk_client_->wait_for_service(std::chrono::seconds(2))) {
    RCLCPP_ERROR(this->get_logger(), "DK service is not responding.");
    return p_out;
  }

  auto result_future = dk_client_->async_send_request(request);
  auto response = result_future.get();

  if (response && response->success) {
    // Store result coordinates in output point
    p_out.x = response->target_pose.position.x;
    p_out.y = response->target_pose.position.y;
    p_out.z = response->target_pose.position.z;

    tf2::Quaternion q(
      response->target_pose.orientation.x,
      response->target_pose.orientation.y,
      response->target_pose.orientation.z,
      response->target_pose.orientation.w
    );
    tf2::Matrix3x3(q).getRPY(p_out.roll, p_out.pitch, p_out.yaw);
  } else {
    RCLCPP_ERROR(this->get_logger(), "Fail in DK service response.");
  }
  return p_out;

}
void MoveArmNode::handle_add_tool(
  const std::shared_ptr<robot_interfaces::srv::ManageTool::Request> request,
  std::shared_ptr<robot_interfaces::srv::ManageTool::Response> response) {
    
  // Reject requests with empty tool name
  if (request->name.empty()) {
    response->success = false;
    response->message = "Error: The tool name is empty.";
    return;
  }

  // Create a new Tool_Config and add it to the library
  Tool_Config new_tool;
  new_tool.name = request->name;
  new_tool.type = request->type;
  new_tool.dimensions = request->dimensions;
  new_tool.offset = request->offset;

  tool_library_[request->name] = new_tool;

  RCLCPP_INFO(this->get_logger(), "Tool '%s' added directly via service.", request->name.c_str());
  
  response->success = true;
  response->message = "Tool '" + request->name + "' saved successfully.";
}

void MoveArmNode::handle_delete_tool(
  const std::shared_ptr<robot_interfaces::srv::ManageTool::Request> request,
  std::shared_ptr<robot_interfaces::srv::ManageTool::Response> response) 
{
  auto it = tool_library_.find(request->name);
  
  // Prevent deletion of the default tool and check if the tool exists
  if (it != tool_library_.end()) {

    if (request->name == "default") {
      response->success = false;
      response->message = "Forbidden to delete the 'default' tool.";
      RCLCPP_WARN(this->get_logger(), "Attempt to delete protected tool.");
      return;
    }
    tool_library_.erase(it);

    // Reset active tool to default if it was deleted
    std::string active_tool_name_ = this->get_parameter("active_tool").as_string();
    if (active_tool_name_ == request->name) {
      this->set_parameter(rclcpp::Parameter("active_tool", "default"));
    }

    RCLCPP_INFO(this->get_logger(), "Tool '%s' deleted via service.", request->name.c_str());
    response->success = true;
    response->message = "Tool deleted successfully.";
  } 
  else {
    response->success = false;
    response->message = "Error: The tool '" + request->name + "' does not exist.";
  }
}

rcl_interfaces::msg::SetParametersResult MoveArmNode::on_set_parameters(
  const std::vector<rclcpp::Parameter> &parameters) {

  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;

  // Validate that the requested active tool exists in the library
  for (const auto &param : parameters) {
    if (param.get_name() == "active_tool") {
      std::string requested_tool = param.as_string();
      
      if (tool_library_.find(requested_tool) == tool_library_.end()) {
        result.successful = false;
        result.reason = "Error: The tool '" + requested_tool + "' is not in the library.";
        RCLCPP_WARN(this->get_logger(), "Rejected parameter change: %s does not exist.", requested_tool.c_str());
      } else {
        RCLCPP_INFO(this->get_logger(), "Validation successful: active tool is now '%s'", requested_tool.c_str());
      }
    }
  }
  return result;
}

std::vector<std::vector<double>> MoveArmNode::get_trajectory_moveJ(geometry_msgs::msg::Pose target_pose) {
  std::vector<std::vector<double>> trajectory;
  auto request = std::make_shared<robot_interfaces::srv::SolveIK::Request>();
  
  request->target_pose = target_pose; 
  request->origin_joint_angles = current_angles_;
  
  // Add tool information to the IK request
  std::string current_tool_name = this->get_parameter("active_tool").as_string();
  auto current_tcp = tool_library_[current_tool_name];
  request->tcp_offset = current_tcp.offset;
  request->tool_dimensions = current_tcp.dimensions;
  request->tool_type = current_tcp.type;

  if (!ik_client_->wait_for_service(std::chrono::seconds(2))) {
    RCLCPP_ERROR(this->get_logger(), "IK service is not responding. Aborting trajectory.");
    return trajectory;
  }

  auto result_future = ik_client_->async_send_request(request);
  auto response = result_future.get();

  if (!response->success || response->joint_angles.empty()) {
    RCLCPP_ERROR(this->get_logger(), "No IK solution found.");
    return trajectory;
  }

  // Calculate the maximum step to determine the necessary steps for smooth trajectory
  std::vector<double> target_angles = response->joint_angles;
  double max_step_rad = 10.0 * PI / 180.0;
  double max_diff = 0.0;
  
  for (size_t i = 0; i < current_angles_.size() && i < target_angles.size(); ++i) {
    double diff = std::abs(target_angles[i] - current_angles_[i]);
    if (diff > max_diff) max_diff = diff;
  }

  int steps = std::max(1, static_cast<int>(std::ceil(max_diff / max_step_rad)));
  RCLCPP_INFO(this->get_logger(), "Generating trajectory with %d steps...", steps);

  for (int step = 1; step <= steps; ++step) {
    std::vector<double> step_angles;
    for (size_t i = 0; i < current_angles_.size(); ++i) {
      double percent = static_cast<double>(step) / steps;
      double ang = current_angles_[i] + (target_angles[i] - current_angles_[i]) * percent;
      step_angles.push_back(ang);
    }
    trajectory.push_back(step_angles);
  }
  return trajectory;
}

// Trajectory generator for linear movement
std::vector<std::vector<double>> MoveArmNode::get_trajectory_moveL(Point target) {
  std::vector<std::vector<double>> trajectory;
  if (current_angles_.empty()) return trajectory;


  Point start = calculate_dk(current_angles_); 
  tf2::Quaternion q_fixed;
  q_fixed.setRPY(start.roll, start.pitch, start.yaw);

  double distance = std::sqrt(std::pow(target.x - start.x, 2) + 
                              std::pow(target.y - start.y, 2) + 
                              std::pow(target.z - start.z, 2));
  
  // Determine number of steps based on distance, with a maximum step size to ensure smoothness
  double step_size = 0.02; // 2cm per step
  int steps = std::max(1, static_cast<int>(std::ceil(distance / step_size)));

  if (!ik_client_->wait_for_service(std::chrono::seconds(2))) return trajectory;
  std::vector<double> current_angles_1 = current_angles_;
  for (int step = 1; step <= steps; ++step) {
    double percent = static_cast<double>(step) / steps;
    auto request = std::make_shared<robot_interfaces::srv::SolveIK::Request>();
    
    // Linear interpolation of position
    request->target_pose.position.x = start.x + (target.x - start.x) * percent;
    request->target_pose.position.y = start.y + (target.y - start.y) * percent;
    request->target_pose.position.z = start.z + (target.z - start.z) * percent;
    
    // Fixed orientation (the tool does not change its orientation during the linear movement)
    request->target_pose.orientation.x = q_fixed.x();
    request->target_pose.orientation.y = q_fixed.y();
    request->target_pose.orientation.z = q_fixed.z();
    request->target_pose.orientation.w = q_fixed.w();

    request->origin_joint_angles = current_angles_1;

    std::string current_tool_name = this->get_parameter("active_tool").as_string();
    auto current_tcp = tool_library_[current_tool_name];
    request->tcp_offset = current_tcp.offset;
    request->tool_dimensions = current_tcp.dimensions;
    request->tool_type = current_tcp.type;

    auto result_future = ik_client_->async_send_request(request);
    if (result_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
      auto response = result_future.get();
      if (response->success) {
        std::vector<double> angles = response->joint_angles;
        // Update current_angles_1 for the next iteration to ensure continuity in the trajectory
        current_angles_1= angles;
        trajectory.push_back(angles);
      } else {
        RCLCPP_WARN(this->get_logger(), "MoveL trajectory blocked by collision/limits.");
        return {};
      }
    } else {
      RCLCPP_ERROR(this->get_logger(), "Timeout: IK server did not respond in time. Aborting trajectory.");
      return {}; 
    }
  }
  return trajectory;
}


// MOVEJ Action Server Callbacks

rclcpp_action::GoalResponse MoveArmNode::handle_goal_moveJ(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const NavigateToPose::Goal> goal) {
  // Log and accept all incoming movement goals
  RCLCPP_INFO(this->get_logger(), "Goal received: x=%.2f y=%.2f z=%.2f",
              goal->pose.pose.position.x,
              goal->pose.pose.position.y,
              goal->pose.pose.position.z);
  (void)uuid;
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse MoveArmNode::handle_cancel_moveJ(
    const std::shared_ptr<GoalHandleNav> goal_handle) {
  // Accept all cancellation requests
  (void)goal_handle;
  RCLCPP_INFO(this->get_logger(), "Cancellation requested.");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void MoveArmNode::handle_accepted_moveJ(const std::shared_ptr<GoalHandleNav> goal_handle) {
  // Run the movement in a background thread
  std::thread{std::bind(&MoveArmNode::execute_moveJ, this, std::placeholders::_1), goal_handle}.detach();
}

void MoveArmNode::execute_moveJ(const std::shared_ptr<GoalHandleNav> goal_handle) {
  const auto goal = goal_handle->get_goal();
  auto feedback = std::make_shared<NavigateToPose::Feedback>();
  auto result = std::make_shared<NavigateToPose::Result>();

  // Compute joint path to the target position
  auto trajectory_angles = get_trajectory_moveJ(goal->pose.pose);

  if (trajectory_angles.empty()) {
    RCLCPP_ERROR(this->get_logger(), "Empty trajectory. Aborting action.");
    goal_handle->abort(result);
    return;
  }

  // Execute the trajectory step by step, checking for cancellation and publishing feedback
  for (const auto & angles : trajectory_angles) {
    if (goal_handle->is_canceling()) {
      goal_handle->canceled(result);
      RCLCPP_INFO(this->get_logger(), "Trajectory canceled.");
      return;
    }

    std_msgs::msg::Int16MultiArray cmd_msg;
    for (double angle_rad : angles) {
      int16_t angle_deg = static_cast<int16_t>(std::round(angle_rad * 180.0 / PI));
      cmd_msg.data.push_back(angle_deg);
    }

    publisher_->publish(cmd_msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send current arm position as feedback
    Point current_xyz = calculate_dk(angles); 
    feedback->current_pose.pose.position.x = current_xyz.x;
    feedback->current_pose.pose.position.y = current_xyz.y;
    feedback->current_pose.pose.position.z = current_xyz.z;

    goal_handle->publish_feedback(feedback);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(750));


  if (rclcpp::ok()) {
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Trajectory completed and Arduino positioned!");
  }

}

// MOVE L Action Server Callbacks
rclcpp_action::GoalResponse MoveArmNode::handle_goal_moveL(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const NavigateToPose::Goal> goal) {
  // Log and accept all incoming movement goals
  RCLCPP_INFO(this->get_logger(), "Goal received: x=%.2f y=%.2f z=%.2f",
              goal->pose.pose.position.x,
              goal->pose.pose.position.y,
              goal->pose.pose.position.z);
  (void)uuid;
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse MoveArmNode::handle_cancel_moveL(
    const std::shared_ptr<GoalHandleNav> goal_handle) {
  // Accept all cancellation requests
  (void)goal_handle;
  RCLCPP_INFO(this->get_logger(), "Cancellation requested.");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void MoveArmNode::handle_accepted_moveL(const std::shared_ptr<GoalHandleNav> goal_handle) {
  // Run the movement in a background thread
  std::thread{std::bind(&MoveArmNode::execute_moveL, this, std::placeholders::_1), goal_handle}.detach();
}

void MoveArmNode::execute_moveL(const std::shared_ptr<GoalHandleNav> goal_handle) {
  const auto goal = goal_handle->get_goal();
  auto feedback = std::make_shared<NavigateToPose::Feedback>();
  auto result = std::make_shared<NavigateToPose::Result>();

  // Extract target position from goal
  Point target_pt = {
    goal->pose.pose.position.x,
    goal->pose.pose.position.y,
    goal->pose.pose.position.z
  };

  auto trajectory_angles = get_trajectory_moveL(target_pt);

  if (trajectory_angles.empty()) {
    RCLCPP_ERROR(this->get_logger(), "Empty trajectory. Aborting action.");
    goal_handle->abort(result);
    return;
  }

  // Execute the trajectory step by step, checking for cancellation and publishing feedback
  for (const auto & angles : trajectory_angles) {
    if (goal_handle->is_canceling()) {
      goal_handle->canceled(result);
      RCLCPP_INFO(this->get_logger(), "Trajectory canceled.");
      return;
    }

    std_msgs::msg::Int16MultiArray cmd_msg;
    for (double angle_rad : angles) {
      int16_t angle_deg = static_cast<int16_t>(std::round(angle_rad * 180.0 / PI));
      cmd_msg.data.push_back(angle_deg);
    }

    publisher_->publish(cmd_msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send current arm position as feedback
    Point current_xyz = calculate_dk(angles); 
    feedback->current_pose.pose.position.x = current_xyz.x;
    feedback->current_pose.pose.position.y = current_xyz.y;
    feedback->current_pose.pose.position.z = current_xyz.z;

    goal_handle->publish_feedback(feedback);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(750));


  if (rclcpp::ok()) {
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Trajectory completed and Arduino positioned!");
  }
}

int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MoveArmNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
