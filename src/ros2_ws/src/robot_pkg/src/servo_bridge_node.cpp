#include "robot_pkg/servo_bridge_node.hpp"

#include <chrono>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <vector> 
#include <cstdio> 
#include <sstream>

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>

using std::placeholders::_1;
using namespace std::chrono_literals;

ServoBridgeNode::ServoBridgeNode()
: Node("servo_bridge_node"), serial_port_(-1)
{
  const char * port_name = "/dev/ttyACM0"; 
  
  serial_port_ = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (serial_port_ < 0) {
    RCLCPP_ERROR(this->get_logger(), "Error fatal: No se puede abrir %s", port_name);
  } else {
    fcntl(serial_port_, F_SETFL, 0); 
    configure_serial(serial_port_);
    RCLCPP_INFO(this->get_logger(), "Arduino conectado en %s", port_name);
  }

  this->declare_parameter("num_joints", 5);
  num_joints_ = this->get_parameter("num_joints").as_int();

  rclcpp::QoS qos_sub(1);
  qos_sub.transient_local();
  subscription_ = this->create_subscription<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 
    qos_sub, 
    std::bind(&ServoBridgeNode::robot_cmd_callback, this, _1)
  );
  
  rclcpp::QoS qos_profile(1); 
  qos_profile.transient_local();  
  qos_profile.reliable();        
  publisher_ = this->create_publisher<std_msgs::msg::Int16MultiArray>(
    "robot_cmd", 
    qos_profile
  );

  // Inicialización de ambos clientes
  add_wall_client_ = this->create_client<robot_interfaces::srv::AddObstacle>("/add_wall");
  dk_client_ = this->create_client<robot_interfaces::srv::SolveDK>("/solve_dk"); // AÑADIDO

  timer_ = this->create_wall_timer(
    20ms, std::bind(&ServoBridgeNode::timer_callback, this));

  RCLCPP_INFO(this->get_logger(), "Servo bridge working");
}

ServoBridgeNode::~ServoBridgeNode() {
  if (serial_port_ >= 0) close(serial_port_);
}

// --- COM: ROS2 -> ARDUINO ---
void ServoBridgeNode::robot_cmd_callback(const std_msgs::msg::Int16MultiArray::SharedPtr msg)
{
  if (serial_port_ < 0) return;
  if (robot_mode != 1 ) return;

  if (msg->data.size() != num_joints_) { 
    RCLCPP_WARN(this->get_logger(), "Ignorando comando: Se esperaban %zu angulos", num_joints_);
    return; 
  }

  std::string command = "";
  for (size_t i = 0; i < msg->data.size(); ++i) {
    command += std::to_string(msg->data[i] +90);
    if (i < msg->data.size() - 1) command += ","; 
  }
  command += "\n"; 
  RCLCPP_INFO(this->get_logger(), "comand: angles %s",command.c_str());

  write(serial_port_, command.c_str(), command.size());
}

// --- COM: ARDUINO -> ROS2  ---
void ServoBridgeNode::timer_callback()
{
  if (serial_port_ < 0) return;

  char buffer[256];
  int n = read(serial_port_, buffer, sizeof(buffer) - 1);

  if (n > 0) {
    buffer[n] = '\0'; 
    read_buffer_ += buffer; 

    if (read_buffer_.size() > 1000) {
      RCLCPP_WARN(this->get_logger(), "¡Buffer saturado! Descartando datos viejos para evitar lag.");
      read_buffer_.clear(); // Vaciamos el cubo
      return;               // Salimos y esperamos a la siguiente lectura limpia
    }

    size_t pos = read_buffer_.find('\n');
    while (pos != std::string::npos) {
      std::string line = read_buffer_.substr(0, pos);
      read_buffer_.erase(0, pos + 1); 
      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (line.find("MANUAL") != std::string::npos) {
        RCLCPP_INFO(this->get_logger(), "Arduino: MANUAL MODE ACTIVATED");
        robot_mode = 0;
      }
      else if (line.find("AUTO") != std::string::npos) {
        RCLCPP_INFO(this->get_logger(), "Arduino: AUTOMATIC MODE ACTIVATED");
        robot_mode = 1;
      }
      else if (robot_mode == 0) {
        
        bool is_wall = false;
        std::string data_str = line;

        // Detectar si es un punto de pared y extraer solo los números
        if (line.find("WALL(") == 0) {
          is_wall = true;
          // Quita "WALL" y el ")" del final
          data_str = line.substr(5, line.length() - 6); 
        }
        data_str.erase(std::remove(data_str.begin(), data_str.end(), '('), data_str.end());
        data_str.erase(std::remove(data_str.begin(), data_str.end(), ')'), data_str.end());

        std::vector<int> v;
        std::stringstream ss(data_str);
        std::string token;

        while (std::getline(ss, token, ',')) {
          try {
            v.push_back(std::stoi(token) - 90);
          } catch (...) {
            RCLCPP_WARN(this->get_logger(), "Token inválido: '%s'", token.c_str());
            continue;
          }
        }

        // Si la cantidad de números leídos coincide con num_joints_
        std::stringstream debug;

        for (size_t i = 0; i < v.size(); i++) {
            debug << v[i];

            if (i < v.size() - 1) {
                debug << ", ";
            }
        }

        RCLCPP_INFO(this->get_logger(), "size: %zu | values: [%s]",
                    v.size(),
                    debug.str().c_str());

        if (v.size() == num_joints_) {
          
          if (is_wall) {
            RCLCPP_INFO(this->get_logger(), "Wall point received");
            add_wall_point(v); 
          }
          
          std_msgs::msg::Int16MultiArray msg_out;
          msg_out.data.assign(v.begin(), v.end()); // Asignar el vector dinámico
          publisher_->publish(msg_out);

        } else {
          RCLCPP_WARN(this->get_logger(), "Formato incorrecto o cantidad de joints no coincide: %s", line.c_str());
        }
      }
      else {
        RCLCPP_INFO(this->get_logger(), "Arduino: UNKNOWN COMMAND");
      } 

      pos = read_buffer_.find('\n');
    }
  }
}

// --- LÓGICA ASÍNCRONA PARA AÑADIR PARED ---
void ServoBridgeNode::add_wall_point(const std::vector<int>& v) { // FIRMA ACTUALIZADA
  
  // Pasar de vector<int> a vector<double> automáticamente
  std::vector<double> angles(v.begin(), v.end());

  // 1. Configurar la petición de Cinemática (SolveDK)
  Tool_Config default_cfg;
  default_cfg.name = "default";
  default_cfg.type = 1; 
  default_cfg.dimensions = {0.01, 0.01}; 
  
  geometry_msgs::msg::Pose default_pose; // Pose vacía en ceros (CORREGIDO)
  default_cfg.offset = default_pose;

  auto request_dk = std::make_shared<robot_interfaces::srv::SolveDK::Request>();
  request_dk->joint_angles = angles;
  request_dk->tcp_offset = default_cfg.offset;
  request_dk->tool_dimensions = default_cfg.dimensions;

  if (!dk_client_->wait_for_service(std::chrono::seconds(2))) {
    RCLCPP_ERROR(this->get_logger(), "El servicio DK no responde. Abortando pared.");
    return;
  }

  // 2. Enviar petición asíncrona a DK
  dk_client_->async_send_request(request_dk, 
    [this](rclcpp::Client<robot_interfaces::srv::SolveDK>::SharedFuture future_dk) {
      
      // Dentro de este callback, .get() es totalmente seguro porque ya resolvió
      auto response_dk = future_dk.get();

      if (response_dk && response_dk->success) {
        
        // Extraer RPY usando TF2
        double roll, pitch, yaw;
        tf2::Quaternion q(
          response_dk->target_pose.orientation.x,
          response_dk->target_pose.orientation.y,
          response_dk->target_pose.orientation.z,
          response_dk->target_pose.orientation.w
        );
        tf2::Matrix3x3(q).getRPY(roll, pitch, yaw);

        // 3. Crear petición para AddObstacle
        auto request_wall = std::make_shared<robot_interfaces::srv::AddObstacle::Request>();
        request_wall->name = "pared_arduino_" + std::to_string(wall_count_);
        request_wall->x = response_dk->target_pose.position.x;
        request_wall->y = response_dk->target_pose.position.y;
        request_wall->z = response_dk->target_pose.position.z;
        request_wall->roll = roll;
        request_wall->pitch = pitch;
        request_wall->yaw = yaw;
        
        // Dimensiones: 1m x 1m x 5cm
        request_wall->width = 1.0;  
        request_wall->height = 1.0; 
        request_wall->depth = 0.05; 

        if (!add_wall_client_->wait_for_service(std::chrono::seconds(2))) {
          RCLCPP_ERROR(this->get_logger(), "El servicio /add_wall no responde.");
          return;
        }

        // 4. Enviar petición asíncrona a AddObstacle
        add_wall_client_->async_send_request(request_wall, 
          [this](rclcpp::Client<robot_interfaces::srv::AddObstacle>::SharedFuture future_wall) {
            
            auto response = future_wall.get();
            
            if (response && response->success) {
              RCLCPP_INFO(this->get_logger(), "Pared %d añadida rotada correctamente.", wall_count_);
              wall_count_++; 
            } else {
              RCLCPP_ERROR(this->get_logger(), "Fallo al añadir la pared en el simulador.");
            }

          }
        );

      } else {
        RCLCPP_ERROR(this->get_logger(), "Fallo en la respuesta del servicio DK.");
      }
    }
  );
}

// --- SERIAL CONFIGURATION  ---
void ServoBridgeNode::configure_serial(int fd) {
  struct termios tty;
  if(tcgetattr(fd, &tty) != 0) return;
  
  cfsetospeed(&tty, B57600);
  cfsetispeed(&tty, B57600);
  
  tty.c_cflag &= ~PARENB; 
  tty.c_cflag &= ~CSTOPB; 
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;     
  tty.c_cflag |= CREAD | CLOCAL; 

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;
  tty.c_lflag &= ~ECHOE;
  tty.c_lflag &= ~ISIG;
  
  tty.c_cc[VMIN]  = 0; 
  tty.c_cc[VTIME] = 0; 

  tcsetattr(fd, TCSANOW, &tty);
  
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// --- MAIN ---
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ServoBridgeNode>());
  rclcpp::shutdown();
  return 0;
}