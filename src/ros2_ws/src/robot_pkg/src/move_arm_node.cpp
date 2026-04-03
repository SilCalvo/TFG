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

  geometry_msgs::msg::Pose default_pose;
  default_pose.position.x = 0.0;
  default_pose.position.y = 0.0;
  default_pose.position.z = 0.0; 
  default_pose.orientation.w = 1.0; 
  add_tool("default", 1, {0.25, 0.05}, default_pose);
  active_tool_name_ = "default";

  RCLCPP_INFO(this->get_logger(), "Nodo 'move_arm_node' inicializado. Publicando en 'robot_cmd'.");
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
  
  auto current_tcp = tool_library_[active_tool_name_];
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

void MoveArmNode::add_tool(std::string name, int type, std::vector<double> dims, geometry_msgs::msg::Pose off) {
    Tool_Config new_tool;
    new_tool.name = name;
    new_tool.type = type;
    new_tool.dimensions = dims;
    new_tool.offset = off;

    tool_library_[name] = new_tool;
    RCLCPP_INFO(this->get_logger(), "Herramienta '%s' añadida a la librería.", name.c_str());
}

// Borrar una herramienta por nombre
void MoveArmNode::delete_tool(std::string name) {
    if (tool_library_.find(name) != tool_library_.end()) {
        tool_library_.erase(name);
        RCLCPP_INFO(this->get_logger(), "Herramienta '%s' eliminada.", name.c_str());
        
        // Si borramos la activa, volvemos a una por defecto
        if (active_tool_name_ == name) active_tool_name_ = "default";
    } else {
        RCLCPP_WARN(this->get_logger(), "No se pudo borrar: '%s' no existe.", name.c_str());
    }
}

// Seleccionar cuál usar en el próximo movimiento
void MoveArmNode::set_active_tool(std::string name) {
    if (tool_library_.count(name)) {
        active_tool_name_ = name;
        RCLCPP_INFO(this->get_logger(), "Herramienta activa cambiada a: %s", name.c_str());
    } else {
        RCLCPP_ERROR(this->get_logger(), "Error: La herramienta '%s' no existe en la librería.", name.c_str());
    }
}

// Generador de trayectoria
std::vector<std::vector<double>> MoveArmNode::get_trajectory_moveJ(geometry_msgs::msg::Pose target_pose) {
  std::vector<std::vector<double>> trajectory;
  auto request = std::make_shared<robot_interfaces::srv::SolveIK::Request>();
  
  request->target_pose = target_pose; 
  
  auto current_tcp = tool_library_[active_tool_name_];
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

  current_angles_ = target_angles;
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

    auto current_tcp = tool_library_[active_tool_name_];
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
        trajectory.push_back(angles);
      } else {
        RCLCPP_WARN(this->get_logger(), "Trayectoria MoveL bloqueada por colisión/límites.");
        return {};
      }
    }
  }
  if (!trajectory.empty()) current_angles_ = trajectory.back(); 
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