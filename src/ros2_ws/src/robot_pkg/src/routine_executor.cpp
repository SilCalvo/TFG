#include "robot_pkg/routine_executor.hpp" 

RoutineExecutor::RoutineExecutor() : Node("routine_executor_node"), current_step_(0) {
    
  this->declare_parameter<std::string>("archivo", "test_moveL.yaml");
  std::string file_name = this->get_parameter("archivo").as_string();
  std::string pkg_share_dir = ament_index_cpp::get_package_share_directory("robot_pkg");
  std::string file_path;

  if (!file_name.empty() && file_name.front() == '/') {
    file_path = file_name;
  } else {
    file_path = pkg_share_dir + "/rutines/" + file_name;
  }

  RCLCPP_INFO(this->get_logger(), "Intentando leer la rutina desde: %s", file_path.c_str());

  // Iniciar clientes
  client_moveJ_ = rclcpp_action::create_client<NavigateToPose>(this, "/moveJ");
  client_moveL_ = rclcpp_action::create_client<NavigateToPose>(this, "/moveL");
  
  client_add_wall_ = this->create_client<robot_interfaces::srv::AddObstacle>("add_wall");
  client_remove_wall_ = this->create_client<robot_interfaces::srv::RemoveObstacle>("remove_wall");
  
  client_add_tool_ = this->create_client<robot_interfaces::srv::ManageTool>("add_tool");
  client_delete_tool_ = this->create_client<robot_interfaces::srv::ManageTool>("delete_tool");

  // Cliente de parámetros apuntando explícitamente a tu "move_action_server"
  param_client_ = std::make_shared<rclcpp::AsyncParametersClient>(this, "move_action_server");
  RCLCPP_INFO(this->get_logger(), "Sincronizando con el servidor de parámetros...");
  if (!param_client_->wait_for_service(std::chrono::seconds(5))) {
    RCLCPP_ERROR(this->get_logger(), "No se encontró el servidor de parámetros de move_action_server.");
    rclcpp::shutdown();
    exit(1);
    return;
  }

  RCLCPP_INFO(this->get_logger(), " Conexión de parámetros establecida.");
  load_routine(file_path);
  execute_next_step();
}

void RoutineExecutor::load_routine(const std::string& file_path) {
  try {
    YAML::Node config = YAML::LoadFile(file_path);
    if (!config["rutina"]) {
      RCLCPP_ERROR(this->get_logger(), "Falta la clave 'rutina' en el YAML.");
      rclcpp::shutdown(); return;
    }

    for (const auto& node : config["rutina"]) {
      RoutineStep step;
      step.type = node["tipo"].as<std::string>();

      // --- 1. MOVIMIENTO ---
      if (step.type == "moveJ" || step.type == "moveL") {
        step.x = node["x"] ? node["x"].as<double>() : 0.0;
        step.y = node["y"] ? node["y"].as<double>() : 0.0;
        step.z = node["z"] ? node["z"].as<double>() : 0.0;
        step.qx = node["qx"] ? node["qx"].as<double>() : 0.0;
        step.qy = node["qy"] ? node["qy"].as<double>() : 0.0;
        step.qz = node["qz"] ? node["qz"].as<double>() : 0.0;
        step.qw = node["qw"] ? node["qw"].as<double>() : 1.0;
      }
      // --- 2. AÑADIR PARED ---
      else if (step.type == "add_wall") {
        step.name = node["name"] ? node["name"].as<std::string>() : "muro_default";
        step.x = node["x"] ? node["x"].as<double>() : 0.0;
        step.y = node["y"] ? node["y"].as<double>() : 0.0;
        step.z = node["z"] ? node["z"].as<double>() : 0.0;
        step.width = node["width"] ? node["width"].as<double>() : 0.1;
        step.depth = node["depth"] ? node["depth"].as<double>(): 0.1;
        step.height = node["height"] ? node["height"].as<double>() : 0.1;
      }
      // --- 3. QUITAR PARED ---
      else if (step.type == "remove_wall") {
        step.name = node["name"].as<std::string>();
      }
      // --- 4. AÑADIR HERRAMIENTA ---
      else if (step.type == "add_tool") {
        step.name = node["name"].as<std::string>();
        step.tool_type = node["tool_type"] ? node["tool_type"].as<int>() : 1;
        
        if (node["dimensions"]) {
          for (const auto& d : node["dimensions"]) {
            step.dimensions.push_back(d.as<double>());
          }
        }
        
        step.off_x = node["off_x"] ? node["off_x"].as<double>() : 0.0;
        step.off_y = node["off_y"] ? node["off_y"].as<double>() : 0.0;
        step.off_z = node["off_z"] ? node["off_z"].as<double>() : 0.0;
        step.off_qw = node["off_qw"] ? node["off_qw"].as<double>() : 1.0;
      }
      // --- 5. BORRAR O SELECCIONAR HERRAMIENTA ---
      else if (step.type == "delete_tool" || step.type == "set_tool") {
        step.name = node["name"].as<std::string>();
      }
      routine_steps_.push_back(step);
    }
    RCLCPP_INFO(this->get_logger(), "Rutina cargada. Pasos totales: %zu", routine_steps_.size());
  } catch (const YAML::Exception& e) {
    RCLCPP_ERROR(this->get_logger(), "Error YAML: %s", e.what());
    rclcpp::shutdown();
  }
}

void RoutineExecutor::execute_next_step() {
  if (current_step_ >= routine_steps_.size()) {
    RCLCPP_INFO(this->get_logger(), "RUTINA COMPLETADA CON ÉXITO.");
    rclcpp::shutdown();
    return;
  }

  const auto& step = routine_steps_[current_step_];
  RCLCPP_INFO(this->get_logger(), " [%zu/%zu] %s", current_step_ + 1, routine_steps_.size(), step.type.c_str());

  // --- 1. MOVIMIENTO ---
  if (step.type == "moveJ" || step.type == "moveL") {
    auto client = (step.type == "moveJ") ? client_moveJ_ : client_moveL_;
    client->wait_for_action_server();
    auto goal_msg = NavigateToPose::Goal();
    goal_msg.pose.header.frame_id = "base_link";
    goal_msg.pose.pose.position.x = step.x; goal_msg.pose.pose.position.y = step.y; goal_msg.pose.pose.position.z = step.z;
    goal_msg.pose.pose.orientation.x = step.qx; goal_msg.pose.pose.orientation.y = step.qy; goal_msg.pose.pose.orientation.z = step.qz; goal_msg.pose.pose.orientation.w = step.qw;

    auto send_options = rclcpp_action::Client<NavigateToPose>::SendGoalOptions();
    send_options.goal_response_callback = std::bind(&RoutineExecutor::goal_response_callback, this, std::placeholders::_1);
    send_options.result_callback = std::bind(&RoutineExecutor::result_callback, this, std::placeholders::_1);
    client->async_send_goal(goal_msg, send_options);
  }
  // --- 2. OBSTÁCULOS ---
  else if (step.type == "add_wall") {
    auto request = std::make_shared<robot_interfaces::srv::AddObstacle::Request>();
    request->name = step.name; request->x = step.x; request->y = step.y; request->z = step.z;
    request->width = step.width; request->depth = step.depth; request->height = step.height;
    
    client_add_wall_->async_send_request(request, [this](rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedFuture future) {
      if (future.get()->success) { 
        this->current_step_++; this->execute_next_step(); 
      }
    });
  }
  else if (step.type == "remove_wall") {
    auto request = std::make_shared<robot_interfaces::srv::RemoveObstacle::Request>();
    request->name = step.name;
    
    client_remove_wall_->async_send_request(request, [this](rclcpp::Client<robot_interfaces::srv::RemoveObstacle>::SharedFuture future) {
      if (future.get()->success) { 
        this->current_step_++; this->execute_next_step();
      }
    });
  }
  // --- 3. GESTIÓN DE HERRAMIENTAS ---
  else if (step.type == "add_tool") {
    auto request = std::make_shared<robot_interfaces::srv::ManageTool::Request>();
    request->name = step.name;
    request->type = step.tool_type;
    request->dimensions = step.dimensions;
    request->offset.position.x = step.off_x; request->offset.position.y = step.off_y; request->offset.position.z = step.off_z;
    request->offset.orientation.w = step.off_qw; 
    
    client_add_tool_->async_send_request(request, [this](rclcpp::Client<robot_interfaces::srv::ManageTool>::SharedFuture future) {
      if (future.get()->success) { 
        this->current_step_++; this->execute_next_step(); 
      }
    });
  }
  else if (step.type == "delete_tool") {
    auto request = std::make_shared<robot_interfaces::srv::ManageTool::Request>();
    request->name = step.name;
    
    client_delete_tool_->async_send_request(request, [this](rclcpp::Client<robot_interfaces::srv::ManageTool>::SharedFuture future) {
      if (future.get()->success) {
        this->current_step_++; this->execute_next_step(); 
      }
    });
  }
  // --- 4. CAMBIAR HERRAMIENTA ACTIVA ---
  else if (step.type == "set_tool") {

      if (!param_client_->service_is_ready()) {
          RCLCPP_ERROR(this->get_logger(), "El servidor de parámetros no está listo.");
          rclcpp::shutdown();
          return;
      }
      param_client_->set_parameters({rclcpp::Parameter("active_tool", step.name)}, 
          [this](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> future) {
              auto results = future.get();
              if (results[0].successful) {
                  RCLCPP_INFO(this->get_logger(), " Herramienta cambiada a '%s'", this->routine_steps_[this->current_step_].name.c_str());
                  this->current_step_++;
                  this->execute_next_step();
              } else {
                  RCLCPP_ERROR(this->get_logger(), " Fallo al establecer herramienta.");
                  rclcpp::shutdown();
              }
          }
      );
  }
}

void RoutineExecutor::goal_response_callback(const GoalHandleNavigate::SharedPtr& goal_handle) {
  if (!goal_handle) { RCLCPP_ERROR(this->get_logger(), " Meta rechazada."); rclcpp::shutdown(); }
}
void RoutineExecutor::result_callback(const GoalHandleNavigate::WrappedResult& result) {
  if (result.code == rclcpp_action::ResultCode::SUCCEEDED) {
    current_step_++; 
    execute_next_step();
  } else {
    RCLCPP_ERROR(this->get_logger(), " Error en movimiento."); rclcpp::shutdown();
  }
}

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<RoutineExecutor>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}