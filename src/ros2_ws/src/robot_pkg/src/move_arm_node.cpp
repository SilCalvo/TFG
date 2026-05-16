#include "robot_pkg/move_arm_node.hpp"

const double PI = 3.14159265358979323846;

MoveArmNode::MoveArmNode()
: Node("move_arm_node") {

  // Servidor de acción
  action_server_moveJ = rclcpp_action::create_server<NavigateToPose>(
    this,
    "moveJ",
    std::bind(&MoveArmNode::handle_goal_moveJ, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&MoveArmNode::handle_cancel_moveJ, this, std::placeholders::_1),
    std::bind(&MoveArmNode::handle_accepted_moveJ, this, std::placeholders::_1)
  );

  // Servidor de acción
  action_server_moveL = rclcpp_action::create_server<NavigateToPose>(
    this,
    "moveL",
    std::bind(&MoveArmNode::handle_goal_moveL, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&MoveArmNode::handle_cancel_moveL, this, std::placeholders::_1),
    std::bind(&MoveArmNode::handle_accepted_moveL, this, std::placeholders::_1)
  );

  geometry_msgs::msg::Pose default_pose;
  default_pose.position.x = 0.0;
  default_pose.position.y = 0.0;
  default_pose.position.z = 0.0; 
  default_pose.orientation.w = 1.0; 

  Tool_Config default_cfg;
  default_cfg.name = "default";
  default_cfg.type = 1; 
  default_cfg.dimensions = {0.25, 0.05}; 
  default_cfg.offset = default_pose;
  tool_library_["default"] = default_cfg;

  // SET TOOL
  this->declare_parameter("active_tool", "default");
  parameter_callback_handle_ = this->add_on_set_parameters_callback(
    std::bind(&MoveArmNode::on_set_parameters, this, std::placeholders::_1));

  // ADD TOOL 
  add_tool_service_ = this->create_service<robot_interfaces::srv::ManageTool>(
    "add_tool", std::bind(&MoveArmNode::handle_add_tool, this, std::placeholders::_1, std::placeholders::_2));

  // DELETE TOOL 
  delete_tool_service_ = this->create_service<robot_interfaces::srv::ManageTool>(
    "delete_tool", std::bind(&MoveArmNode::handle_delete_tool, this, std::placeholders::_1, std::placeholders::_2));

  // Cliente del servicio IK
  ik_client_ = this->create_client<robot_interfaces::srv::SolveIK>("solve_ik");
  dk_client_ = this->create_client<robot_interfaces::srv::SolveDK>("solve_dk");
  add_wall_client_ = this->create_client<robot_interfaces::srv::AddObstacle>("add_wall");
  remove_wall_client_ = this->create_client<robot_interfaces::srv::RemoveObstacle>("remove_wall");

  // Publicador
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

  RCLCPP_INFO(this->get_logger(), "Nodo 'move_arm_node' inicializado. Publicando en 'robot_cmd'.");
}

void MoveArmNode::joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
{
    size_t n = std::min(current_angles_.size(), msg->position.size());
    for (size_t i = 0; i < n; ++i) {
        current_angles_[i] = msg->position[i];  // actualizar tu vector miembro
    }
}

// Cinemática directa -------------------
MoveArmNode::Point MoveArmNode::calculate_dk(const std::vector<double>& angles) {
  /*Point p = {0.0, 0.0, 0.0};
  if (angles.size() >= 3) {
    double q1 = angles[0], q2 = angles[1], q3 = angles[2];
    p.x = std::cos(q1) * (0.4 * std::sin(q2 + q3) + 0.3 * std::sin(q2));
    p.y = std::sin(q1) * (0.4 * std::sin(q2 + q3) + 0.3 * std::sin(q2));
    p.z = 0.4 * std::cos(q2 + q3) + 0.3 * std::cos(q2) + 0.4;
  }
  return p;*/

  Point p_out = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  auto request = std::make_shared<robot_interfaces::srv::SolveDK::Request>();
  request->joint_angles = angles;
  
  std::string current_tool_name = this->get_parameter("active_tool").as_string();
  auto current_tcp = tool_library_[current_tool_name];
  request->tcp_offset = current_tcp.offset;
  request->tool_dimensions = current_tcp.dimensions;

  if (!dk_client_->wait_for_service(std::chrono::seconds(2))) {
    RCLCPP_ERROR(this->get_logger(), "El servicio DK no responde.");
    return p_out;
  }

  auto result_future = dk_client_->async_send_request(request);
  auto response = result_future.get();

  if (response && response->success) {
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
    RCLCPP_ERROR(this->get_logger(), "Fallo en la respuesta del servicio DK.");
  }
  return p_out;

}
void MoveArmNode::handle_add_tool(
  const std::shared_ptr<robot_interfaces::srv::ManageTool::Request> request,
  std::shared_ptr<robot_interfaces::srv::ManageTool::Response> response) {
    
  if (request->name.empty()) {
    response->success = false;
    response->message = "Error: El nombre de la herramienta está vacío.";
    return;
  }

  // Lógica que antes estaba en add_tool()
  Tool_Config new_tool;
  new_tool.name = request->name;
  new_tool.type = request->type;
  new_tool.dimensions = request->dimensions;
  new_tool.offset = request->offset;

  tool_library_[request->name] = new_tool;

  RCLCPP_INFO(this->get_logger(), "Herramienta '%s' añadida directamente vía servicio.", request->name.c_str());
  
  response->success = true;
  response->message = "Herramienta '" + request->name + "' guardada con éxito.";
}

void MoveArmNode::handle_delete_tool(
  const std::shared_ptr<robot_interfaces::srv::ManageTool::Request> request,
  std::shared_ptr<robot_interfaces::srv::ManageTool::Response> response) 
{
  auto it = tool_library_.find(request->name);
  
  if (it != tool_library_.end()) {

    if (request->name == "default") {
      response->success = false;
      response->message = "Prohibido borrar la herramienta 'default'.";
      RCLCPP_WARN(this->get_logger(), "Intento de borrado de herramienta protegida.");
      return;
    }
    tool_library_.erase(it);

    std::string active_tool_name_ = this->get_parameter("active_tool").as_string();
    if (active_tool_name_ == request->name) {
      this->set_parameter(rclcpp::Parameter("active_tool", "default"));
    }

    RCLCPP_INFO(this->get_logger(), "Herramienta '%s' eliminada vía servicio.", request->name.c_str());
    response->success = true;
    response->message = "Herramienta eliminada correctamente.";
  } 
  else {
    response->success = false;
    response->message = "Error: La herramienta '" + request->name + "' no existe.";
  }
}

rcl_interfaces::msg::SetParametersResult MoveArmNode::on_set_parameters(
  const std::vector<rclcpp::Parameter> &parameters) {

  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;

  for (const auto &param : parameters) {
    if (param.get_name() == "active_tool") {
      std::string requested_tool = param.as_string();
      
      if (tool_library_.find(requested_tool) == tool_library_.end()) {
        result.successful = false;
        result.reason = "Error: La herramienta '" + requested_tool + "' no está en la librería.";
        RCLCPP_WARN(this->get_logger(), "Rechazado cambio de parámetro: %s no existe.", requested_tool.c_str());
      } else {
        RCLCPP_INFO(this->get_logger(), "Validación exitosa: herramienta activa ahora es '%s'", requested_tool.c_str());
      }
    }
  }
  return result;
}

// Generador de trayectoria
std::vector<std::vector<double>> MoveArmNode::get_trajectory_moveJ(geometry_msgs::msg::Pose target_pose) {
  std::vector<std::vector<double>> trajectory;
  auto request = std::make_shared<robot_interfaces::srv::SolveIK::Request>();
  
  request->target_pose = target_pose; 
  request->origin_joint_angles = current_angles_;
  
  std::string current_tool_name = this->get_parameter("active_tool").as_string();
  auto current_tcp = tool_library_[current_tool_name];
  request->tcp_offset = current_tcp.offset;
  request->tool_dimensions = current_tcp.dimensions;
  request->tool_type = current_tcp.type;

  if (!ik_client_->wait_for_service(std::chrono::seconds(2))) {
    RCLCPP_ERROR(this->get_logger(), "El servicio IK no responde. Abortando trayectoria.");
    return trajectory;
  }

  auto result_future = ik_client_->async_send_request(request);
  auto response = result_future.get();

  if (!response->success || response->joint_angles.empty()) {
    RCLCPP_ERROR(this->get_logger(), "No se encontró solución IK.");
    return trajectory;
  }

  std::vector<double> target_angles = response->joint_angles;
  double max_step_rad = 10.0 * PI / 180.0;
  double max_diff = 0.0;
  


  for (size_t i = 0; i < current_angles_.size() && i < target_angles.size(); ++i) {
    double diff = std::abs(target_angles[i] - current_angles_[i]);
    if (diff > max_diff) max_diff = diff;
  }

  int steps = std::max(1, static_cast<int>(std::ceil(max_diff / max_step_rad)));
  RCLCPP_INFO(this->get_logger(), "Generando trayectoria de %d pasos...", steps);

  for (int step = 1; step <= steps; ++step) {
    std::vector<double> step_angles;
    for (size_t i = 0; i < current_angles_.size(); ++i) {
      double percent = static_cast<double>(step) / steps;
      double ang = current_angles_[i] + (target_angles[i] - current_angles_[i]) * percent;
      step_angles.push_back(ang);
    }
    trajectory.push_back(step_angles);
  }
  RCLCPP_INFO(this->get_logger(), "Angulos:");
  for (size_t i = 0; i < target_angles.size(); ++i) {
      RCLCPP_INFO(this->get_logger(), "  Joint %zu: %f rad", i, target_angles[i]);
  }
  
  //current_angles_ = target_angles;
  return trajectory;
}

// Generador de trayectoria
std::vector<std::vector<double>> MoveArmNode::get_trajectory_moveL(Point target) {
  std::vector<std::vector<double>> trajectory;
  if (current_angles_.empty()) return trajectory;

  // 1. Obtener Pose actual (Posición + Orientación real)
  Point start = calculate_dk(current_angles_); 
  
  // 2. Fijar la orientación del inicio para toda la línea
  tf2::Quaternion q_fixed;
  q_fixed.setRPY(start.roll, start.pitch, start.yaw);

  double distance = std::sqrt(std::pow(target.x - start.x, 2) + 
                              std::pow(target.y - start.y, 2) + 
                              std::pow(target.z - start.z, 2));
  
  double step_size = 0.02; // 2cm por paso
  int steps = std::max(1, static_cast<int>(std::ceil(distance / step_size)));

  if (!ik_client_->wait_for_service(std::chrono::seconds(2))) return trajectory;
  std::vector<double> current_angles_1 = current_angles_;
  for (int step = 1; step <= steps; ++step) {
    double percent = static_cast<double>(step) / steps;
    auto request = std::make_shared<robot_interfaces::srv::SolveIK::Request>();
    
    // Interpolación lineal de posición
    request->target_pose.position.x = start.x + (target.x - start.x) * percent;
    request->target_pose.position.y = start.y + (target.y - start.y) * percent;
    request->target_pose.position.z = start.z + (target.z - start.z) * percent;
    
    // Orientación fija (la herramienta no "baila")
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
        // Filtro de seguridad
        for (size_t j = 0; j < angles.size(); ++j) {
          angles[j] = std::clamp(atan2(sin(angles[j]), cos(angles[j])), -PI/2.0, PI/2.0);
        }
        current_angles_1= angles;
        trajectory.push_back(angles);
      } else {
        RCLCPP_WARN(this->get_logger(), "Trayectoria MoveL bloqueada por colisión/límites.");
        return {};
      }
    } else {
      RCLCPP_ERROR(this->get_logger(), "Timeout: El servidor IK no respondió a tiempo. Abortando trayectoria.");
      return {}; 
    }
  }
  //if (!trajectory.empty()) current_angles_ = trajectory.back(); 
  return trajectory;
}

// Callbacks de la acción
rclcpp_action::GoalResponse MoveArmNode::handle_goal_moveJ(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const NavigateToPose::Goal> goal) {
  RCLCPP_INFO(this->get_logger(), "Meta recibida: x=%.2f y=%.2f z=%.2f",
              goal->pose.pose.position.x,
              goal->pose.pose.position.y,
              goal->pose.pose.position.z);
  (void)uuid;
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse MoveArmNode::handle_cancel_moveJ(
    const std::shared_ptr<GoalHandleNav> goal_handle) {
  (void)goal_handle;
  RCLCPP_INFO(this->get_logger(), "Cancelación solicitada.");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void MoveArmNode::handle_accepted_moveJ(const std::shared_ptr<GoalHandleNav> goal_handle) {
  std::thread{std::bind(&MoveArmNode::execute_moveJ, this, std::placeholders::_1), goal_handle}.detach();
}

void MoveArmNode::execute_moveJ(const std::shared_ptr<GoalHandleNav> goal_handle) {
  const auto goal = goal_handle->get_goal();
  auto feedback = std::make_shared<NavigateToPose::Feedback>();
  auto result = std::make_shared<NavigateToPose::Result>();

  // Le pasamos el Pose directamente desde el goal
  auto trajectory_angles = get_trajectory_moveJ(goal->pose.pose);

  if (trajectory_angles.empty()) {
    RCLCPP_ERROR(this->get_logger(), "Trayectoria vacía. Abortando acción.");
    goal_handle->abort(result);
    return;
  }

  for (const auto & angles : trajectory_angles) {
    if (goal_handle->is_canceling()) {
      goal_handle->canceled(result);
      RCLCPP_INFO(this->get_logger(), "Trayectoria cancelada.");
      return;
    }

    std_msgs::msg::Int16MultiArray cmd_msg;
    for (double angle_rad : angles) {
      int16_t angle_deg = static_cast<int16_t>(std::round(angle_rad * 180.0 / PI));
      cmd_msg.data.push_back(angle_deg);
    }

    publisher_->publish(cmd_msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Point current_xyz = calculate_dk(angles); 
    feedback->current_pose.pose.position.x = current_xyz.x;
    feedback->current_pose.pose.position.y = current_xyz.y;
    feedback->current_pose.pose.position.z = current_xyz.z;

    goal_handle->publish_feedback(feedback);
  }

  if (rclcpp::ok()) {
    RCLCPP_INFO(this->get_logger(), "Angulos joint:");
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Trayectoria completada y Arduino posicionado!");
  }

}



// Callbacks de la acción
rclcpp_action::GoalResponse MoveArmNode::handle_goal_moveL(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const NavigateToPose::Goal> goal) {
  RCLCPP_INFO(this->get_logger(), "Meta recibida: x=%.2f y=%.2f z=%.2f",
              goal->pose.pose.position.x,
              goal->pose.pose.position.y,
              goal->pose.pose.position.z);
  (void)uuid;
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse MoveArmNode::handle_cancel_moveL(
    const std::shared_ptr<GoalHandleNav> goal_handle) {
  (void)goal_handle;
  RCLCPP_INFO(this->get_logger(), "Cancelación solicitada.");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void MoveArmNode::handle_accepted_moveL(const std::shared_ptr<GoalHandleNav> goal_handle) {
  std::thread{std::bind(&MoveArmNode::execute_moveL, this, std::placeholders::_1), goal_handle}.detach();
}

void MoveArmNode::execute_moveL(const std::shared_ptr<GoalHandleNav> goal_handle) {
  const auto goal = goal_handle->get_goal();
  auto feedback = std::make_shared<NavigateToPose::Feedback>();
  auto result = std::make_shared<NavigateToPose::Result>();

  Point target_pt = {
    goal->pose.pose.position.x,
    goal->pose.pose.position.y,
    goal->pose.pose.position.z
  };

  auto trajectory_angles = get_trajectory_moveL(target_pt);

  if (trajectory_angles.empty()) {
    RCLCPP_ERROR(this->get_logger(), "Trayectoria vacía. Abortando acción.");
    goal_handle->abort(result);
    return;
  }

  for (const auto & angles : trajectory_angles) {
    if (goal_handle->is_canceling()) {
      goal_handle->canceled(result);
      RCLCPP_INFO(this->get_logger(), "Trayectoria cancelada.");
      return;
    }

    std_msgs::msg::Int16MultiArray cmd_msg;
    for (double angle_rad : angles) {
      int16_t angle_deg = static_cast<int16_t>(std::round(angle_rad * 180.0 / PI));
      cmd_msg.data.push_back(angle_deg);
    }

    publisher_->publish(cmd_msg);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Point current_xyz = calculate_dk(angles); 
    feedback->current_pose.pose.position.x = current_xyz.x;
    feedback->current_pose.pose.position.y = current_xyz.y;
    feedback->current_pose.pose.position.z = current_xyz.z;

    goal_handle->publish_feedback(feedback);
  }

  if (rclcpp::ok()) {
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Trayectoria completada y Arduino posicionado!");
  }
}

int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<MoveArmNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}